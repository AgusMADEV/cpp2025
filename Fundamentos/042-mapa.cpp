#include <iostream>
#include <map>
using namespace std;

int main() {
    map<string, int> edad;
    edad["Alice"] = 25;
    edad["Bob"] = 30;

    for (auto& par : edad) {
        cout << par.first << ": " << par.second << endl;
    }

    return 0;
}

