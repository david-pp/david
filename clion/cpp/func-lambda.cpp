#include <iostream>
#include <vector>
#include <algorithm>

//
// 《Lambda表达式》
//  - 闭包
//  - 匿名函数对象
//
// http://en.cppreference.com/w/cpp/language/lambda
//

////////////////////////////////////////////////////

// 简单用法
void test_1()
{
    std::cout << "----" << __PRETTY_FUNCTION__ << std::endl;

    int a = 1, b = 1, c = 1;

    auto m1 = [a, &b, &c]() mutable {
        auto m2 = [a, b, &c]() mutable {
            std::cout << a << b << c << '\n';
            a = 4; b = 4; c = 4;
        };
        a = 3; b = 3; c = 3;
        m2();
    };

    a = 2; b = 2; c = 2;

    m1();                             // calls m2() and prints 123
    std::cout << a << b << c << '\n'; // prints 234
}

void test_2()
{
    std::cout << "----" << __PRETTY_FUNCTION__ << std::endl;

    std::vector<int> c = {1, 2, 3, 4, 5, 6, 7, 3};

    std::for_each(c.begin(), c.end(), [](int x) {
        std::cout << x << " ";
    });

    std::cout << std::endl;

    c.erase(std::remove_if(c.begin(), c.end(), [](int n) { return n < 5; }), c.end());

    std::for_each(c.begin(), c.end(), [](int x) {
        std::cout << x << " ";
    });

    std::cout << std::endl;
}

int main(int argc, const char* argv[])
{
    test_1();
    test_2();
}