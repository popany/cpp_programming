#include <string>
#include <iostream>
#include <sstream>

int main()
{
    std::istringstream input;
    input.str("1|2|3");

    for (std::string s; std::getline(input, s, '|'); ) {
        std::cout << s << std::endl;
    }

    return 0;
}
