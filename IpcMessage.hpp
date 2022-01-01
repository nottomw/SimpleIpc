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
        : mMessageData(nullptr), //
          mMessageSize(0)
    {
    }

    IpcMessage(void *dataBuffer, const uint32_t dataSize) //
        : mMessageData(dataBuffer),                       //
          mMessageSize(dataSize)
    {
    }

  private:
    void *mMessageData;
    uint32_t mMessageSize;

    friend class SimpleIpc;
};

} // namespace kiss

#endif
