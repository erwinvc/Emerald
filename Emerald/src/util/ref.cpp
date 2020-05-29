#include "stdafx.h"
#include "graphics/buffers/indexBuffer.h"

template class ManagedRef<IndexBuffer>;
template class ManagedRef<VertexBuffer>;
template class ManagedRef<VertexArray>;

template<typename T>
ManagedRef<T>::ManagedRef(ManagedRef const& managedRef) : m_pointer(managedRef.m_pointer) { ++m_pointer->m_refCount; }

template<typename T>
ManagedRef<T>::ManagedRef(T* pointer) : m_pointer(pointer) { if (m_pointer) ++m_pointer->m_refCount; }

template<typename T>
ManagedRef<T>::~ManagedRef() { if (--m_pointer->m_refCount == 0) delete m_pointer; }