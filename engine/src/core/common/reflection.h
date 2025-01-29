#pragma once
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <functional>
#include <string_view>
#include "rtti.h"

namespace emerald {
	using RTTIType = std::type_index;

	template<typename, typename = std::void_t<>>
	struct has_rtti_marker : std::false_type {};

	template<typename T>
	struct has_rtti_marker<T, std::void_t<typename T::rtti_marker>> : std::true_type {};

	template<typename T>
	struct TypeTraits {
		static constexpr bool isReflectable = has_rtti_marker<T>::value;
		static constexpr bool isAbstract = std::is_abstract_v<T>;
		static constexpr bool isPolymorphic = std::is_polymorphic_v<T>;
	};

	struct ReflectionData {
		std::string_view m_className;
		RTTIType m_type;
		RTTIType m_parentType;
		std::vector<std::type_index> m_children;

		ReflectionData() : m_className(""), m_type(typeid(void)), m_parentType(typeid(void)) {}
	};

	class Reflection {
		STATIC_CLASS(Reflection)
	public:

		static const ReflectionData* findByType(RTTIType t) {
			auto it = s_registryByType.find(t);
			return (it != s_registryByType.end()) ? &it->second : nullptr;
		}

		static const ReflectionData* findByName(std::string_view name) {
			auto it = s_registryByName.find(name);
			if (it != s_registryByName.end()) {
				auto it2 = s_registryByType.find(it->second);
				if (it2 != s_registryByType.end())
					return &it2->second;
			}
			return nullptr;
		}

		template<typename T>
		static const ReflectionData* findByType() {
			static_assert(TypeTraits<T>::isReflectable, "T is not an RTTI type.");
			return findByType(T::getStaticClassType());
		}

		static std::vector<RTTIType> getAllRegisteredTypes() {
			std::vector<RTTIType> out;
			out.reserve(s_registryByType.size());
			for (auto& kv : s_registryByType)
				out.push_back(kv.first);
			return out;
		}

	private:
		static inline std::unordered_map<RTTIType, ReflectionData> s_registryByType;
		static inline std::unordered_map<std::string_view, RTTIType> s_registryByName;
		friend class ReflectionRegistrar;
	};

	class ReflectionRegistrar
	{
	public:
		ReflectionRegistrar(RTTIType thisIdx, RTTIType parentIdx, std::string_view className) {
			ReflectionData& data = Reflection::s_registryByType[thisIdx];
			data.m_className = className;
			data.m_type = thisIdx;
			data.m_parentType = parentIdx;

			Reflection::s_registryByName.insert({ className, thisIdx });

			if (parentIdx != typeid(void)) {
				ReflectionData& parentData = Reflection::s_registryByType[parentIdx];
				parentData.m_children.push_back(thisIdx);
			}
		}
	};
}