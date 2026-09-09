#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

template <typename T, size_t max_buffer_size> class RingBuffer {
  public:
    // Push a single sample into the buffer.
    // Overwrites oldest data when full.
    void push(T xn);

    // Pushes a block of samples
    void push(const T *data, size_t num_samples);

    // Read the sample 'delay_samples' in the past
    T read(size_t delay_samples = 0) const;

    /*
     * Copy 'num_samples' consecutive samples into 'dest',
     * starting from 'start_delay' samples ago, up to
     * 'start_delay + num_samples - 1' samples ago.
     * i.e. dest[0] is the most recent if start_delay = 0
     */
    void readBlock(T *dest, size_t num_samples, size_t start_delay = 0) const;

    // Clears the buffer
    void clear();

    // Resize the buffer. Should only do on sample rate changes!!
    void resize(size_t new_capacity);

    size_t   getCapacity() const { return capacity; }
    size_t   getSize() const { return size; }
    bool     isEmpty() const { return size == 0; }
    bool     isFull() const { return size == capacity; }
    const T *getData() const { return buffer.data(); }
    size_t   getWriteHead() const { return write_head; }

  protected:
    size_t wrap(size_t index) const {
        return ((index % capacity) + capacity) % capacity;
    }

    std::array<T, max_buffer_size> buffer;
    size_t                         capacity{max_buffer_size};
    size_t                         write_head{0};
    size_t                         size{0};
};

/* ======================================================== */
/* IMPLEMENTATIONS ======================================== */
/* ======================================================== */

template <typename T, size_t maxBufferSize>
void RingBuffer<T, maxBufferSize>::push(T xn) {
    buffer[write_head] = xn;

    write_head = wrap(write_head + 1);

    if (size < capacity)
        ++size;
}

template <typename T, size_t maxBufferSize>
void RingBuffer<T, maxBufferSize>::push(const T *data, size_t num_samples) {
    for (int i = 0; i < num_samples; ++i)
        push(data[i]);
}

template <typename T, size_t maxBufferSize>
T RingBuffer<T, maxBufferSize>::read(size_t delay_samples) const {
    // writeHead points to the NEXT write slow, so most recent sample is at
    // writeHead - 1
    int index = wrap(write_head - 1 - delay_samples);
    return buffer[index];
}

template <typename T, size_t maxBufferSize>
void RingBuffer<T, maxBufferSize>::readBlock(T *dest, size_t num_samples,
                                             size_t start_delay) const {
    for (int i = 0; i < num_samples; ++i)
        dest[i] = read(start_delay + i);
}

template <typename T, size_t maxBufferSize>
void RingBuffer<T, maxBufferSize>::clear() {
    write_head = 0;
    size       = 0;
    buffer.fill(T(0));
}

template <typename T, size_t maxBufferSize>
void RingBuffer<T, maxBufferSize>::resize(size_t newCapacity) {
    capacity = std::min(newCapacity, maxBufferSize);
    clear();
}
