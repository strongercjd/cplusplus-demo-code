#include <iostream>
#include "math_utils.h"

int main()
{
    std::cout << "Math Utils Demo" << std::endl;
    std::cout << "===============" << std::endl;

    std::cout << "5 + 3 = " << MathUtils::add(5, 3) << std::endl;
    std::cout << "10 - 4 = " << MathUtils::subtract(10, 4) << std::endl;
    std::cout << "6 * 7 = " << MathUtils::multiply(6, 7) << std::endl;

    try
    {
        std::cout << "15 / 3 = " << MathUtils::divide(15, 3) << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cout << "Error: " << e.what() << std::endl;
    }

    std::cout << "Is 8 even? " << (MathUtils::isEven(8) ? "Yes" : "No") << std::endl;

    return 0;
}