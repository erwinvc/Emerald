#pragma once

#pragma once

template<typename T>
class AssetRef {
private:
	T* m_object = nullptr;
public:
	AssetRef() {
		Set(nullptr);
	}

	AssetRef(T* object) {
		Set(object);
	}

	AssetRef(const AssetRef& other) {
		this->operator=(other);
	}

	AssetRef& operator=(const AssetRef& other) {
		Set(other.Get());
		return *this;
	}

	operator T*() { return m_object; }
	//operator const T*() { return m_object; }

	~AssetRef() {
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

	bool operator==(const AssetRef<T>& other) const {
		return m_object == other.m_object;
	}
};

template<typename T>
class ManagedRef {
private:
	shared_ptr<T> m_object = nullptr;
public:
	ManagedRef() {
		//Set(nullptr);
	}

	ManagedRef(T* object) {
		m_object = shared_ptr<T>(object);
	}

	//Ref(shared_ptr<T> object) {
	//	m_object = object;
	//}

	//Ref(const Ref& other) {
	//	this->operator=(other);
	//}
	//
	//Ref& operator=(T* other) {
	//	Set(other);
	//	return *this;
	//}
	//
	//Ref& operator=(const Ref& other) {
	//	Set(other.Get());
	//	return *this;
	//}
	//
	//operator shared_ptr<T>() { return m_object; }
	//operator const shared_ptr<T>() const { return m_object; }
	//
	operator bool() const { return m_object != nullptr; }
	//
	//~Ref() {
	//	Set(nullptr);
	//}
	//
	//shared_ptr<T> Get()const {
	//	return m_object;
	//}
	//

	//void Set(shared_ptr<T> object) {
	//	m_object = object;
	//}
	//
	//void Set(T* object) {
	//	m_object = make_shared<T>(object);
	//}
	//
	shared_ptr<T> operator ->() { return m_object; }
	const shared_ptr<T> operator ->() const { return m_object; }
	//
	//T& operator *() { return *m_object; }
	//const T& operator *() const { return *m_object; }
	//
	//bool operator==(const Ref<T>& other) const {
	//	return m_object == other.m_object;
	//}
};