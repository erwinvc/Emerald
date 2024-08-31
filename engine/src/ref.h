#pragma once
#include <atomic>

namespace emerald {
	template<typename T>
	class Ref;

	template<typename T>
	class WeakRef;

	class RefCounted {
	public:
		std::atomic<uint32_t> m_refCount = 0;
	};

	template<typename T>
	class Ref {
	public:
		// Default constructor: Creates an empty Ref object with no referenced object.
		Ref() : m_reference(nullptr) {}

		// Nullptr constructor: Creates an empty Ref object with no referenced object.
		Ref(std::nullptr_t) : m_reference(nullptr) {}

		// Pointer constructor: Creates a Ref object that references the given object.
		Ref(T* pointer) : m_reference(pointer) {
			incrementRef();
		}

		// Copy constructor: Creates a new Ref object that references the same object as the given Ref object.
		// The referenced object's reference count is incremented.
		Ref(const Ref& other) : m_reference(other.m_reference) {
			incrementRef();
		}

		Ref& operator=(std::nullptr_t) {
			decrementRef();
			m_reference = nullptr;
			return *this;
		}

		// Move constructor: Creates a new Ref object that takes ownership of the referenced object from the given Ref object.
		// The referenced object's reference count is not incremented.
		Ref(Ref&& other) noexcept : m_reference(other.m_reference) {
			other.m_reference = nullptr;
		}

		// Copy assignment operator: Assigns the referenced object of the given Ref object to this Ref object.
		// The previously referenced object's reference count is decremented, and the new referenced object's reference count is incremented.
		Ref& operator=(const Ref& other) {
			if (this != &other) {
				decrementRef();
				m_reference = other.m_reference;
				incrementRef();
			}
			return *this;
		}

		// Move assignment operator: Assigns the referenced object of the given Ref object to this Ref object, transferring ownership.
		// The previously referenced object's reference count is decremented, but the new referenced object's reference count is not incremented.
		Ref& operator=(Ref&& other) noexcept {
			if (this != &other) {
				decrementRef();
				m_reference = other.m_reference;
				other.m_reference = nullptr;
			}
			return *this;
		}

		~Ref() {
			decrementRef();
		}

		operator bool() { return m_reference != nullptr; }
		operator bool() const { return m_reference != nullptr; }

		T* operator->() { return m_reference; }
		const T* operator->() const { return m_reference; }

		T& operator*() { return *m_reference; }
		const T& operator*() const { return *m_reference; }

		T* raw() { return m_reference; }
		const T* raw() const { return m_reference; }

		template<typename T2>
		Ref<T2> as() const {
			return Ref<T2>(*this);
		}

		template<typename... Args>
		static Ref<T> create(Args&&... args) {
			return Ref<T>(new T(std::forward<Args>(args)...));
		}

		void reset() {
			decrementRef();
			m_reference = nullptr;
		}

		bool operator==(const Ref<T>& other) const {
			return m_reference == other.m_reference;
		}

		bool operator!=(const Ref<T>& other) const {
			return !(*this == other);
		}

		bool EqualsObject(const Ref<T>& other) {
			if (!m_reference || !other.m_reference)
				return false;

			return *m_reference == *other.m_reference;
		}

		uint32_t getRefCount() const {
			if (m_reference) {
				return m_reference->m_refCount.load();
			} else return 0;
		}

	private:
		template<class T2>
		friend class Ref;
		template<class T2>
		friend class WeakRef;

		T* m_reference;

		void incrementRef() const {
			if (m_reference) {
				auto& refCount = const_cast<std::atomic<uint32_t>&>(m_reference->m_refCount); // Cast away constness in case we're working with a const Ref object
				refCount++;
			}
		}

		void decrementRef() const {
			if (m_reference) {
				auto& refCount = const_cast<std::atomic<uint32_t>&>(m_reference->m_refCount);
				refCount--;
				if (refCount == 0) {
					delete m_reference;
				}
			}
		}
	};

	template<typename T>
	class UniqueRef {
	public:
		// Default constructor: Creates an empty UniqueRef object with no referenced object.
		UniqueRef() : m_reference(nullptr) {}

		// Nullptr constructor: Creates an empty UniqueRef object with no referenced object.
		UniqueRef(std::nullptr_t) : m_reference(nullptr) {}

		// Pointer constructor: Creates a UniqueRef object that owns the given object.
		explicit UniqueRef(T* pointer) : m_reference(pointer) {}

		// Move constructor: Transfers ownership from another UniqueRef object.
		UniqueRef(UniqueRef&& other) noexcept : m_reference(other.m_reference) {
			other.m_reference = nullptr;
		}

		// Move assignment operator: Transfers ownership from another UniqueRef object.
		UniqueRef& operator=(UniqueRef&& other) noexcept {
			if (this != &other) {
				reset();
				m_reference = other.m_reference;
				other.m_reference = nullptr;
			}
			return *this;
		}

		// Delete copy constructor and copy assignment operator to prevent copying.
		UniqueRef(const UniqueRef&) = delete;
		UniqueRef& operator=(const UniqueRef&) = delete;

		~UniqueRef() {
			reset();
		}

		operator bool() const { return m_reference != nullptr; }

		T* operator->() { return m_reference; }
		const T* operator->() const { return m_reference; }

		T& operator*() { return *m_reference; }
		const T& operator*() const { return *m_reference; }

		T* raw() { return m_reference; }
		const T* raw() const { return m_reference; }

		void reset(T* pointer = nullptr) {
			if (m_reference) {
				delete m_reference;
			}
			m_reference = pointer;
		}

		T* release() {
			T* temp = m_reference;
			m_reference = nullptr;
			return temp;
		}

		bool operator==(const UniqueRef<T>& other) const {
			return m_reference == other.m_reference;
		}

		bool operator!=(const UniqueRef<T>& other) const {
			return !(*this == other);
		}

		template<typename... Args>
		static UniqueRef<T> create(Args&&... args) {
			return UniqueRef<T>(new T(std::forward<Args>(args)...));
		}

	private:
		T* m_reference;
	};

	template<typename T>
	class WeakRef {
	public:
		WeakRef() : m_reference(nullptr) {}

		WeakRef(std::nullptr_t) : m_reference(nullptr) {}

		WeakRef(const Ref<T>& ref) : m_reference(ref.m_reference) {}

		WeakRef(const WeakRef& other) : m_reference(other.m_reference) {}

		WeakRef(WeakRef&& other) noexcept : m_reference(other.m_reference) {
			other.m_reference = nullptr;
		}

		WeakRef& operator=(std::nullptr_t) {
			m_reference = nullptr;
			return *this;
		}

		WeakRef& operator=(const Ref<T>& ref) {
			m_reference = ref.m_reference;
			return *this;
		}

		WeakRef& operator=(const WeakRef& other) {
			if (this != &other) {
				m_reference = other.m_reference;
			}
			return *this;
		}

		WeakRef& operator=(WeakRef&& other) noexcept {
			if (this != &other) {
				m_reference = other.m_reference;
				other.m_reference = nullptr;
			}
			return *this;
		}

		~WeakRef() = default;

		operator bool() const { return m_reference != nullptr && m_reference->m_refCount > 0; }

		bool expired() const {
			return m_reference == nullptr || m_reference->m_refCount == 0;
		}

		Ref<T> lock() const {
			return expired() ? Ref<T>(nullptr) : Ref<T>(m_reference);
		}

		T* raw() { return  m_reference; }
		const T* raw() const { return  m_reference; }

		bool operator==(const WeakRef<T>& other) const {
			return m_reference == other.m_reference;
		}

		bool operator!=(const WeakRef<T>& other) const {
			return !(*this == other);
		}

	private:
		T* m_reference;
	};
}