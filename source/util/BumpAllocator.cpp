//------------------------------------------------------------------------------
// BumpAllocator.cpp
// Fast allocator based on pointer bumping.
//
// File is under the MIT license; see LICENSE for details.
//------------------------------------------------------------------------------
#include "BumpAllocator.h"

#include <cstdlib>

namespace slang {

BumpAllocator::BumpAllocator() {
    head = allocSegment(nullptr, INITIAL_SIZE);
    endPtr = (std::byte*)head + INITIAL_SIZE;
}

BumpAllocator::~BumpAllocator() {
    Segment* seg = head;
    while (seg) {
        Segment* prev = seg->prev;
        free(seg);
        seg = prev;
    }
}

BumpAllocator::BumpAllocator(BumpAllocator&& other) noexcept :
    head(std::exchange(other.head, nullptr)), endPtr(other.endPtr)
{
}

BumpAllocator& BumpAllocator::operator=(BumpAllocator&& other) noexcept {
    if (this != &other) {
        this->~BumpAllocator();
        new (this) BumpAllocator(std::move(other));
    }
    return *this;
}

std::byte* BumpAllocator::allocateSlow(size_t size, size_t alignment) {
    // for really large allocations, give them their own segment
    if (size > (SEGMENT_SIZE >> 1)) {
        size = (size + alignment - 1) & ~(alignment - 1);
        head->prev = allocSegment(head->prev, size + sizeof(Segment));
        return alignPtr(head->prev->current, alignment);
    }

    // otherwise, start a new block
    head = allocSegment(head, SEGMENT_SIZE);
    endPtr = (std::byte*)head + SEGMENT_SIZE;
    return allocate(size, alignment);
}

BumpAllocator::Segment* BumpAllocator::allocSegment(Segment* prev, size_t size) {
    auto seg = (Segment*)malloc(size);
    seg->prev = prev;
    seg->current = (std::byte*)seg + sizeof(Segment);
    return seg;
}

}
