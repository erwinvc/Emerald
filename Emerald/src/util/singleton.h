#pragma once

template<typename T>
class Singleton {
public:
    static T* GetInstance() {
        static T instance;
        return &instance;
    }

	Singleton() {};
	~Singleton() {};
private:
    Singleton(Singleton const &) = delete;
    Singleton& operator=(Singleton const &) = delete;
};