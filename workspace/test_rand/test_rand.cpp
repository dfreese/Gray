#include <mt19937.h>
#include <iostream>

using namespace std;

int main() {
    for (int ii = 0; ii < 10; ++ii) {
        cout << genrand() << endl;
    }
}
