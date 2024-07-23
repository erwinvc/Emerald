#pragma once
#include <atomic>

namespace emerald {
	template<typename T>
	class ManagedRef;

	class RefCounted {
	public:
		std::atomic<uint32_t> m_refCount = 0;
	};

	template<typename T>
	class ManagedRef {
	public:
		ManagedRef() {
			m_reference == nullptr;
		}

		ManagedRef(std::nullptr_t) {
			m_reference == nullptr;
		}

		ManagedRef(T* pointer) {
			m_reference = pointer;
			IncrementRef();
		}
		ManagedRef(const ManagedRef& managedRef) {
			m_reference = managedRef.m_reference;
			IncrementRef();
		}

		~ManagedRef() {
			DecrementRef();
		}

		ManagedRef& operator=(std::nullptr_t) {
			DecrementRef();
			m_reference = nullptr;
			return *this;
		}

		ManagedRef& operator=(const ManagedRef& other) {
			other->IncrementRef();
			DecrementRef();
			m_reference = other->m_reference;
			return *this;
		}

		operator bool() { return m_reference != nullptr; }
		operator bool() const { return m_reference != nullptr; }

		T* operator->() { return m_reference; }
		const T* operator->() const { return m_reference; }
		T& operator*() { return *m_reference; }
		const T& operator*() const { return *m_reference; }
		T* Raw() { return  m_reference; }
		const T* Raw() const { return  m_reference; }

		bool operator==(const ManagedRef<T>& other) const {
			return m_reference == other.m_reference;
		}

		bool operator!=(const ManagedRef<T>& other) const {
			return !(*this == other);
		}

		bool EqualsObject(const ManagedRef<T>& other) {
			if (!m_reference || !other.m_reference)
				return false;

			return *m_reference == *other.m_reference;
		}

		template<typename... Args>
		static ManagedRef<T> Create(Args&&... args) {
			return ManagedRef<T>(new T(std::forward<Args>(args)...));
		}

	private:
		T* m_reference;

		void IncrementRef() const {
			if (m_reference)
				m_reference->m_refCount++;
		}

		void DecrementRef() const {
			if (m_reference) {
				m_reference->m_refCount--;
				if (m_reference->m_refCount == 0) {
					delete m_reference;
				}
			}
		}
	};
}