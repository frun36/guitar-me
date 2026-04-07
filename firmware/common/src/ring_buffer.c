#include "ring_buffer.h"
#include <string.h>

static inline void* GetDataPtr(RingBuffer_t* rb, size_t idx) {
    return (uint8_t*)(rb->data) + (idx * rb->element_size);
}

void RingBuffer_Init(
    RingBuffer_t* rb,
    void* buff,
    size_t capacity,
    size_t element_size
) {
    rb->data = buff;
    rb->capacity = capacity;
    rb->element_size = element_size;
    rb->read_idx = 0;
    rb->write_idx = 0;
}

bool RingBuffer_Push(RingBuffer_t* rb, const void* data) {
    if (RingBuffer_IsFull(rb))
        return false;

    void* insert_ptr = GetDataPtr(rb, rb->write_idx);
    memcpy(insert_ptr, data, rb->element_size);

    rb->write_idx = (rb->write_idx + 1) % rb->capacity;

    return true;
}

bool RingBuffer_Pop(RingBuffer_t* rb, void* out) { 
    if (RingBuffer_IsEmpty(rb))
        return false;

    const void* read_ptr = GetDataPtr(rb, rb->read_idx);
    memcpy(out, read_ptr, rb->element_size);

    rb->read_idx = (rb->read_idx + 1) % rb->capacity;

    return true;
}

bool RingBuffer_IsFull(const RingBuffer_t* rb) {
    return (rb->write_idx + 1) % rb->capacity == rb->read_idx;
}

bool RingBuffer_IsEmpty(const RingBuffer_t* rb) {
    return rb->write_idx == rb->read_idx;
}
