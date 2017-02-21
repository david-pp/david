#include <iostream>
#include <cstring>


//
// 移动语义: std::move
//

class String {
public:
    String() {
        buf_ = nullptr;
        buflen_ = 0;
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }

    ~String() {
        if (buf_) delete[] buf_;
        std::cout << __PRETTY_FUNCTION__ << std::endl;
    }

    String(const char *cstr) {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        size_t len = std::strlen(cstr);
        buf_ = new char[len + 1];
        strncpy(buf_, cstr, len);
        buf_[len] = 0;
        buflen_ = len + 1;
    }

    // 拷贝构造
    String(const String &rhs) {
        std::cout << __PRETTY_FUNCTION__ << std::endl;

        buf_ = new char[rhs.buflen_];
        std::memcpy(buf_, rhs.buf_, rhs.buflen_);
        buflen_ = rhs.buflen_;
    }

    // 拷贝赋值
    String &operator=(const String &rhs) {
        std::cout << __PRETTY_FUNCTION__ << std::endl;

        buf_ = new char[rhs.buflen_];
        std::memcpy(buf_, rhs.buf_, rhs.buflen_);
        buflen_ = rhs.buflen_;
    }

    // 移动构造
    String(String &&rhs) {
        std::cout << __PRETTY_FUNCTION__ << std::endl;

        buf_ = rhs.buf_;
        buflen_ = rhs.buflen_;

        rhs.buf_ = nullptr;
        rhs.buflen_ = 0;
    }

    // 移动赋值
    String& operator=(String&& rhs) {
        std::cout << __PRETTY_FUNCTION__ << std::endl;

        buf_ = rhs.buf_;
        buflen_ = rhs.buflen_;

        rhs.buf_ = nullptr;
        rhs.buflen_ = 0;
    }

    const char *c_str() { return buf_ ? buf_ : ""; }

private:
    char *buf_;
    size_t buflen_;
};

//
// Copy vs. Move
//
String getName() {
    return String("david");
}

void test_1() {
    // https://gcc.gnu.org/onlinedocs/gcc/C_002b_002b-Dialect-Options.html
    // https://en.wikipedia.org/wiki/Return_value_optimization
    // -fno-elide-constructors
    String name = getName();
}

//
// std::move <=> static_cast<T&&>(lvalue)
//
struct PhoneInfo {
    PhoneInfo(int size) : name("david"), numbers(new int[size]) {}

//    PhoneInfo(PhoneInfo&& rhs) : name(rhs.name), numbers(rhs.numbers) {}
    PhoneInfo(PhoneInfo &&rhs) noexcept : name(std::move(rhs.name)), numbers(rhs.numbers) {}

    String name;
    int *numbers;
};

PhoneInfo getPhone() {
    PhoneInfo phone(12);
    std::cout << phone.name.c_str() << "@" << (void *) phone.name.c_str() << std::endl;
    std::cout << "number@" << (void *) phone.numbers << std::endl;
    return phone;
}

void test_2() {
    PhoneInfo phone = getPhone();
    std::cout << phone.name.c_str() << "@" << (void *) phone.name.c_str() << std::endl;
    std::cout << "number@" << (void *) phone.numbers << std::endl;
}

template<typename T>
void myswap(T &a, T &b) {
    T tmp = a;
    a = b;
    b = tmp;
}

template<typename T>
void myswap2(T &a, T &b) {
    T tmp(std::move(a));
    a = std::move(b);
    b = std::move(tmp);
}

void test_3() {
    String a("david");
    String b("wang");

//    myswap(a, b);
    myswap2(a, b);

    std::cout << a.c_str() << std::endl;
    std::cout << b.c_str() << std::endl;
}


int main() {
//    test_1();
//    test_2();
    test_3();
    return 0;
}