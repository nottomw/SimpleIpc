#ifndef ALLOCATOR_HPP
#define ALLOCATOR_HPP

#include <cassert>
#include <cstdint>

namespace kiss
{

class Allocator
{
  public:
    Allocator(uint8_t *const memoryRegionStart, const uint32_t memoryRegionSize)
        : mMemoryRegionStart(memoryRegionStart), //
          mMemoryRegionSize(memoryRegionSize)
    {
    }

    void *alloc(const uint32_t sizeBytes)
    {
        // TODO: implement
        static uint32_t curIdx = 0U;

        assert(curIdx < mMemoryRegionSize);

        void *ret = mMemoryRegionStart + curIdx;
        curIdx = sizeBytes;

        return ret;
    }

    void dealloc(void *const ptr)
    {
        // TODO: implement
        (void)ptr;
    }

  private:
    uint8_t *mMemoryRegionStart;
    uint32_t mMemoryRegionSize;
};

} // namespace kiss

#endif
