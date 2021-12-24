#ifndef IPCMESSAGE_HPP
#define IPCMESSAGE_HPP

#include <cstdint>

namespace kiss
{

// This class is placed directly in shared memory.
class IpcMessage
{
  public:
    enum class MsgState
    {
        INIT,
        PRODUCED,
        CONSUMED,
        SKIP
    };

    uint8_t *getBuffer()
    {
        return (reinterpret_cast<uint8_t *>(this) + sizeof(IpcMessage));
    }

  private:
    IpcMessage(const uint32_t dataSize)
        : mMsgState(IpcMessage::MsgState::INIT), //
          mMessageSize(dataSize)
    {
    }

    MsgState mMsgState; // TODO: needs to be IPC-atomic
    uint32_t mMessageSize;

    // memory directly after the struct is used as data buffer
    // uint8_t mBuffer[mMessageSize];

    friend class SimpleIpc;
};

} // namespace kiss

#endif
