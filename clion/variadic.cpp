
#include <iostream>
#include <cstdio>
#include <vector>

//
// Variadic macros
// http://en.cppreference.com/w/cpp/preprocessor/replace
//

#define PRINTF(format, ...) std::printf(format, __VA_ARGS__)


#define LOG(...) { \
        fprintf(stderr, "%s: Line %d:\t", __FILE__, __LINE__); \
        fprintf(stderr, __VA_ARGS__); \
        fprintf(stderr, "\n"); \
    }


void test_macro()
{
    std::cout <<"--------" << __PRETTY_FUNCTION__ << std::endl;

    PRINTF("hello %s ! %u\n", "world", 1);
    LOG("[error] %s", ".... test ...");
}

//
// Variadic functions
// http://en.cppreference.com/w/cpp/utility/variadic
//
#include <cstdarg>

int add_nums(int count, ...)
{
    int result = 0;
    va_list args;
    va_start(args, count);

    for (int i = 0; i < count; ++i)
        result += va_arg(args, int);

    va_end(args);
    return result;
}

void simple_vprintf(const char* fmt, va_list ap)
{
    while (*fmt != '\0') {
        if (*fmt == 'd') {
            int i = va_arg(ap, int);
            std::cout << i << '\n';
        } else if (*fmt == 'c') {
            // note automatic conversion to integral type
            int c = va_arg(ap, int);
            std::cout << static_cast<char>(c) << '\n';
        } else if (*fmt == 'f') {
            double d = va_arg(ap, double);
            std::cout << d << '\n';
        }
        ++fmt;
    }
}

void simple_printf(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    simple_vprintf(fmt, ap);
    va_end(ap);
}

// c++11
#include <initializer_list>

int add(std::initializer_list<int> values)
//int add(std::vector<int> values)
{
    int result = 0;
    for (auto i = values.begin(); i != values.end(); ++i)
        result += *i;
    return result;
}

void test_func()
{
    std::cout <<"--------" << __PRETTY_FUNCTION__ << std::endl;

    std::cout << add_nums(4, 1,2,3,4,5,6) << std::endl;

    simple_printf("dcff", 3, 'a', 1.999, 42.5);

    std::cout << add({1, 2, 3, 4, 5, 6}) << std::endl;
}


//
// Variadic templates
// http://en.cppreference.com/w/cpp/language/parameter_pack
//

// Variadic function template 1
int print_all()
{
    return 0;
}

template <typename T, typename... Args>
int print_all(T value, Args... args)
{
    std::cout << "print_all:" << value << std::endl;
    return 1 + print_all(args...);
}

// Variadic function template 2
void tprintf(const char* format)
{
    std::cout << format;
}

template <typename T, typename... Args>
void tprintf(const char* format, T value, Args... args)
{
    for ( ; *format != '\0'; format++ )
    {
        if (*format == '%')
        {
            std::cout << value;
            tprintf(format+1, args...);
            return;
        }
        else
        {
            std::cout << *format;
        }
    }
};


void test_func_template()
{
    std::cout <<"--------" << __PRETTY_FUNCTION__ << std::endl;
    std::cout << print_all(1, "fuck", 3.24, std::string("you")) << std::endl;

    tprintf("% world % %\n", std::string("hello"), "!", 3.146);
}

// Variadic class template

template <typename... Types> class Tuple;

template <typename T, typename... Types>
struct Tuple<T, Types...>
{
    Tuple<T, Types...>(T v, Types... args)
            : first(v), second(args...) {}

    T first;
    Tuple<Types...> second;
};

template <>
struct Tuple<> {};


template <typename... Types>
Tuple<Types...> makeTuple(Types... args)
{
    return Tuple<Types...>(args...);
}


template <size_t i, typename T1, typename... Types>
struct TupleElement
{
    typedef T1 Type;

    static typename TupleElement<i-1, Types...>::Type get(Tuple<T1, Types...>& t)
    {
        std::cout <<"--" << __PRETTY_FUNCTION__ << std::endl;
        return TupleElement<i-1, Types...>::get(t.second);
    }
};

template <typename T1, typename... Types>
struct TupleElement<0ul, T1, Types...>
{
    typedef T1 Type;

    static T1 get(Tuple<T1, Types...>& t)
    {
        std::cout <<"--" << __PRETTY_FUNCTION__ << std::endl;
        std::cout << t.first << std::endl;
        return t.first;
    }
};

template <size_t i, typename T1, typename... Args>
typename TupleElement<i, T1, Args...>::Type Get(Tuple<T1, Args...>& t)
{
    return TupleElement<i, T1, Args...>::get(t);
};

void test_class_template()
{
    std::cout <<"--------" << __PRETTY_FUNCTION__ << std::endl;

    Tuple<double, int, std::string> t1(1.2, 3, std::string("test"));

    auto t2 = makeTuple(1.3, 3, 'c');
    std::cout << Get<0>(t2) << std::endl;
    std::cout << Get<1>(t2) << std::endl;
    std::cout << Get<2>(t2) << std::endl;
}

int main()
{
    test_macro();
    test_func();
    test_func_template();
    test_class_template();
}

