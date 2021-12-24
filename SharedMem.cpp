#include "SharedMem.hpp"

#include <cassert>
#include <cstdio>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

namespace kiss
{

// linux-only for now
#define SHM_OBJ_NAME "/shmem_test"

SharedMem::SharedMem(const uint32_t sharedMemSize)
{

    const uint32_t pageSize = sysconf(_SC_PAGE_SIZE);
    const uint32_t alignedSize = sharedMemSize + (pageSize - (sharedMemSize % pageSize));

    printf("algn size = %d, user size = %d\n", alignedSize, sharedMemSize);

    shmFd = shm_open(SHM_OBJ_NAME, O_RDWR | O_CREAT, 0777);
    assert(shmFd != -1);

    const int ftruncRes = ftruncate(shmFd, alignedSize);
    assert(ftruncRes != -1);

    mSharedMemoryAddr = (uint8_t *)mmap(0, alignedSize, PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
    assert(mSharedMemoryAddr != MAP_FAILED);

    mSharedMemSize = sharedMemSize;
}

SharedMem::SharedMem(void *const sharedMemAddr, const uint32_t sharedMemSize)
{
    mSharedMemSize = sharedMemSize;
    mSharedMemoryAddr = static_cast<uint8_t *>(sharedMemAddr);
}

SharedMem::~SharedMem()
{
    (void)munmap(mSharedMemoryAddr, mSharedMemSize);
    (void)shm_unlink(SHM_OBJ_NAME);
}

uint32_t SharedMem::getSize() const
{
    return mSharedMemSize;
}

} // namespace kiss
