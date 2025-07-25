#pragma once
#include <iostream>
class ArenaAllocator {
public:
    inline ArenaAllocator(size_t size)
        : m_size(size)
    {
        m_buffer = new(std::byte[size]);
        m_offset = m_buffer;
    }
    template<typename T>
    inline T* alloc()
    {
        std::byte* offset = m_offset;
        m_offset += sizeof(T);
        return static_cast<T*>(offset);
    }
    inline ArenaAllocator(const ArenaAllocator& other) = delete;
    inline ArenaAllocator& operator=(const ArenaAllocator& other) = delete;
    inline ~ArenaAllocator()
    {
        ::operator delete(m_buffer);
    }

private:
    size_t m_size;
    std::byte* m_buffer;
    std::byte* m_offset;
};