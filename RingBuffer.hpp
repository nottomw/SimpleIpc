#ifndef RINGBUFFER_HPP
#define RINGBUFFER_HPP

#include <array>
#include <cstring>

template <typename T, size_t Size> class RingBuffer
{
    static constexpr size_t SizeWithPadding = Size + 1;

  public:
    enum class Result
    {
        OK,
        ERR_FULL,
        ERR_EMPTY
    };

    RingBuffer()
    {
        mHead = 0U;
        mTail = 0U;
    }

    Result push(const T *data, const uint32_t count)
    {
        // modify only head
        const uint32_t sizeLeftCur = sizeLeft();
        if (sizeLeftCur < count)
        {
            return Result::ERR_FULL;
        }

        if ((mHead + count) < SizeWithPadding)
        {
            (void)memcpy(&ringBuffer[mHead], data, count);
        }
        else
        {
            const uint32_t firstCopySize = SizeWithPadding - mHead;
            const uint32_t secondCopySize = count - firstCopySize;

            (void)memcpy(&ringBuffer[mHead], data, firstCopySize);
            (void)memcpy(&ringBuffer[0], data + firstCopySize, secondCopySize);
        }

        mHead = (mHead + count) % SizeWithPadding;

        return Result::OK;
    }

    Result pop(T *data, const uint32_t count)
    {
        // modify only tail
        if (isEmpty())
        {
            return Result::ERR_EMPTY;
        }

        if ((mTail + count) < SizeWithPadding)
        {
            (void)memcpy(data, &ringBuffer[mTail], count);
        }
        else
        {
            const uint32_t firstCopySize = SizeWithPadding - mTail;
            const uint32_t secondCopySize = count - firstCopySize;

            (void)memcpy(data, &ringBuffer[mTail], firstCopySize);
            (void)memcpy(data + firstCopySize, &ringBuffer[0], secondCopySize);
        }

        mTail = (mTail + count) % SizeWithPadding;

        return Result::OK;
    }

  private:
    bool isFull(void)
    {
        return ((mHead + 1U) == mTail);
    }
    bool isEmpty(void)
    {
        return (mHead == mTail);
    }

    uint32_t sizeLeft(void)
    {
        if (isFull())
        {
            return 0U;
        }

        if (mHead > mTail)
        {
            return (mHead - mTail - 1);
        }
        else
        {
            return (SizeWithPadding + mHead - mTail - 1);
        }
    }

    uint32_t mHead;
    uint32_t mTail;

    std::array<T, SizeWithPadding> ringBuffer;
};

#endif // RINGBUFFER_HPP
