#include <iostream>

int main() {
	 bool comparacion;
	 comparacion = 4 == 4 && 3 == 3 && 2 == 2;
    std::cout << comparacion << std::endl;
    comparacion = 4 == 4 && 3 == 3 && 2 == 1;
    std::cout << comparacion << std::endl;
    
    comparacion = 4 == 4 || 3 == 3 || 2 == 2;
    std::cout << comparacion << std::endl;
    comparacion = 4 == 4 || 3 == 3 || 2 == 1;
    std::cout << comparacion << std::endl;
    comparacion = 4 == 4 || 3 == 2 || 2 == 1;
    std::cout << comparacion << std::endl;
    comparacion = 4 == 3 || 3 == 2 || 2 == 1;
    std::cout << comparacion << std::endl;
    return 0;
}
