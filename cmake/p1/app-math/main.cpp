#include <iostream>
#include <cmath>
#include "tinymath.h"

int main(int argc, const char** argv) {

#ifdef USE_STD_MATH
    std::cout << "stdmath =" << std::pow(10.0f, 2.0f) << std::endl;
#else
     std::cout << "tinymath =" << tinymath::square(10.0f) << std::endl;
#endif

    return 0;
}

