#include <iostream>
using namespace std;

struct Persona {
    string nombre;
    int edad;
    float altura;
};

int main() {
    Persona p = {"Juan", 25, 1.75};
    cout << "Nombre: " << p.nombre << ", Edad: " << p.edad << ", Altura: " << p.altura << endl;
    return 0;
}

