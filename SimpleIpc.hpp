#ifndef SIMPLEIPC_HPP
#define SIMPLEIPC_HPP

#include <cstdint>

#include "IpcMessage.hpp"
#include "SharedMem.hpp"
#include "SimpleUtils/RelativeAllocator.hpp"
#include "SimpleUtils/RingBuffer.hpp"

namespace kiss
{

class SystemWideLockIf;

class SimpleIpc
{
  public:
    enum class Result
    {
        OK,
        ERR_GENERAL,
        ERR_NO_MSG,
        ERR_BUFFER_TOO_SMALL
    };

    SimpleIpc(SharedMem &sharedMemoryMetadata, SharedMem &sharedMemoryData, SystemWideLockIf &mSystemWideLock);

    Result sendMessage(uint8_t *const message, const uint32_t messageSize);
    Result receiveMessage(uint8_t *const message, const uint32_t &messageSize);

  private:
    SharedMem &mSharedMemoryData;
    SystemWideLockIf &mSystemWideLock;

    RelativeAllocator mSharedMemBufferAllocator;
    RingBuffer<IpcMessage, 100> *mRbProducerConsumerQueue; // placed in shared memory
};

} // namespace kiss

#endif // SIMPLEIPC_HPP
