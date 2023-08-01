#include "lib/parser.h"
#include <iostream>
#include <string>
#include <sstream>

using namespace omfl;

int main(int, char**) {
    std::string data = R"(
        key = 111
        key2 = 2
        key3 = false
        key4 = 23.12
        key5 = "sdasd"
        key6 = [1, 2, 3]

        [sda]
        key = 2
        [sda.sdas]
        key2 = 2
    )";

    Section params = parse(std::filesystem::path("C:\\Users\\realisatorq\\labs\\labwork-6-exc11g\\example\\config.omfl"));

    return 0;
}
