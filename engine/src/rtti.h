#pragma once
#include <typeindex>

namespace emerald {
	using RTTIType = std::type_index;

#define RTTI_BASE_CLASS_DECL(_thisclass)                                                            \
private:                                                                                            \
    static const RTTIType type_##_thisclass;                                                        \
public:                                                                                             \
	typedef _thisclass thisclass;                                                                   \
	virtual const RTTIType getClassType() const { return getStaticClassType(); }                    \
	virtual const RTTIType getParentClassType() const { return getStaticParentClassType(); }        \
    static const RTTIType getStaticClassType() { return type_##_thisclass; }                        \
	static const RTTIType getStaticParentClassType() { return RTTIType(typeid(void)); }             \
	static const std::string_view getStaticClassName() { return type_##_thisclass.name(); }         \
	virtual const std::string_view getClassName() const { return getStaticClassName(); }            \
    static bool isClassType(const RTTIType classType) {                                             \
        return classType == getStaticClassType();                                                   \
    }                                                                                               \
    template<typename T>                                                                            \
    static bool checkRTTI();

#define RTTI_DERIVED_CLASS_DECL(_thisclass, _parentclass)                                           \
private:                                                                                            \
    static const RTTIType type_##_thisclass;                                                        \
public:                                                                                             \
	typedef _thisclass thisclass;                                                                   \
	typedef _parentclass parentclass;                                                               \
	virtual const RTTIType getClassType() const { return getStaticClassType(); }                    \
	virtual const RTTIType getParentClassType() const { return _parentclass::getClassType(); }      \
    static const RTTIType getStaticClassType() { return type_##_thisclass; }                        \
	static const RTTIType getStaticParentClassType() { return _parentclass::getStaticClassType(); } \
	static const std::string_view getStaticClassName() { return type_##_thisclass.name(); }         \
	virtual const std::string_view getClassName() const { return getStaticClassName(); }            \
    static bool isClassType(const RTTIType classType) {                                             \
        if (classType == getStaticClassType()) return true;                                         \
        return _parentclass::isClassType(classType);                                                \
    }       																                        \
    template<typename T>                                                                            \
    static bool checkRTTI();

#define RTTI_CLASS_DEF(_thisclass)                                                                  \
const RTTIType _thisclass::type_##_thisclass  = RTTIType(typeid(_thisclass));                       \
template<>                                                                                          \
bool _thisclass::checkRTTI<_thisclass>() { return true; }
}