#include <iostream>
#include "eventloop.h"

int main() {
    using namespace tiny;

    EventLoop loop;
    loop.onIdle([]() {
                std::cout << "Idle1.." << std::endl;
            })
            .onIdle([]() {
                std::cout << "Idle2.." << std::endl;
            })
            .onTimer([]() {
                std::cout << "1s.." << std::endl;
            }, 1)
            .onTimer([]() {
                std::cout << "0.5s.." << std::endl;
            }, 0.5);

    loop.run();
}