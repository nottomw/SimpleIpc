### Simple IPC
Simple IPC toy project for prototyping, lots of hacks.

```cpp
// Producer
kiss::SharedMem mem(128);
kiss::SimpleIpc ipcProducer(mem);

auto &ipcMsg = ipcProducer.messageBufferFetch(dataBufferSize);
auto *ipcBuffer = ipcMsg.getBuffer();

(void)memcpy(ipcBuffer, dataBuffer, dataBufferSize);

ipcProducer.sendMessage(ipcMsg);
```

```cpp
// Consumer
kiss::SharedMem mem(128);
kiss::SimpleIpc ipcConsumer(mem);

kiss::IpcMessage *msg = ipcConsumer.receiveStart();
if (msg != nullptr)
{
    // copy msg data somewhere...
    ipcConsumer.receiveEnd(msg);
}
```

### Build
```
cmake ../SimpleIpc/ -G Ninja
ninja
```
