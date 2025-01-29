#pragma once
#include <typeindex>
#include <string_view>
#include "reflection.h"

using RTTIType = std::type_index;

////////////////////////////////////////
// Base Class RTTI Declaration Macro
////////////////////////////////////////
#define RTTI_BASE_CLASS_DECL(_thisclass)                                                            \
private:                                                                                            \
    static const RTTIType type_##_thisclass;                                                        \
    static ReflectionRegistrar s_Registrar_##_thisclass;                                            \
public:                                                                                             \
    using rtti_marker = int;               					                                        \
    typedef _thisclass thisclass;                                                                   \
    typedef void parentclass;                                                                       \
    virtual const RTTIType getClassType() const { return getStaticClassType(); }                    \
    virtual const RTTIType getParentClassType() const { return getStaticParentClassType(); }        \
    static const RTTIType getStaticClassType() { return type_##_thisclass; }                        \
    static const RTTIType getStaticParentClassType() { return RTTIType(typeid(void)); }             \
    static const std::string_view getStaticClassName() { return #_thisclass; }                      \
    virtual const std::string_view getClassName() const { return getStaticClassName(); }            \
    bool isClassType(const RTTIType classType) {                                                    \
        return classType == getClassType();                                                         \
    }                                                                                               \
    template<typename T>                                                                            \
    static bool checkRTTI();                                                                        \

////////////////////////////////////////
// Derived Class RTTI Declaration Macro
////////////////////////////////////////
#define RTTI_DERIVED_CLASS_DECL(_thisclass, _parentclass)                                           \
private:                                                                                            \
    static const RTTIType type_##_thisclass;                                                        \
    static ReflectionRegistrar s_Registrar_##_thisclass;                                            \
public:                                                                                             \
    using rtti_marker = int;               					                                        \
    typedef _thisclass thisclass;                                                                   \
    typedef _parentclass parentclass;                                                               \
    virtual const RTTIType getClassType() const { return getStaticClassType(); }                    \
    virtual const RTTIType getParentClassType() const { return _parentclass::getStaticClassType(); }\
    static const RTTIType getStaticClassType() { return type_##_thisclass; }                        \
    static const RTTIType getStaticParentClassType() { return _parentclass::getStaticClassType(); } \
    static const std::string_view getStaticClassName() { return #_thisclass; }                      \
    virtual const std::string_view getClassName() const { return getStaticClassName(); }            \
    bool isClassType(const RTTIType classType) {                                                    \
        if (classType == getClassType()) return true;                                               \
        return _parentclass::isClassType(classType);                                                \
    }                                                                                               \
    template<typename T>                                                                            \
    static bool checkRTTI();                                                                        \

////////////////////////////////////////
// Definition Macro
////////////////////////////////////////
#define RTTI_CLASS_DEF(_thisclass)                                                                  \
    const RTTIType _thisclass::type_##_thisclass = RTTIType(typeid(_thisclass));                    \
    template<>                                                                                      \
    bool _thisclass::checkRTTI<_thisclass>() { return true; }                                       \
    ReflectionRegistrar _thisclass::s_Registrar_##_thisclass(                                       \
        RTTIType(typeid(_thisclass)),                                                               \
        RTTIType(typeid(_thisclass::parentclass)),                                                  \
        #_thisclass                                                                                 \
    );