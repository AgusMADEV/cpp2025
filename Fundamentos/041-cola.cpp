#include <iostream>
#include <queue>
using namespace std;

int main() {
    queue<string> cola;
    cola.push("Primero");
    cola.push("Segundo");
    cola.push("Tercero");

    while (!cola.empty()) {
        cout << cola.front() << " ";
        cola.pop();
    }

    return 0;
}

