#include <iostream>

class Persona{
	public:
    	int edad;  // Propiedad pública
};

int main() {
	Persona Agustínvicente;
	Agustínvicente.edad = 46;
	std::cout << "La edad es de " << Agustínvicente.edad << " años" << std::endl;
    return 0;
}
