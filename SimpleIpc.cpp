#include "IpcMessage.hpp"
#include "SimpleIpc.hpp"
#include "SimpleUtils/RingBuffer.hpp"
#include "SystemWideLockIf.hpp"

#include <cassert>
#include <cerrno>
#include <cstdio>
#include <new>

namespace kiss
{

SimpleIpc::SimpleIpc(SharedMem &sharedMemoryMetadata, SharedMem &sharedMemoryData, SystemWideLockIf &systemWideLock) //
    : mSharedMemoryData(sharedMemoryData),                                                                           //
      mSystemWideLock(systemWideLock),                                                                               //
      mSharedMemBufferAllocator(sharedMemoryMetadata.mSharedMemoryAddr,                                              //
                                sharedMemoryMetadata.getSize() / 2U,                                                 //
                                mSharedMemoryData.getSize())
{
    // shared memory use:
    //  - [metadata] allocator metadata
    //  - [metadata + sizeof allocator meta] producer-consumer queue (ring buffer)
    //  - [data] data buffers

    // TODO: fix magic number: 100
    using RB = RingBuffer<IpcMessage, 100>;

    // divide the metadata size to two regions: queue & allocator
    const uint32_t queueShmemSize = sharedMemoryMetadata.getSize() / 2U;
    const uint32_t allocatorShmemSize = queueShmemSize;
    constexpr uint32_t allocatorMinShmemSize = 10U * sizeof(MemoryRegion);

    assert(allocatorShmemSize >= allocatorMinShmemSize);
    assert(sizeof(RB) <= queueShmemSize);

    uint8_t *const queueSharedMem = sharedMemoryMetadata.mSharedMemoryAddr + allocatorShmemSize;

    mRbProducerConsumerQueue = new (queueSharedMem) RB;
}

SimpleIpc::Result SimpleIpc::sendMessage(uint8_t *const message, const uint32_t messageSize)
{
    RelativeAllocator::RelativePtr pushedAddr{0};
    {
        LockScoped lk{mSystemWideLock};
        pushedAddr = mSharedMemBufferAllocator.alloc(messageSize);
    }

    void *addr = mSharedMemoryData.mSharedMemoryAddr + pushedAddr;
    (void)memcpy(addr, message, messageSize);

    IpcMessage messageToSend{pushedAddr, messageSize};

    RingBufferResult queuePushRes = RingBufferResult::OK;
    {
        LockScoped lk{mSystemWideLock};
        queuePushRes = mRbProducerConsumerQueue->push(&messageToSend);
    }

    if (queuePushRes != RingBufferResult::OK)
    {
        return Result::ERR_GENERAL;
    }

    return Result::OK;
}

SimpleIpc::Result SimpleIpc::receiveMessage(uint8_t *const message, const uint32_t &messageSize)
{
    IpcMessage receivedMessage;
    RingBufferResult queuePopRes = RingBufferResult::OK;

    {
        LockScoped lk{mSystemWideLock};
        queuePopRes = mRbProducerConsumerQueue->pop(&receivedMessage);
    }

    if (queuePopRes != RingBufferResult::OK)
    {
        return Result::ERR_GENERAL;
    }

    if (receivedMessage.mMessageSize > messageSize)
    {
        return Result::ERR_BUFFER_TOO_SMALL;
    }

    void *addr = mSharedMemoryData.mSharedMemoryAddr + receivedMessage.mMessageData;
    (void)memcpy(message, addr, messageSize);

    {
        LockScoped lk{mSystemWideLock};
        mSharedMemBufferAllocator.dealloc(receivedMessage.mMessageData);
    }

    return Result::OK;
}

} // namespace kiss
