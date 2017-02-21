#include <iostream>
#include <vector>
#include <algorithm>

// 左值(lvalue)：
//        - 出现在等号左边
//        - 可以取地址的、有名字的
// 右值(rvalue)：
//        - 出现在等号右边
//        - 不能取地址、没有名字的

// 《C++PLv4》 6.4
//
// 引用类型     |  非常量左值 | 常量左值 | 非常量右值 | 常量右值
// -------------------------------------------------------
//  X&               Y          N        N          N       - 传引用
// const X&          Y          Y        Y          Y       - 全能类型, 可用于拷贝语义
//  X&&              N          N        Y          N       - 用于移动语义、完美转发
// const X&&         N          N        Y          Y       - 无意义
//

int x;

// 右值引用
std::vector<int> magicNumbers() {
    return std::vector<int>{1, 3, 4, 5, 6, 7};
}

void test_rvalue() {
    // 纯右值(prvalue)
    int &&five = 3 + 2;

    // 将亡值(xvalue)
    std::vector<int> &&v = magicNumbers();

    // int&& xx = x;

    std::for_each(v.begin(), v.end(), [](int value) {
        std::cout << value << " ";
    });
    std::cout << std::endl;

    std::cout << five << std::endl;
}


// 左值引用
int &getRef() {
    return x;
}

void test_lvalue() {
    int a = 1;    // here, a is an lvalue
    int &ra = a;
    getRef() = 4; //  reference to a global variable

    // 常量左引用(万能引用)
    const int &rv = 100;
    const std::vector<int> &v = magicNumbers();
}

// 传值 vs. 传引用
struct X {
    X() {}

    X(const X &x) {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }
};

X getX() { return X(); }

void acceptVal(X x) {}
//void acceptRef(X& x) {}
void acceptRef(const X& x) {}
void acceptRRef(X&& x) {}

void test_3() {
    std::cout << "acceptVal:" << std::endl;
    acceptVal(getX());

    std::cout << "acceptRef:" << std::endl;
    acceptRef(getX());

    std::cout << "acceptRRef:" << std::endl;
    acceptRRef(getX());
}

int main() {
    test_rvalue();
    test_lvalue();
    test_3();
    return 0;
}


