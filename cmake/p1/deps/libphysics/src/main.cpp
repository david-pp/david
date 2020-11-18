#include <iostream>
#include "physics.h"

#include "tinymath.h"

int main(int argc, const char** argv) {
    std::cout << physics::velocity(10.0f, 10) << std::endl;
    std::cout << tinymath::square(10) << std::endl;
    return 0;
}

