#pragma once

template<typename T>
class Ref {
private:
	T* m_object = nullptr;
public:
	Ref() {
		Set(nullptr);
	}

	Ref(T* object) {
		Set(object);
	}

	Ref(const Ref& other) {
		this->operator=(other);
	}

	Ref& operator=(const Ref& other) {
		Set(other.Get());
		return *this;
	}

	operator T*() { return m_object; }
	operator const T*() { return m_object; }

	~Ref() {
		Set(nullptr);
	}

	T* Get()const {
		return m_object;
	}

	void Set(T* object) {
		m_object = object;
	}

	T* operator ->() { return m_object; }
	const T* operator ->() const { return m_object; }

	T& operator *() { return *m_object; }
	const T& operator *() const { return *m_object; }

	bool operator==(const Ref<T>& other) const {
		return m_object == other.m_object;
	}
};