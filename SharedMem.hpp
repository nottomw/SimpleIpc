#ifndef SHAREDMEM_HPP
#define SHAREDMEM_HPP

#include <cstdint>

namespace kiss
{

class SharedMem
{
  public:
    SharedMem(const uint32_t sharedMemSize);
    SharedMem(void *const sharedMemAddr, const uint32_t sharedMemSize);
    ~SharedMem();

    uint32_t getSize() const;

  private:
    uint32_t mSharedMemSize;
    uint8_t *mSharedMemoryAddr;

    int shmFd;

    friend class SimpleIpc;
};

} // namespace kiss

#endif // SHAREDMEM_HPP
