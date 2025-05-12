#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> numeros = {10, 20, 30};
    numeros.push_back(40);

    for (int num : numeros) {
        cout << num << " ";
    }

    return 0;
}

