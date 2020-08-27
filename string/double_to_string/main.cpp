#include <iomanip>
#include <sstream>
#include <iostream>
#include <string>

int main()
{
    double v = 3.14159;

    std::ostringstream ss;
    ss << std::fixed;
    ss << std::setprecision(2);
    ss << v;

    std::string s = ss.str();
    std::cout << s << std::endl;  // 3.14

    return 0;
}
