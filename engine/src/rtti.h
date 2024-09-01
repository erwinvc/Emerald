#pragma once
#include <typeindex>


using RTTIType = std::type_index;

#define RTTI_BASE_CLASS_DECL(thisclass)                                             \
private:                                                                            \
    static const RTTIType type;                                                     \
public:                                                                             \
    static RTTIType getClassType() { return type; }                                 \
    static RTTIType getParentClassType() { return RTTIType(typeid(void)); }         \
    static bool isClassType(const RTTIType classType) {                       \
        return classType == getClassType();                                   \
    }                                                                               \
    template<typename T>                                                            \
    static bool checkRTTI();

#define RTTI_DERIVED_CLASS_DECL(thisclass, parentclass)                             \
private:                                                                            \
    static const RTTIType type;                                                     \
public:                                                                             \
    static RTTIType getClassType() { return type; }                           \
    static RTTIType getParentClassType() {                                    \
        return parentclass::getClassType();                                   \
    }                                                                               \
    static bool isClassType(const RTTIType classType) {                       \
        if (classType == getClassType()) return true;                         \
        return parentclass::isClassType(classType);                           \
    }       																        \
    template<typename T>                                                            \
    static bool checkRTTI();

#define RTTI_CLASS_DEF(thisclass)                                                   \
const RTTIType thisclass::type = RTTIType(typeid(thisclass));                       \
template<>                                                                          \
bool thisclass::checkRTTI<thisclass>() { return true; }