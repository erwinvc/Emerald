#pragma once

#define STATIC_CLASS(ClassName)                     \
private:                                            \
    ClassName() = delete;                           \
    ~ClassName() = delete;                          \
    ClassName(const ClassName&) = delete;           \
    ClassName& operator=(const ClassName&) = delete;\
    ClassName(ClassName&&) = delete;                \
    ClassName& operator=(ClassName&&) = delete;