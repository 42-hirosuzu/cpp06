#include <iostream>
#include <cstdlib>
#include <ctime>
#include "Base.hpp"
#include "identify.hpp"

int main() {
    std::srand(static_cast<unsigned int>(std::time(NULL)));

    std::cout << "--- Pointer Identification ---" << std::endl;
    for (int i = 0; i < 5; ++i) {
        Base* p = generate();
        std::cout << "Test " << i + 1 << ": ";
        identify(p);
        delete p;
    }

    std::cout << "\n--- Reference Identification ---" << std::endl;
    for (int i = 0; i < 5; ++i) {
        Base* p = generate();
        std::cout << "Test " << i + 1 << ": ";
        identify(*p);
        delete p;
    }

    return 0;
}