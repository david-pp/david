#include <iostream>

//
// 完美转发: std::forward
//
// 在函数模板中，完全依照模板的参数类型，将参数传递给其中调用的另外一个函数。目标：
//  - 参数为引用类型
//  - 能够接受左值引用、右值引用
//
// 引用折叠(reference collapsing): 一旦定义中出现了左值引用，引用折叠总是优先将其折叠为左值引用
//
//  模板参数类型 | 变量声明的类型 | 实际类型
// -------------------------------------
//    T&            X             X&
//    T&            X&            X&
//    T&            X&&           X&
//    T&&           X             X&&
//    T&&           X&            X&
//    T&&           X&&           X&&
//

// 不完美转发
namespace test1 {
    void callee(int v) { std::cout << __PRETTY_FUNCTION__ << std::endl; }
//    void callee(int& v) { std::cout << __PRETTY_FUNCTION__ << std::endl; }
//    void callee(const int& v) { std::cout << __PRETTY_FUNCTION__ << std::endl; }

    template<typename T>
    void caller(T v) {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        callee(v);
    }

    void run() {
        int v = 0;
        caller(v);
        caller<int &>(v);
        caller<const int &>(v);
    }
}

// 完美转发: std::forward <=> static_cast<T&&>
namespace test2{
    void callee(int& v) { std::cout << __PRETTY_FUNCTION__ << std::endl; }
    void callee(const int& v) { std::cout << __PRETTY_FUNCTION__ << std::endl; }

    void callee(int&& v) { std::cout << __PRETTY_FUNCTION__ << std::endl; }
    void callee(const int&& v) { std::cout << __PRETTY_FUNCTION__ << std::endl; }

    template<typename T>
    void caller(T&& v) {
        std::cout << "---" << std::endl;
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        callee(std::forward<T>(v));
    }

    void run() {
        int a;
        int b;
        const int c = 1;
        const int d = 2;

        caller(a);            // 左值引用
        caller(std::move(b)); // 右值引用
        caller(c);            // const左值引用
        caller(std::move(c)); // const右值引用

        std::make_pair(1, 2);
    }
}

int main() {
//    test1::run();
    test2::run();
    return 0;
}