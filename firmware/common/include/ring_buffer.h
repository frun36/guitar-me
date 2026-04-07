#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    void* data;
    size_t capacity;
    size_t element_size;
    size_t fill;
    size_t write_idx;
    size_t read_idx;
} RingBuffer_t;

/// Keeps one slot open for empty/full differentiation
void RingBuffer_Init(RingBuffer_t* rb, void* buff, size_t capacity, size_t element_size);

bool RingBuffer_Push(RingBuffer_t* rb, const void* data);
bool RingBuffer_Pop(RingBuffer_t* rb, void* out);

bool RingBuffer_IsFull(const RingBuffer_t* rb);
bool RingBuffer_IsEmpty(const RingBuffer_t* rb);
