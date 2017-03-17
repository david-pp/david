//
// http://en.cppreference.com/w/cpp/utility/functional

#include <iostream>
#include <cstdio>


//
//
// 《Function Pointers(函数指针)》
//
//  1. 普通函数指针
//  2. 静态函数指针
//  3. 成员函数指针
//
//

//////////////////////////////////////////////////////////
//
// 普通函数指针/静态函数指针(C)
//
//////////////////////////////////////////////////////////

void func_1(int x)
{
    std::cout << x << std::endl;
}

void test_1()
{
    void (*func) (int);

    func = func_1;
    func(22);

    func = &func_1;
    func(22);
    (*func)(22);
}

//////////////////////////////////////////////////////////

// 函数指针 -> 成员函数
struct ObjectType_C
{
    static void constructor_default(ObjectType_C* obj)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        obj->init_ = 100;
        obj->add = &ObjectType_C::add_default;
    }

    static int add_default(ObjectType_C* obj, int x)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        return x + obj->init_;
    }

    void (* constructor)(ObjectType_C* self);

    int  (* add) (ObjectType_C* self, int x);

    int init_;
};

struct ObjectType_CPP
{
    ObjectType_CPP()
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        this->init_ = 100;
    }

    int add(int x)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        return x + this->init_;
    }

    int init_;
};

void test_2()
{
    ObjectType_C obj;
    obj.constructor = &ObjectType_C::constructor_default;
    obj.constructor(&obj);  // 构造函数
    obj.add(&obj, 100);

    ObjectType_CPP obj2;
    obj2.add(100);
}

//////////////////////////////////////////////////////////

// 函数指针实现多态
struct Base
{
    Base()
    {
        print = &Base::print_;
    }

    static void print_(Base* self, int a)
    {
        std::cout << __PRETTY_FUNCTION__ << ": " << a << std::endl;
    }

    void (* print) (Base* self, int a);
};

struct Derived : public Base
{
    Derived()
    {
        print = &Derived::print_;
    }

    static void print_(Base* self, int a)
    {
        std::cout << __PRETTY_FUNCTION__ << ": " << a << std::endl;
    }
};

void test_3()
{
    Base* b = new Base;
    b->print(b, 11);

    b = new Derived;
    b->print(b, 12);
}


//////////////////////////////////////////////////////////
//
// 成员函数指针(C++)
//
//////////////////////////////////////////////////////////

struct Test
{
public:
    void print_1(int a)
    {
        std::cout << __PRETTY_FUNCTION__ << ": " << a << std::endl;
    }
    void print_2(int a)
    {
        std::cout << __PRETTY_FUNCTION__ << ": " << a << std::endl;
    }
public:

};

void test_4()
{
    typedef void (Test::* PrintFunc)(int);

    PrintFunc print = &Test::print_1;

    Test t;
    (t.*print)(20);
}


int main(int argc, const char* argv[])
{
    test_1();
    test_2();
    test_3();
    test_4();
}