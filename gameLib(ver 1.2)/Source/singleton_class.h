#pragma once
#include<memory>
//シングルトンクラス
template <class T>

class Singleton
{
public:
	//生成
	static void Create()
	{
		if (!ref())
		{
			ref() = std::make_unique<T>();
		}
	}
	//消去
	static void Destroy()
	{
		if(ref())ref().reset();
	}
	//インスタンス
	static const std::unique_ptr<T>& GetInctance()
	{
		return ref();
	}
protected:
	Singleton() {}

private:
	static std::unique_ptr<T>& ref()
	{
		static std::unique_ptr<T>p = std::make_unique<T>();
		return p;
	}
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;
	Singleton(Singleton&&) = delete;
	Singleton& operator=(Singleton&&) = delete;
};
