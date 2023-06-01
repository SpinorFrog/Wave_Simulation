#include <iostream>

int sum(int x, int y);
int multiply(int x, int y);

int main()
{
    int num1 = 10, num2 = 4;
    int r1 = sum(num1, num2);
    int r2 = multiply(num1, num2);
    std::cout << "adding two numbers: " << num1 << ", " << num2 << " result: " << r1 << std::endl;
    std::cout << "multiplying two numbers: " << num1 << ", " << num2 << " result: " << r2 << std::endl;
}