#include <iostream>

#include "atlas/core.hpp"

int main() {
    if (atlas::version() != "0.1.0") {
        std::cerr << "Unexpected ATLAS version\n";
        return 1;
    }
    return 0;
}
