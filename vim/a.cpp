#include <iostream>
#include <typeinfo>

struct A {
    A(int i) {}

    A(int x, int y) {}
};

struct B : public A {
    using A::A;
};

template<typename T>
struct C : public A {
    using A::A;
};

template<typename T>
struct D : public T {
    using T::T;
};

int main() {
    B b(1);
    C<int> c(1);

    D<A> d(1);
    D<A> d2(1, 2);

    {
        int *a, *b;
        std::cout << typeid(a).name() << std::endl;
        std::cout << typeid(b).name() << std::endl;
    }
}

void test() {
    int a = 0;
    a = 100;
}

int a()
{
    a = 10;
    a = 10;
    a = 10;
    xxx == 10;
    aaa == 10;
}
