#include <iostream>

class A
{
public:
    virtual void g(){
        std::cout << __PRETTY_FUNCTION__  << std::endl;
    }
};
class B : public A
{
public:
    virtual void g(){
        std::cout << __PRETTY_FUNCTION__  << std::endl;
    }
};
class C : public B
{
public:
    virtual void g(){
        std::cout << __PRETTY_FUNCTION__  << std::endl;
    }
};
class D : public C
{
public:
    virtual void g(){
        std::cout << __PRETTY_FUNCTION__  << std::endl;
    }
};

A* f1()
{
    A *pa = new C;
    B *pb = dynamic_cast<B*>(pa);
    return pb;
}

A* f2()
{
    A *pb = new B;
    C *pc = dynamic_cast<C*>(pb);
    return pc;
}

A* f3()
{
    A *pa = new D;
    B *pb = dynamic_cast<B*>(pa);
    return pb;
}

int main()
{
    f1()->g();  // (1)
//    f2()->g();   // (2)
    f3()->g();   // (3)

    return 0;
}