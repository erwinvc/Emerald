#include "stdafx.h"
//
//LayoutVertexArray::LayoutVertexArray(BufferLayout& layout, uint count) :m_layout(layout) {
//    m_buffer = NEW(Buffer(nullptr, count, 0, GL_DYNAMIC_DRAW));
//}
//
//LayoutVertexArray::LayoutVertexArray(BufferLayout& layout, float* data, uint count) : m_layout(layout) {
//    m_buffer = NEW(Buffer(data, count, 0, GL_STATIC_DRAW));
//}
//
//LayoutVertexArray::~LayoutVertexArray() {
//    DELETE(m_buffer);
//}
//
//void LayoutVertexArray::Bind() {
//    m_buffer->Bind();
//
//    const auto& elements = m_layout.GetElements();
//    uint stride = m_layout.GetStride();
//    for (uint i = 0; i < elements.size(); i++) {
//        GL(glEnableVertexAttribArray(i));
//        glVertexAttribPointer(i, elements[i].m_amount, elements[i].m_type, GL_FALSE, stride, (const void*)elements[i].m_offset);
//    }
//}
//
//void LayoutVertexArray::Unbind() {
//    GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
//}
//
////void LayoutVertexArray::Unmap() {
////    GL(glUnmapBuffer(GL_ARRAY_BUFFER));
////}
////
////void * LayoutVertexArray::MapInternal() {
////    GL(void* result = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
////    return result;
////}