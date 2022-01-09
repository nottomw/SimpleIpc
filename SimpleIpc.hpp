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
        ERR_NO_MSG
    };

    SimpleIpc(SharedMem &sharedMemory, SystemWideLockIf &mSystemWideLock);

    Result sendMessage(uint8_t *const message, const uint32_t messageSize);
    Result receiveMessage(uint8_t *const message, const uint32_t &messageSize);

  private:
    SharedMem &mSharedMemory;
    SystemWideLockIf &mSystemWideLock;

    // TODO: sane sizes
    uint8_t *mSharedMemAllocatorRegion;
    RelativeAllocator mSharedMemBufferAllocator;
    RingBuffer<IpcMessage, 100> *mRbProducerConsumerQueue; // placed in shared memory
};

} // namespace kiss

#endif // SIMPLEIPC_HPP
