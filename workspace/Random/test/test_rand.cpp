#include <Random/Random.h>
#include <iostream>

using namespace std;

int main() {
    Random::Seed(1);
    for (int ii = 0; ii < 10; ++ii) {
        double x, y, z;
        x = Random::Uniform();
        y = Random::Gaussian();
        z = Random::Exponential(1.0);
        cout << x << " " << y << " " << z << endl;
    }
}
