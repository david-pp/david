//
// Created by wangdawei on 2017/2/14.
//

#include <iostream>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <functional>

//
//
// 《仿函数（Functor）》
//
//  - 带状态的函数
//  - 作为函数参数传递，STL <algorithm>
//

////////////////////////////////////////////////////////

// 简单用法
class Add
{
public:
    Add (int x) : _x( x ) {}
    int operator() (int y) { return _x + y; }

private:
    int _x;
};

void test_1()
{
    std::cout << "----" << __PRETTY_FUNCTION__ << std::endl;

    Add add5(5);

    std::cout << add5(4) << std::endl;
}


////////////////////////////////////////////////////////

// 作为函数参数进行传递
template <typename FuncType>
std::vector<int> find_matching_numbers(std::vector<int> vec, FuncType pred) {
    std::vector<int> ret_vec;
    std::vector<int>::iterator itr = vec.begin(), end = vec.end();
    while (itr != end) {
        if (pred(*itr)) {
            ret_vec.push_back(*itr);
        }
        ++itr;
    }
    return ret_vec;
}

std::ostream& operator << (std::ostream& oss, const std::vector<int>& v)
{
    for (auto i : v)
        oss << i << ",";
    return oss;
}

void test_2()
{
    std::cout << "----" << __PRETTY_FUNCTION__ << std::endl;

    std::vector<int> v1{1, 2, 3, 4, 5, 6, 7, 8};

    struct IsOdds
    {
        bool operator () (int x) { return x%2 == 1; }
    };

    std::vector<int> v2 = find_matching_numbers(v1, IsOdds());

    std::cout << v1 << std::endl;
    std::cout << v2 << std::endl;
}

////////////////////////////////////////////////////////

// Functors vs. Virtual Functions
// http://en.cppreference.com/w/cpp/utility/functional
//

// Functor
template <typename Type, typename FuncType>
Type doMath (Type x, Type y, FuncType func)
{
    return func(x, y);
}

// Virtual Functions
struct MathComputer
{
    virtual int computeResult (int x, int y) = 0;
};

struct MathAdd : public MathComputer
{
    int computeResult (int x, int y) { return x + y; }
};

int doMath2 (int x, int y, MathComputer* p_computer)
{
    return p_computer->computeResult( x, y );
}

void test_3()
{
    std::cout << "----" << __PRETTY_FUNCTION__ << std::endl;

    std::cout << doMath(10, 20, std::plus<int>()) << std::endl;
    std::cout << doMath(10.5, 20.0, std::plus<float>()) << std::endl;

    std::cout << doMath2(10, 20, new MathAdd) << std::endl;
}

////////////////////////////////////////////////////////

// STL
// http://en.cppreference.com/w/cpp/algorithm

void test_4()
{
    std::cout << "----" << __PRETTY_FUNCTION__ << std::endl;

    std::vector<int> v1{1, 2, 3, 4, 5, 6, 7, 8};

    struct GetOdds
    {
        GetOdds operator () (int x)
        {
            if (x%2 == 1)
                v2.push_back(x);
            return *this;
        }
        std::vector<int> v2;
    };

    GetOdds get = std::for_each(v1.begin(), v1.end(), GetOdds());

    std::cout << v1 << std::endl;
    std::cout << get.v2 << std::endl;
}

int main(int argc, const char* argv[])
{
    test_1();
    test_2();
    test_3();
    test_4();
}