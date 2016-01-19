

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <iostream>


// 普通函数
int sub(int a, int b)
{
	std::cout << __PRETTY_FUNCTION__ << std::endl;
	return a - b;
}

// 函数指针
typedef int (* subfunc_t) (int a, int b);
subfunc_t subptr = sub;

// 成员函数

struct demo
{
	int sub(int a, int b)
	{
		std::cout << __PRETTY_FUNCTION__ << std::endl;
		return a - b;
	}
};

demo a;
demo& ra = a;
demo* pa = &a;

// 函数对象

struct functor
{
	typedef int result_type;

	int operator () (int a, int b)
	{
		std::cout << __PRETTY_FUNCTION__ << std::endl;
		return a - b;
	}
};


int main()
{
	{
		int v1 = boost::bind(sub, 1, 2)();
		int v2 = boost::bind(sub, _1, 2)(1);
		int v3 = boost::bind(subptr, _1, _2)(1, 2);
		int v4 = boost::bind(&demo::sub, ra, _1, 2)(1);
		int v5 = boost::bind<int>(functor(), _1, 2)(1);
		int v6 = boost::bind(functor(), _1, 2)(1);

		std::cout << v1 << std::endl;
		std::cout << v2 << std::endl;
		std::cout << v3 << std::endl;
		std::cout << v4 << std::endl;
		std::cout << v5 << std::endl;
	}

	{
		boost::function<int(int, int)> func1 = sub;
		boost::function<int(int, int)> func2 = subptr;
		boost::function<int(int, int)> func3 = functor();
		boost::function<int(int, int)> func4 = boost::bind(&demo::sub, a, _1, _2);

		std::cout << func1(1, 2) << std::endl;
		std::cout << func2(1, 2) << std::endl;
		std::cout << func3(1, 2) << std::endl;
		std::cout << func4(1, 2) << std::endl;
	}
}