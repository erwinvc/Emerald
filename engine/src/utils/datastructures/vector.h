#pragma once
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <numeric>
#include <functional>

namespace emerald {
	// A simple vector warpper class that provides additional functionality

	template<typename T>
	class Vector {
	public:
		// Constructors
		Vector() = default;
		Vector(const std::initializer_list<T>& list) : vec(list) {}
		explicit Vector(const std::vector<T>& _vec) : vec(_vec) {}

		// Iterator types
		using iterator = typename std::vector<T>::iterator;
		using const_iterator = typename std::vector<T>::const_iterator;
		using reverse_iterator = typename std::vector<T>::reverse_iterator;
		using const_reverse_iterator = typename std::vector<T>::const_reverse_iterator;

		// Emplace operations
		template<typename... Args>
		T& emplace_back(Args&&... args) {
			return vec.emplace_back(std::forward<Args>(args)...);
		}

		template<typename... Args>
		auto emplace(const_iterator position, Args&&... args) {
			return vec.emplace(position, std::forward<Args>(args)...);
		}

		template<typename... Args>
		void emplace_at(size_t index, Args&&... args) {
			if (index > vec.size()) {
				throw std::out_of_range("Index out of range");
			}
			vec.emplace(vec.begin() + index, std::forward<Args>(args)...);
		}

		// Standard vector operations
		void pushBack(const T& value) { vec.push_back(value); }
		void popBack() { vec.pop_back(); }

		// Capacity Management
		void reserve(std::size_t newCapacity) { vec.reserve(newCapacity); }
		void shrinkToFit() { vec.shrink_to_fit(); }
		size_t size() const { return vec.size(); }
		bool empty() const { return vec.empty(); }
		void clear() { vec.clear(); }

		// Access operations
		T& at(size_t index) { return vec.at(index); }
		const T& at(size_t index) const { return vec.at(index); }
		T& operator[](size_t index) { return vec[index]; }
		const T& operator[](size_t index) const { return vec[index]; }

		// front() / back()
		T& front() { return vec.front(); }
		const T& front() const { return vec.front(); }
		T& back() {	return vec.back(); }
		const T& back() const {	return vec.back(); }

		// data()
		T* data() {	return vec.data(); }
		const T* data() const {	return vec.data(); }

		// Iterator support
		auto begin() { return vec.begin(); }
		auto end() { return vec.end(); }
		auto begin() const { return vec.begin(); }
		auto end() const { return vec.end(); }
		auto rbegin() { return vec.rbegin(); }
		auto rend() { return vec.rend(); }

		// Enhanced functionality
		bool contains(const T& value) const {
			return std::find(vec.begin(), vec.end(), value) != vec.end();
		}

		bool remove(const T& value) {
			auto it = std::find(vec.begin(), vec.end(), value);
			if (it != vec.end()) {
				vec.erase(it);
				return true;
			}
			return false;
		}

		void removeAll(const T& value) {
			vec.erase(std::remove(vec.begin(), vec.end(), value), vec.end());
		}

		bool removeAt(std::size_t index) {
			if (index >= vec.size()) {
				return false;
			}
			vec.erase(vec.begin() + index);
			return true;
		}

		template <typename Predicate>
		int removeIf(Predicate pred) {
			auto oldSize = vec.size();
			auto it = std::remove_if(vec.begin(), vec.end(), pred);
			if (it != vec.end()) {
				vec.erase(it, vec.end());
			}
			return static_cast<int>(oldSize - vec.size());
		}

		size_t count(const T& value) const {
			return std::count(vec.begin(), vec.end(), value);
		}

		size_t count_if(std::function<bool(const T&)> predicate) const {
			return std::count_if(vec.begin(), vec.end(), predicate);
		}

		template <typename Predicate>
		typename std::vector<T>::iterator findIf(Predicate pred) {
			return std::find_if(vec.begin(), vec.end(), pred);
		}

		void insert_at(size_t index, const T& value) {
			if (index > vec.size()) {
				throw std::out_of_range("Index out of range");
			}
			vec.insert(vec.begin() + index, value);
		}

		T first() const {
			if (vec.empty()) {
				throw std::runtime_error("Vector is empty");
			}
			return vec.front();
		}

		T last() const {
			if (vec.empty()) {
				throw std::runtime_error("Vector is empty");
			}
			return vec.back();
		}

		void reverse() {
			std::reverse(vec.begin(), vec.end());
		}

		void sort() {
			std::sort(vec.begin(), vec.end());
		}

		void sortDescending() {
			std::sort(vec.begin(), vec.end(), std::greater<T>());
		}

		void sortBy(std::function<bool(const T&, const T&)> comparator) {
			std::sort(vec.begin(), vec.end(), comparator);
		}

		template<typename U = T>
		U sum() const {
			return std::accumulate(vec.begin(), vec.end(), U{});
		}

		template<typename U = T>
		U product() const {
			return std::accumulate(vec.begin(), vec.end(), U{ 1 }, std::multiplies<U>());
		}

		double average() const {
			if (vec.empty()) {
				throw std::runtime_error("Vector is empty");
			}
			return static_cast<double>(sum()) / size();
		}

		T min() const {
			if (vec.empty()) {
				throw std::runtime_error("Vector is empty");
			}
			return *std::min_element(vec.begin(), vec.end());
		}

		T max() const {
			if (vec.empty()) {
				throw std::runtime_error("Vector is empty");
			}
			return *std::max_element(vec.begin(), vec.end());
		}

		void swap(Vector<T>& other) {
			vec.swap(other.vec);
		}

	private:
		std::vector<T> vec;
	};
}