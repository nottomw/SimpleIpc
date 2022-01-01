#ifndef ALLOCATOR_HPP
#define ALLOCATOR_HPP

#include <cassert>
#include <cstdint>

namespace kiss
{

// TODO: rework for relative addresses
// TOOD: rename RelativeAllocator
class Allocator
{
  public:
    using RelativePtr = uint32_t;

    Allocator(uint8_t *const memoryRegionStart, const uint32_t memoryRegionSize)
        : mMemoryRegionStart(memoryRegionStart), //
          mMemoryRegionSize(memoryRegionSize)
    {
    }

    RelativePtr alloc(const uint32_t sizeBytes)
    {
        // TODO: implement
        static uint32_t curIdx = 0U;

        assert(curIdx < mMemoryRegionSize);

        const RelativePtr allocAddr = curIdx;
        curIdx += sizeBytes;

        return allocAddr;
    }

    void dealloc(const RelativePtr ptr)
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
