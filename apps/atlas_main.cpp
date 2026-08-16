#include <iostream>
#include <string_view>

#include "atlas/core.hpp"

int main(int argc, char* argv[]) {
    if (argc == 2 && std::string_view(argv[1]) == "--version") {
        std::cout << atlas::version() << '\n';
        return 0;
    }

    std::cout << "ATLAS routing and optimization engine\n"
              << "Usage: atlas --version\n";
    return argc == 1 ? 0 : 2;
}
