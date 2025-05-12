#include <iostream>

int main() {
	int dia;
	dia = 40;
	do{
		std::cout << "Hoy es el dia " << dia << " del mes" << std::endl;
	 	dia++;
	}while(dia < 31);
    return 0;
}
