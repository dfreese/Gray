#include <Random.h>
#include <iostream>

using namespace std;

int main() {
    for (int ii = 0; ii < 10; ++ii) {
        cout << Random::Uniform() << " "
             << Random::Gaussian() << " "
             << Random::Exponential(1.0) << " "
             << endl;
    }
}
