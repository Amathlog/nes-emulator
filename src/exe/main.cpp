#include <iostream>
#include <core/bus.h>

int main(int, char**) {
    NesEmulator::Bus bus;
    bus.GetCPU().Clock();
    std::cout << "Hello, world!\n";
}
