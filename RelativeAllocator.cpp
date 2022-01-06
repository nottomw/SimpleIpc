#include "RelativeAllocator.hpp"

namespace kiss
{

RelativeAllocator::RelativeAllocator(const uint32_t memoryRegionSize) : mMemoryRegionSize(memoryRegionSize)
{
    mRegionsFree.emplace(0, memoryRegionSize);
}

RelativeAllocator::RelativePtr RelativeAllocator::alloc(const uint32_t sizeBytes)
{
    bool allocSuccess = false;
    RelativePtr allocAddr = 0;

    for (auto &it : mRegionsFree)
    {
        if (it.size > sizeBytes)
        {
            mRegionsUsed.emplace(it.ptr, sizeBytes);
            allocAddr = it.ptr;

            const uint32_t newFreeRegionSize = it.size - sizeBytes;
            if (newFreeRegionSize != 0)
            {
                mRegionsFree.emplace((it.ptr + sizeBytes), newFreeRegionSize);
            }

            mRegionsFree.erase(it);

            allocSuccess = true;

            break;
        }
    }

    // TODO: return error, assert for now...
    assert(allocSuccess);

    return allocAddr;
}

void RelativeAllocator::dealloc(const RelativeAllocator::RelativePtr ptr)
{
    auto it = mRegionsUsed.find(ptr);
    if (it != mRegionsUsed.end())
    {
        mRegionsUsed.erase(it);
        auto newMemRegion = mRegionsFree.emplace(it->ptr, it->size).first;

        coalesce(*newMemRegion);
    }
    else
    {
        assert("tried to free invalid ptr" == nullptr);
    }
}

void RelativeAllocator::coalesce(const MemoryRegion &freeMemRegion)
{
    // if possible coalesce the freedPtr with adjacent free memory

    MemoryRegion memRegionToCoalesce{freeMemRegion};

    // find free regions before
    if (memRegionToCoalesce.ptr != 0)
    {
        auto freeRegionBefore = mRegionsFree.find(memRegionToCoalesce.ptr - 1U);
        if (freeRegionBefore != mRegionsFree.end())
        {
            // coalesce
        }
    }
}

} // namespace kiss