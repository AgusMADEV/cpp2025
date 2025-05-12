#include <iostream>

class Persona{
	private:
    	int edad;  // Propiedad pública
   public:
		void setEdad(int nuevaedad){
			edad = nuevaedad;
		}
		int getEdad(){
			return edad;
		}
};

int main() {
	Persona Agustínvicente;
	Agustínvicente.setEdad(46);
	std::cout << "La edad es de " << Agustínvicente.getEdad() << " años" << std::endl;
    return 0;
}
