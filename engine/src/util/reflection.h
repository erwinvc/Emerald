#pragma once
#include <type_traits>
#include "json.h"

namespace emerald {

	template <typename Class, typename T>
	using member_ptr_t = T Class::*;

	template <typename Class, typename T>
	class Field {
	public:
		using class_type = Class;
		using member_type = T;

		Field(const char* name, member_ptr_t<Class, T> ptr) : name(name), ptr(ptr) {}

		const T& get(const Class& obj) const { return obj.*ptr; }

		template <typename V, typename = std::enable_if_t<std::is_constructible<T, V>::value>>
		void set(Class& obj, V&& value) const { obj.*ptr = value; }

		const char* getName() const { return name; }
		const char* name;
		member_ptr_t<Class, T> ptr;
	};

	template <typename Class, typename T>
	Field<Class, T> member(const char* name, T Class::* ptr) {
		return Field<Class, T>(name, ptr);
	}

	template <typename Class>
	inline auto getFields();

	// Helper trait to check if a type is reflectable
	template <typename, typename = std::void_t<>>
	struct is_reflectable : std::false_type {};

	template <typename T>
	struct is_reflectable<T, std::void_t<decltype(getFields<T>())>> : std::true_type {};

	template <typename T>
	inline constexpr bool is_reflectable_v = is_reflectable<T>::value;

	template <typename T, typename TupleType>
	struct MetaHolder {
		static TupleType members;
	};

	template <typename T, typename TupleType>
	TupleType MetaHolder<T, TupleType>::members = getFields<T>();

	template <typename T>
	nlohmann::json serialize(const T& obj);

	template <typename T>
	struct is_vector : std::false_type {};

	template <typename T, typename Alloc>
	struct is_vector<std::vector<T, Alloc>> : std::true_type {};

	template <typename T>
	void serialize_field(const char* name, const T& value, nlohmann::json& j) {
		if constexpr (is_vector<T>::value) {
			nlohmann::json jsonArray = nlohmann::json::array();
			for (const auto& elem : value) {
				jsonArray.push_back(serialize(elem));
			}
			j[name] = jsonArray;
		} else if constexpr (is_reflectable_v<T>) {
			j[name] = serialize(value);
		} else {
			j[name] = value;
		}
	}

	template <typename T>
	void deserialize_field(const char* name, T& value, const nlohmann::json& j) {
		if (j.contains(name)) {
			if constexpr (is_vector<T>::value) {
				value.clear();
				for (const auto& elem : j.at(name)) {
					typename T::value_type temp;
					deserialize(temp, elem);
					value.push_back(std::move(temp));
				}
			} else if constexpr (is_reflectable_v<T>) {
				// If the field is reflectable, recursively deserialize it
				deserialize(value, j.at(name));
			} else {
				// Otherwise, deserialize it directly
				value = j.at(name).get<T>();
			}
		}
	}


	// Function to iterate over a tuple of fields and apply a function
	template <typename T, typename Tuple, std::size_t... Is>
	void for_each_field_impl(const T& obj, const Tuple& fields, nlohmann::json& j, std::index_sequence<Is...>) {
		(serialize_field(std::get<Is>(fields).name, obj.*(std::get<Is>(fields).ptr), j), ...);
	}

	template <typename T, typename Tuple>
	void for_each_field(const T& obj, const Tuple& fields, nlohmann::json& j) {
		constexpr std::size_t numFields = std::tuple_size_v<Tuple>;
		for_each_field_impl(obj, fields, j, std::make_index_sequence<numFields>{});
	}

	// General serialize function that calls for_each_field on a reflectable type
	template <typename T>
	nlohmann::json serialize(const T& obj) {
		nlohmann::json j;

		const auto& fields = MetaHolder<T, decltype(getFields<T>())>::members;
		for_each_field(obj, fields, j);
		return j;
	}

	// Function to iterate over a tuple of fields and apply a function for deserialization
	template <typename T, typename Tuple, std::size_t... Is>
	void for_each_field_deserialize_impl(T& obj, const Tuple& fields, const nlohmann::json& j, std::index_sequence<Is...>) {
		(deserialize_field(std::get<Is>(fields).name, obj.*(std::get<Is>(fields).ptr), j), ...);
	}

	template <typename T, typename Tuple>
	void for_each_field_deserialize(T& obj, const Tuple& fields, const nlohmann::json& j) {
		constexpr std::size_t numFields = std::tuple_size_v<Tuple>;
		for_each_field_deserialize_impl(obj, fields, j, std::make_index_sequence<numFields>{});
	}

	// General deserialize function that calls for_each_field_deserialize on a reflectable type
	template <typename T>
	void deserialize(T& obj, const nlohmann::json& j) {
		const auto& fields = MetaHolder<T, decltype(getFields<T>())>::members;
		for_each_field_deserialize(obj, fields, j);
	}
}