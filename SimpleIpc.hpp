#ifndef SIMPLEIPC_HPP
#define SIMPLEIPC_HPP

#include <cstdint>

#include "Allocator.hpp"
#include "IpcMessage.hpp"
#include "RingBuffer.hpp"
#include "SharedMem.hpp"

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
    Allocator mSharedMemBufferAllocator;
    RingBuffer<IpcMessage, 100> *mRbProducerConsumerQueue; // placed in shared memory
};

} // namespace kiss

#endif // SIMPLEIPC_HPP
