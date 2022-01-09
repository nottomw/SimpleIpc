### Simple IPC
Simple IPC toy project for prototyping, lots of hacks.

```cpp
// Producer
kiss::SharedMem mem(2048);
kiss::SimpleIpc ipcProducer(mem);

auto sendRes = ipcProducer.sendMessage(data, data.size());
// ...
```

```cpp
// Consumer
kiss::SharedMem mem(2048);
kiss::SimpleIpc ipcConsumer(mem);

auto res = ipcConsumer.receiveMessage(&buffer[0], 128);
// ...
```

### Build
```
cmake ../SimpleIpc/ -G Ninja
ninja
```
