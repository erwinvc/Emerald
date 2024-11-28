#pragma once
#include <atomic>
#include <type_traits>

namespace emerald {
	template<typename T>
	class Ref;

	template<typename T>
	class WeakRef;

	struct ControlBlock {
		std::atomic<uint32_t> strongCount = 0;
		std::atomic<uint32_t> weakCount = 0;
	};

	class RefCounted {
	public:
		template<typename T>
		friend class Ref;

		template<typename T>
		friend class WeakRef;

		virtual ~RefCounted() = default;

		uint32_t getRefCount() const {
			return m_controlBlock ? m_controlBlock->strongCount.load() : 0;
		}

	protected:
		RefCounted() : m_controlBlock(new ControlBlock()) {}
		RefCounted(const RefCounted&) = delete;
		RefCounted& operator=(const RefCounted&) = delete;

	private:
		ControlBlock* m_controlBlock;
	};

	template<typename T>
	class Ref {
	public:
		// Default constructor: Creates an empty Ref object with no referenced object.
		constexpr Ref() : m_reference(nullptr), m_controlBlock(nullptr) {}
		// Nullptr constructor: Creates an empty Ref object with no referenced object.
		constexpr Ref(std::nullptr_t) : m_reference(nullptr), m_controlBlock(nullptr) {}

		// Pointer constructor: Creates a Ref object that references the given object.
		Ref(T* pointer) : m_reference(pointer), m_controlBlock(pointer ? pointer->m_controlBlock : nullptr) {
			incrementRef();
		}

		// Copy constructor: Creates a new Ref object that references the same object as the given Ref object.
		// The referenced object's reference count is incremented.
		Ref(const Ref& other) : m_reference(other.m_reference), m_controlBlock(other.m_controlBlock) {
			incrementRef();
		}

		// Move constructor: Creates a new Ref object that takes ownership of the referenced object from the given Ref object.
		// The referenced object's reference count is not incremented.
		Ref(Ref&& other) : m_reference(other.m_reference), m_controlBlock(other.m_controlBlock) {
			other.m_reference = nullptr;
			other.m_controlBlock = nullptr;
		}

		// Copy constructor for derived types: Creates a new Ref object that references the same object as the given Ref object.
		template<typename U>
		Ref(const Ref<U>& other) : m_reference(dynamic_cast<T*>(other.m_reference)), m_controlBlock(other.m_controlBlock) {
			static_assert(std::is_base_of<T, U>::value || std::is_base_of<U, T>::value,
				"Types must share an inheritance relationship");
			incrementRef();
		}

		Ref& operator=(std::nullptr_t) {
			decrementRef();
			m_reference = nullptr;
			m_controlBlock = nullptr;
			return *this;
		}

		// Copy assignment operator: Assigns the referenced object of the given Ref object to this Ref object.
		// The previously referenced object's reference count is decremented, and the new referenced object's reference count is incremented.
		Ref& operator=(const Ref& other) {
			if (this != &other) {
				decrementRef();
				m_reference = other.m_reference;
				m_controlBlock = other.m_controlBlock;
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
				m_controlBlock = other.m_controlBlock;
				other.m_reference = nullptr;
				other.m_controlBlock = nullptr;
			}
			return *this;
		}

		//template<typename U>
		//Ref& operator=(Ref<U>&& other) noexcept {
		//	static_assert(std::is_base_of<T, U>::value || std::is_base_of<U, T>::value, "Types must share an inheritance relationship");
		//	if (this != &other) {
		//		decrementRef();
		//		m_reference = other.m_reference;
		//		other.m_reference = nullptr;
		//	}
		//	return *this;
		//}

		~Ref() {
			decrementRef();
		}

		operator bool() { return m_reference != nullptr; }
		operator bool() const { return m_reference != nullptr; }

		T* operator->() { return m_reference; }
		T* operator->() const { return m_reference; }

		T& operator*() { return *m_reference; }
		T& operator*() const { return *m_reference; }

		T* raw() { return m_reference; }
		T* raw() const { return m_reference; }

		//explicit Ref(const WeakRef<T>& weak) : m_reference(nullptr) {
		//	if (!weak.expired()) {
		//		m_reference = weak.m_reference;
		//		incrementRef();
		//	}
		//}

		template<typename T2>
		Ref<T2> as() const {
			T2* casted = dynamic_cast<T2*>(m_reference);
			if (casted) {
				return Ref<T2>(casted);
			}
			return Ref<T2>(nullptr);
		}


		template<typename... Args>
		static Ref<T> create(Args&&... args) {
			return Ref<T>(new T(std::forward<Args>(args)...));
		}

		void reset() {
			decrementRef();
			m_reference = nullptr;
		}

		T* release() {
			T* tmp = m_reference;
			if (m_controlBlock) {
				m_controlBlock->strongCount--;
				if (m_controlBlock->weakCount == 0) {
					delete m_controlBlock;
				}
			}
			m_reference = nullptr;
			m_controlBlock = nullptr;
			return tmp;
		}

		bool operator==(const Ref<T>& other) const {
			return m_reference == other.m_reference;
		}

		bool operator!=(const Ref<T>& other) const {
			return !(*this == other);
		}

		bool operator==(std::nullptr_t) const { return m_reference == nullptr; }
		bool operator!=(std::nullptr_t) const { return m_reference != nullptr; }

		bool EqualsObject(const Ref<T>& other) {
			if (!m_reference || !other.m_reference)
				return false;

			return *m_reference == *other.m_reference;
		}

		uint32_t getRefCount() const {
			return m_controlBlock ? m_controlBlock->strongCount.load() : 0;
		}

	private:
		template<class T2>
		friend class Ref;
		template<class T2>
		friend class WeakRef;

		T* m_reference;
		ControlBlock* m_controlBlock;

		void incrementRef() const {
			if (m_controlBlock) {
				auto& refCount = const_cast<std::atomic<uint32_t>&>(m_controlBlock->strongCount); // Cast away constness in case we're working with a const Ref object
				refCount++;
			}
		}

		void decrementRef() {
			if (m_controlBlock) {
				if (--m_controlBlock->strongCount == 0) {
					delete m_reference;
					m_reference = nullptr;
					if (m_controlBlock->weakCount == 0) {
						delete m_controlBlock;
						m_controlBlock = nullptr;
					}
				}
			}
		}
	};

	template<typename T>
	bool operator==(std::nullptr_t, const Ref<T>& ref) {
		return ref == nullptr;
	}

	template<typename T>
	bool operator!=(std::nullptr_t, const Ref<T>& ref) {
		return ref != nullptr;
	}

	template<typename T>
	class UniqueRef {
	public:
		// Default constructor: Creates an empty UniqueRef object with no referenced object.
		constexpr UniqueRef() : m_reference(nullptr) {}

		// Nullptr constructor: Creates an empty UniqueRef object with no referenced object.
		constexpr UniqueRef(std::nullptr_t) : m_reference(nullptr) {}

		// Pointer constructor: Creates a UniqueRef object that owns the given object.
		explicit UniqueRef(T* pointer) : m_reference(pointer) {}

		// Move constructor: Transfers ownership from another UniqueRef object.
		UniqueRef(UniqueRef&& other) noexcept : m_reference(other.m_reference) {
			other.m_reference = nullptr;
		}

		// Templated move constructor for upcasting
		template<typename U, typename = std::enable_if_t<std::is_base_of_v<T, U>>>
		UniqueRef(UniqueRef<U>&& other) noexcept
			: m_reference(other.m_reference) {
			other.m_reference = nullptr;
		}

		// Templated move assignment operator for upcasting
		template<typename U, typename = std::enable_if_t<std::is_base_of_v<T, U>>>
		UniqueRef& operator=(UniqueRef<U>&& other) noexcept {
			if (this != reinterpret_cast<UniqueRef*>(&other)) {
				reset();
				m_reference = other.m_reference;
				other.m_reference = nullptr;
			}
			return *this;
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
		T* operator->() const { return m_reference; }

		T& operator*() { return *m_reference; }
		T& operator*() const { return *m_reference; }

		T* raw() { return m_reference; }
		T* raw() const { return m_reference; }

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

		template<typename T2>
		UniqueRef<T2> as() const {
			T2* casted = dynamic_cast<T2*>(m_reference);
			if (casted) {
				return UniqueRef<T2>(casted);
			}
			return UniqueRef<T2>(nullptr);
		}

		template<typename... Args>
		static UniqueRef<T> create(Args&&... args) {
			return UniqueRef<T>(new T(std::forward<Args>(args)...));
		}

	private:
		T* m_reference;

		template<typename> friend class UniqueRef;
	};

	template<typename T>
	class WeakRef {
	public:
		constexpr WeakRef() : m_reference(nullptr), m_controlBlock(nullptr) {}
		constexpr WeakRef(std::nullptr_t) : m_reference(nullptr), m_controlBlock(nullptr) {}

		WeakRef(const Ref<T>& ref) : m_reference(ref.m_reference), m_controlBlock(ref.m_controlBlock) {
			if (m_controlBlock) {
				m_controlBlock->weakCount++;
			}
		}

		WeakRef(const WeakRef& other) : m_reference(other.m_reference), m_controlBlock(other.m_controlBlock) {
			if (m_controlBlock) {
				m_controlBlock->weakCount++;
			}
		}

		WeakRef(WeakRef&& other) : m_reference(other.m_reference), m_controlBlock(other.m_controlBlock) {
			other.m_reference = nullptr;
			other.m_controlBlock = nullptr;
		}

		~WeakRef() {
			if (m_controlBlock) {
				if (--m_controlBlock->weakCount == 0 && m_controlBlock->strongCount == 0) {
					delete m_controlBlock;
				}
			}
		}

		WeakRef& operator=(const Ref<T>& ref) {
			decrementRef();
			m_reference = ref.m_reference;
			m_controlBlock = ref.m_controlBlock;
			if (m_controlBlock) {
				m_controlBlock->weakCount++;
			}
			return *this;
		}

		WeakRef& operator=(const WeakRef& other) {
			if (this != &other) {
				decrementRef();
				m_reference = other.m_reference;
				m_controlBlock = other.m_controlBlock;
				if (m_controlBlock) {
					m_controlBlock->weakCount++;
				}
			}
			return *this;
		}

		bool expired() const {
			return m_controlBlock == nullptr || m_controlBlock->strongCount == 0 || m_reference == nullptr;
		}

		Ref<T> lock() const {
			if (expired()) {
				return Ref<T>(nullptr);
			}
			Ref<T> ref(m_reference);
			return ref;
		}

		T* raw() { return  m_reference; }
		const T* raw() const { return  m_reference; }

		bool operator==(const WeakRef<T>& other) const {
			return m_reference == other.m_reference;
		}

		bool operator!=(const WeakRef<T>& other) const {
			return !(*this == other);
		}

		bool operator==(std::nullptr_t) const { return m_reference == nullptr; }
		bool operator!=(std::nullptr_t) const { return m_reference != nullptr; }

	private:
		T* m_reference;
		ControlBlock* m_controlBlock;

		void decrementRef() {
			if (m_controlBlock) {
				if (--m_controlBlock->weakCount == 0 && m_controlBlock->strongCount == 0) {
					delete m_controlBlock;
				}
			}
		}
	};
}