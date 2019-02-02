#include "stdafx.h"

BufferLayout::BufferLayout()
    : m_stride(0) {
}

void BufferLayout::PushInternal(const String& name, uint type, Byte count, Byte size) {
    Byte index = 0;
    Byte offset = 0;
    if (m_elements.size() > 0) {
        const BufferElement& prev = m_elements.back();
        offset = prev.m_offset + prev.m_size * prev.m_amount;
    }
    m_elements.push_back({ name, type, count, size, offset });
    m_stride += count * size;
}