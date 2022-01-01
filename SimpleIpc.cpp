#include "IpcMessage.hpp"
#include "RingBuffer.hpp"
#include "SimpleIpc.hpp"

#include <cassert>
#include <cerrno>
#include <cstdio>
#include <new>

namespace kiss
{

SimpleIpc::SimpleIpc(SharedMem &sharedMemory, SystemWideLockIf &systemWideLock)                         //
    : mSharedMemory(sharedMemory),                                                                      //
      mSystemWideLock(systemWideLock),                                                                  //
      mSharedMemAllocatorRegion(mSharedMemory.mSharedMemoryAddr + sizeof(RingBuffer<IpcMessage, 100>)), //
      mSharedMemBufferAllocator(mSharedMemAllocatorRegion, 1024)
{
    // TODO: correct sizes of allocator region and queue
    mRbProducerConsumerQueue = //
        new (mSharedMemory.mSharedMemoryAddr) RingBuffer<IpcMessage, 100>;
}

SimpleIpc::Result SimpleIpc::sendMessage(uint8_t *const message, const uint32_t messageSize)
{
    Allocator::RelativePtr pushedAddr = mSharedMemBufferAllocator.alloc(messageSize);

    void *addr = mSharedMemAllocatorRegion + pushedAddr;
    (void)memcpy(addr, message, messageSize);

    IpcMessage messageToSend{pushedAddr, messageSize};
    // TODO: fix horrible address fetch
    const auto queuePushRes = mRbProducerConsumerQueue->push(&messageToSend);
    if (queuePushRes != RingBufferResult::OK)
    {
        return Result::ERR_GENERAL;
    }

    return Result::OK;
}

SimpleIpc::Result SimpleIpc::receiveMessage(uint8_t *const message, const uint32_t &messageSize)
{
    IpcMessage receivedMessage;
    const auto queuePopRes = mRbProducerConsumerQueue->pop(&receivedMessage);
    if (queuePopRes != RingBufferResult::OK)
    {
        return Result::ERR_GENERAL;
    }

    // TODO: size check
    assert(receivedMessage.mMessageSize <= messageSize);

    void *addr = mSharedMemAllocatorRegion + receivedMessage.mMessageData;
    (void)memcpy(message, addr, messageSize);

    mSharedMemBufferAllocator.dealloc(receivedMessage.mMessageData);

    return Result::OK;
}

} // namespace kiss
