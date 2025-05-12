#include <iostream>

int main() {
	int diadelasemana = 4;
	switch(diadelasemana){
		case 1: 
			std::cout << "Hoy es el peor dia de la semana" << std::endl;
			break;
		case 2: 
			std::cout << "Hoy es el segundo peor dia de la semana" << std::endl;
			break;
		case 3: 
			std::cout << "Ya estamos a mitad de semana" << std::endl;
			break;
		case 4: 
			std::cout << "Ya es casi viernes" << std::endl;
			break;
		case 5: 
			std::cout << "Hoy es el mejor dia de la semana" << std::endl;
			break;
		case 6: 
			std::cout << "Hoy es el peor dia de la semana" << std::endl;
			break;
		case 7: 
			std::cout << "Parece mentira que mañana ya sea lunes" << std::endl;
			break;
	}
    return 0;
}
