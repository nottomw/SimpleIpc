#include "IpcMessage.hpp"
#include "SimpleIpc.hpp"

#include <cassert>
#include <cerrno>
#include <cstdio>
#include <new>

namespace kiss
{

SimpleIpc::SimpleIpc(SharedMem &sharedMemory) //
    : mSharedMemory(sharedMemory)
{
    metadata = new (mSharedMemory.mSharedMemoryAddr) Metadata;
}

IpcMessage &SimpleIpc::messageBufferFetch(const uint32_t dataSize)
{
    uint32_t offsetTailAfterFetch = metadata->mOffsetTail + sizeof(IpcMessage) + dataSize;
    const uint32_t sharedMemorySize = mSharedMemory.getSize();

    if ((metadata->usedMemoryBytes + dataSize + sizeof(IpcMessage)) > sharedMemorySize)
    {
        // TODO: proper error return
        assert("out of memory" == nullptr);
    }

    assert(dataSize < sharedMemorySize);

    uint8_t *firstFreeMsgBlock = reinterpret_cast<uint8_t *>(metadata) + sizeof(Metadata);
    if (offsetTailAfterFetch > sharedMemorySize)
    {
        printf("SEND MSG: ring reset\n");
        const uint32_t offsetTailOnlyWithHeader = metadata->mOffsetTail + sizeof(IpcMessage);
        if (offsetTailOnlyWithHeader <= sharedMemorySize)
        {
            printf("SEND MSG: adding marker IpcMessage\n");
            // TODO: if header can fit in the rest of memory, add the header and special mark
            // that should reset ring
            IpcMessage *const msg = new (firstFreeMsgBlock + metadata->mOffsetTail) IpcMessage(dataSize);
            msg->mMsgState = IpcMessage::MsgState::SKIP;
        }

        // Intentionally waste some memory to keep the messages in continuous region.
        offsetTailAfterFetch = 0;

        // Make sure the block is free.
        // TODO: return proper error instead of assert
        const auto *const msgBlock = reinterpret_cast<IpcMessage *>(firstFreeMsgBlock);
        assert(msgBlock->mMsgState == IpcMessage::MsgState::CONSUMED);
    }
    else
    {
        firstFreeMsgBlock += metadata->mOffsetTail;
    }

    IpcMessage *const msg = new (firstFreeMsgBlock) IpcMessage(dataSize);

    metadata->mOffsetTail = offsetTailAfterFetch;
    metadata->usedMemoryBytes += dataSize + sizeof(IpcMessage);

    return *msg;
}

void SimpleIpc::sendMessage(IpcMessage &message)
{
    message.mMsgState = IpcMessage::MsgState::PRODUCED;
}

IpcMessage *SimpleIpc::receiveStart()
{
    // TODO: make sure no accumulating error from continuous memory hack

    printf("RECV START: head: %d, tail: %d\n", metadata->mOffsetHead, metadata->mOffsetTail);

    if (metadata->mOffsetHead != metadata->mOffsetTail)
    {
        uint8_t *const msgBlockFirst = mSharedMemory.mSharedMemoryAddr + sizeof(Metadata);
        IpcMessage *msgCurrent = reinterpret_cast<IpcMessage *>(msgBlockFirst + metadata->mOffsetHead);

        if ((metadata->mOffsetHead + sizeof(IpcMessage)) > mSharedMemory.getSize())
        {
            printf("RECV START: Using first block\n");
            msgCurrent = reinterpret_cast<IpcMessage *>(msgBlockFirst);
        }
        else
        {
            printf("RECV START: Using next block\n");
        }

        if (msgCurrent->mMsgState == IpcMessage::MsgState::PRODUCED)
        {
            return msgCurrent;
        }
        else if (msgCurrent->mMsgState == IpcMessage::MsgState::SKIP)
        {
            printf("RECV START: Skipping block\n");
            return reinterpret_cast<IpcMessage *>(msgBlockFirst);
        }
        else
        {
            printf("RECV START: invalid msg current state\n");
        }
    }

    return nullptr;
}

void SimpleIpc::receiveEnd(IpcMessage *const message)
{
    message->mMsgState = IpcMessage::MsgState::CONSUMED;
    metadata->mOffsetHead += (sizeof(IpcMessage) + message->mMessageSize);

    const uint32_t sharedMemorySize = mSharedMemory.getSize();
    if (metadata->mOffsetHead > sharedMemorySize)
    {
        printf("RECV MSG: ring reset\n");
        metadata->mOffsetHead = (sizeof(IpcMessage) + message->mMessageSize);
    }

    metadata->usedMemoryBytes -= (message->mMessageSize + sizeof(IpcMessage));
}

} // namespace kiss
