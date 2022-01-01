### Simple IPC
Simple IPC toy project for prototyping, lots of hacks.

```cpp
// Producer
kiss::SharedMem mem(128);
kiss::SimpleIpc ipcProducer(mem);

auto &ipcMsg = ipcProducer.messageBufferFetch(dataBufferSize);

// ...
```

```cpp
// Consumer
kiss::SharedMem mem(128);
kiss::SimpleIpc ipcConsumer(mem);

// ...
```

### Build
```
cmake ../SimpleIpc/ -G Ninja
ninja
```
