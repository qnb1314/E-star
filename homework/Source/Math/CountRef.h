#pragma once
#include <iostream>
/*************************************************

Author:乔南

Date:2022-3.25

Description:智能计数指针类,当引用归零时自动delete指针

**************************************************/
template<class Type>
class CountRef
{
public:
	CountRef(Type* t) :target(t)
	{ 
		count = new int(0); 
		Load();
	}
	CountRef(const CountRef<Type>& countref)
	{
		target = countref.target;
		count = countref.count;
		Load(); 
	}
	~CountRef() { UnLoad(); }
	Type* operator->() const
	{
		return target;
	}
	CountRef<Type>& operator=(CountRef<Type> countref)
	{
		UnLoad();
		target = countref.target;
		count = countref.count;
		Load();
		return *this;
	}
	CountRef<Type>& operator=(Type* type)
	{
		UnLoad();
		target = type;
		count = new int(0);
		Load();
		return *this;
	}
	bool operator==(CountRef<Type> countref)
	{
		return target == countref.target;
	}
private:
	Type* target;
	int* count;
	void Load() { (*count)++; }
	void UnLoad()
	{
		(*count)--;
		if ((*count) == 0)
		{
			delete target;
			delete count;
		}
		else if ((*count) < 0)
		{
			std::cout << "ERROR:引用计数小于零!" << std::endl;
		}
	}
};