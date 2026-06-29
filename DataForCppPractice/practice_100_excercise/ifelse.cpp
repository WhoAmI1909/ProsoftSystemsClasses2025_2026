#include <iostream>

int main()
{
    int year;

    std::cout << "Enter the year: ";
    std::cin >> year;

    if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0)
    {
        std::cout << "Leap year\n";
    }
    else
    {
        std::cout << "Not leap year\n";
    }

    return 0;
}