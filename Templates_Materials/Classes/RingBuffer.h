/*
  ==============================================================================

    RingBuffer.h
    Created: 22 Feb 2026 9:54:56pm
    Author:  Nico Russo

  ==============================================================================
*/

#pragma once
#include "Basics.h"

template <typename T, size_t maxBufferSize>
class RingBuffer {
public:
    
    /* Push a single sample. Overwrites oldest data when full */
    void push(T xn);
    
    /* Push a block of samples */
    void push(const T* data, int numSamples);
    
    /* Read the sample that is 'delaySamples' in the past */
    T read(int delaySamples = 0) const;
    
    /*
     * Copy 'numSamples' consecutive samples into 'dest', starting from 'startDelay'
     * samples ago up to 'startDelay + numSamples - 1' samples ago
     * i.e. dest[0] is the most recent if startDelay = 0
     */
    void readBlock(T* dest, int numSamples, int startDelay = 0) const;
    
    /* Accessors */
    int      getCapacity()    const noexcept { return capacity; }
    int      getSize()        const noexcept { return size; }
    bool     isEmpty()        const noexcept { return size == 0; }
    bool     isFull()         const noexcept { return size == capacity; }
    const T* getData()        const noexcept { return buffer.data(); }
    int      getWriteHead()   const noexcept { return writeHead; }
    
    /* Clear */
    void clear();
    
    /* Resize */
    void resize(int newCapacity);
    
protected:
    int wrap (int index) const noexcept {
        return ((index % capacity) + capacity) % capacity;
    }
    
    std::array<T, maxBufferSize> buffer;
    int capacity  { maxBufferSize };
    int writeHead { 0 };
    int size      { 0 };
};




template <typename T, size_t maxBufferSize>
void RingBuffer<T, maxBufferSize>::push(T xn) {
    buffer[writeHead] = xn;
    writeHead = wrap(writeHead + 1);
    if (size < capacity)
        ++size;
}

template <typename T, size_t maxBufferSize>
void RingBuffer<T, maxBufferSize>::push(const T *data, int numSamples) {
    for (int i = 0; i < numSamples; ++i)
        push(data[i]);
}

template <typename T, size_t maxBufferSize>
T RingBuffer<T, maxBufferSize>::read(int delaySamples) const {
    // writeHead points to the NEXT write slow, so most recent sample is at writeHead - 1
    int index = wrap(writeHead - 1 - delaySamples);
    return buffer[index];
}

template <typename T, size_t maxBufferSize>
void RingBuffer<T, maxBufferSize>::readBlock(T *dest, int numSamples, int startDelay) const {
    for (int i = 0; i < numSamples; ++i)
        dest[i] = read(startDelay + i);
}

template <typename T, size_t maxBufferSize>
void RingBuffer<T, maxBufferSize>::clear() {
    writeHead = 0;
    size = 0;
    buffer.fill(T(0));
}

template <typename T, size_t maxBufferSize>
void RingBuffer<T, maxBufferSize>::resize(int newCapacity) {
    capacity = std::min(newCapacity, (int)maxBufferSize);
    clear();
}
