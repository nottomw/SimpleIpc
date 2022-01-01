#include "IpcMessage.hpp"
#include "RingBuffer.hpp"
#include "SimpleIpc.hpp"

#include <cassert>
#include <cerrno>
#include <cstdio>
#include <new>

namespace kiss
{

SimpleIpc::SimpleIpc(SharedMem &sharedMemory, SystemWideLockIf &systemWideLock) //
    : mSharedMemory(sharedMemory),                                              //
      mSystemWideLock(systemWideLock),                                          //
      mSharedMemBufferAllocator(mSharedMemory.mSharedMemoryAddr + sizeof(RingBuffer<IpcMessage, 100>), 1024)
{
    // TODO: correct sizes of allocator region and queue
    mRbProducerConsumerQueue = //
        new (mSharedMemory.mSharedMemoryAddr) RingBuffer<IpcMessage, 100>;
}

SimpleIpc::Result SimpleIpc::sendMessage(uint8_t *const message, const uint32_t messageSize)
{
    void *pushedAddr = mSharedMemBufferAllocator.alloc(messageSize);
    if (pushedAddr == nullptr)
    {
        // TODO
        return Result::ERR_GENERAL;
    }

    (void)memcpy(pushedAddr, message, messageSize);

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
    IpcMessage *receivedMessage = nullptr;
    const auto queuePopRes = mRbProducerConsumerQueue->pop(receivedMessage);
    if (queuePopRes != RingBufferResult::OK)
    {
        return Result::ERR_GENERAL;
    }

    // TODO: size check
    assert(receivedMessage->mMessageSize <= messageSize);

    (void)memcpy(message, receivedMessage->mMessageData, messageSize);

    mSharedMemBufferAllocator.dealloc(receivedMessage->mMessageData);

    return Result::OK;
}

} // namespace kiss
