#ifndef SIMPLEIPC_HPP
#define SIMPLEIPC_HPP

#include <cstdint>

#include "IpcMessage.hpp"
#include "SharedMem.hpp"

namespace kiss
{

class SystemWideLockIf;

// TODO: cleanup messy API

class SimpleIpc
{
  public:
    SimpleIpc(SharedMem &sharedMemory, SystemWideLockIf &mSystemWideLock);

    IpcMessage &messageBufferFetch(const uint32_t dataSize);

    void sendMessage(IpcMessage &message);

    IpcMessage *receiveStart();
    void receiveEnd(IpcMessage *const message);

  private:
    struct Metadata
    {
        Metadata()
        {
            // TODO: proper init guard
            static bool initOnlyOnceGuard = false;
            if (!initOnlyOnceGuard)
            {
                initOnlyOnceGuard = true;
                usedMemoryBytes = sizeof(Metadata);
                mOffsetHead = 0;
                mOffsetTail = 0;
            }
        }

        // offsets from AFTER metadata
        uint32_t mOffsetHead; // TODO: needs to be IPC-atomic
        uint32_t mOffsetTail;
        uint32_t usedMemoryBytes;
    };

    Metadata *metadata;
    SharedMem &mSharedMemory;
    SystemWideLockIf &mSystemWideLock;
};

} // namespace kiss

#endif // SIMPLEIPC_HPP
