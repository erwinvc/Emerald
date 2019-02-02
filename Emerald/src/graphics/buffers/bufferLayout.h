#pragma once

class BufferLayout {
private:
    struct BufferElement {
        String m_name;
        uint m_type;
        Byte m_amount;
        Byte m_size;
        uint m_offset;
    };

    vector<BufferElement> m_elements;
    uint m_stride;
    void PushInternal(const String& name, uint type, Byte count, Byte size);

public:
    BufferLayout();

    inline vector<BufferElement>& GetElements() { return m_elements; }
    inline uint GetStride() { return m_stride; }

    template<typename T>
    void Push(const String& name) {
        LOG_ERROR("[Buffers] Unknown element type");
    }

    template<> void Push<float>(const String& name);
    template<> void Push<int>(const String& name);
    template<> void Push<uint>(const String& name);
    template<> void Push<Byte>(const String& name);

    template<> void Push<Vector2>(const String& name);
    template<> void Push<Vector3>(const String& name);
    template<> void Push<Vector4>(const String& name);
};

template<>
void BufferLayout::Push<float>(const String& name) {
    PushInternal(name, GL_FLOAT, 1, sizeof(float));
}

template<>
void BufferLayout::Push<int>(const String& name) {
    PushInternal(name, GL_INT, 1, sizeof(int));
}

template<>
void BufferLayout::Push<uint>(const String& name) {
    PushInternal(name, GL_UNSIGNED_INT, 1, sizeof(uint));
}

template<>
void BufferLayout::Push<Byte>(const String& name) {
    PushInternal(name, GL_BYTE, 1, sizeof(Byte));
}

template<>
void BufferLayout::Push<Vector2>(const String& name) {
    PushInternal(name, GL_FLOAT, 2, sizeof(float));
}

template<>
void BufferLayout::Push<Vector3>(const String& name) {
    PushInternal(name, GL_FLOAT, 3, sizeof(float));
}

template<>
void BufferLayout::Push<Vector4>(const String& name) {
    PushInternal(name, GL_FLOAT, 4, sizeof(float));
}