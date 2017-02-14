//
// http://en.cppreference.com/w/cpp/utility/functional

#include <iostream>
#include <cstdio>
#include <functional>

//
// 函数对象(Function Object)：
//
//  1. 可以像函数一样使用的对象
//  2. 携带状态的函数
//  3. 形如：f(args...)
//

// 用途：
//  1. 函数对象作为函数的参数
//  2. 回调

// 演进：
//
//  语言：函数指针(普通、静态、成员函数) -> 仿函数(Functor) -> Lambda
//  思想：基于数据 -> 基于对象 -> 面向对象 -> 函数式
//
// C++标准库：<functional>

//  std::function
//  std::bind
//  std::mem_fn
//


//
// 函数对象的使用: std::function
//
//  - http://en.cppreference.com/w/cpp/utility/functional/function
//  - 可以存储、COPY、调用任何可以被执行的目标（函数、函数指针、Functor、Lambda表达式、Bind表达式等）

// -------------- 普通函数
void print_num(int i)
{
    std::cout << i << '\n';
}


void test_1()
{
    std::cout << "----" << __PRETTY_FUNCTION__ << std::endl;

    std::function<void(int)> print = print_num;
    print(100);
}

// --------------- 成员函数/成员变量
struct Foo {
    void display_number(int i) {
        std::cout << "my:" << my << " number: " << i << '\n';
    }

    int my = 10;
};

void test_2()
{
    std::cout << "----" << __PRETTY_FUNCTION__ << std::endl;

    // 成员函数-指针
    Foo foo;
    auto p = &Foo::display_number;
    (foo.*p)(10);

    // 成员函数-Functor
    auto foo_print = std::mem_fn(&Foo::display_number);
    foo_print(foo, 10);

    // 成员变量-地址
    auto pv = &Foo::my;
    std::cout << foo.*pv << std::endl;

    // 成员变量-Functor
    auto get_v = std::mem_fn(&Foo::my);
    std::cout << get_v(foo) << std::endl;

    // 使用统一的std::function
    std::function<void(int)> print = std::bind(&Foo::display_number, &foo, std::placeholders::_1);
    print(10);
}

// --------------- 仿函数 & Lambda表达式

struct Foo2 {
    void operator() (int i) {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        std::cout << "my:" << my << " number: " << i << '\n';
    }

    int my = 10;
};

void test_3()
{
    std::cout << "----" << __PRETTY_FUNCTION__ << std::endl;

    Foo2 foo;
    std::function<void(int)> print = std::bind(&Foo2::operator (), &foo, std::placeholders::_1);
    print(100);

    int my = 10;
    std::function<void(int)> print2 = [my](int i) {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        std::cout << "my:" << my << " number: " << i << '\n';
    };

    print2(200);
}


//
// 函数对象类型的转换: std::bind
//
//  - http://en.cppreference.com/w/cpp/utility/functional/bind
//  - bind( F&& f, Args&&... args )

// -------------- 普通函数
void print_3num(int a, int b, int c)
{
    std::cout << a << ", " << b << ", " << c << std::endl;
}

void test_bind_1()
{
    std::cout << "----" << __PRETTY_FUNCTION__ << std::endl;

    using namespace std::placeholders;

    // 保持原样
    std::function<void(int, int, int)> print3 = std::bind(print_3num, _1, _2, _3);
    print3(1, 2, 3);

    // 调整参数的顺序
    std::function<void(int, int, int)> print3r = std::bind(print_3num, _3, _2, _1);
    print3r(1, 2, 3);

    // 吃掉一个参数：c固定为100
    std::function<void(int, int)> print2 = std::bind(print_3num, _1, _2, 100);
    print2(1, 2);
    // 吃掉一个参数：b固定为100
    std::function<void(int, int)> print2_1 = std::bind(print_3num, _1, 100, _2);
    print2_1(1, 2);
}

// -------------- 成员函数/成员变量
struct Foo3 {
    void print_sum(int x, int y) {
        std::cout << "my:" << my << " sum: " << x+y << '\n';
    }

    int my = 10;
};

void test_bind_2()
{
    std::cout << "----" << __PRETTY_FUNCTION__ << std::endl;

    using namespace std::placeholders;

    Foo3 foo;

    // 成员函数
    std::function<void(int, int)> print = std::bind(&Foo3::print_sum, &foo, _1, _2);
    print(10, 20);

    // 成员函数吃掉一个参数
    std::function<void(int)> print100 = std::bind(&Foo3::print_sum, &foo, _1, 100);
    print100(2);

    // 成员变量
    {
        auto get_my = std::bind(&Foo3::my, &foo);
        std::cout << get_my() << std::endl;
    }
    {
        auto get_my = std::bind(&Foo3::my, _1);
        std::cout << get_my(foo) << std::endl;
    }
}

//////////////////////////////////////

template <typename TP, typename Class>
struct TinyMemFunctor
{
    TinyMemFunctor(TP Class::* pm) : pm_(pm) {}

    TP operator() (Class& obj)
    {
        return (obj.*pm_);
    }

    TP Class::* pm_;
};

template <typename TP, typename Class>
TinyMemFunctor<TP, Class> tiny_mem_fn(TP Class::* pm)
{
    return TinyMemFunctor<TP, Class>(pm);
};


int main(int argc, const char* argv[])
{
    test_1();
    test_2();
    test_3();
    test_bind_1();
    test_bind_2();
}