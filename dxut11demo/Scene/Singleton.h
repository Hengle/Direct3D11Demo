#pragma once

template<typename T>
class Singleton
{
	friend  T;
public:
	static T* Instance()
	{
		static T* instance = new T();
		return instance;
	}

	virtual ~Singleton()
	{
		delete Instance();
	}
private:
	Singleton() = default;
};