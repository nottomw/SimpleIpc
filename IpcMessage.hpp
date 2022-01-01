#ifndef IPCMESSAGE_HPP
#define IPCMESSAGE_HPP

#include <cstdint>

namespace kiss
{

// This class is placed directly in shared memory.
class IpcMessage
{
  public:
    IpcMessage()
        : mMessageData(0), //
          mMessageSize(0)
    {
    }

    IpcMessage(uint32_t dataBuffer, const uint32_t dataSize) //
        : mMessageData(dataBuffer),                          //
          mMessageSize(dataSize)
    {
    }

  private:
    uint32_t mMessageData; // data = sharedMem base + mMessageData
    uint32_t mMessageSize;

    friend class SimpleIpc;
};

} // namespace kiss

#endif
