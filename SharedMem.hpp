#ifndef SHAREDMEM_HPP
#define SHAREDMEM_HPP

#include <cstdint>

namespace kiss
{

class SharedMem
{
  public:
    SharedMem(const char *const name, const uint32_t sharedMemSize);
    SharedMem(void *const sharedMemAddr, const uint32_t sharedMemSize);
    ~SharedMem();

    uint32_t getSize() const;
    uint8_t *getAddress() const;

  private:
    uint32_t mSharedMemSize;
    uint8_t *mSharedMemoryAddr;

    int shmFd;
    const char *const mSharedMemName;

    friend class SimpleIpc;
};

} // namespace kiss

#endif // SHAREDMEM_HPP
