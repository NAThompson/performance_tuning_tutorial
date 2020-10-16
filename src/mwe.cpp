#include <iostream>
#include <vector>

double dot_product(double* a, double* b, size_t n) {
    double d = 0;
    for (size_t i = 0; i < n; ++i) {
        d += a[i]*b[i];
    }
    return d;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: ./dot 10\n";
        return 1;
    }
    size_t n = atoi(argv[1]);

    std::vector<double> a(n);
    std::vector<double> b(n);
    for (size_t i = 0; i < n; ++i) {
        a[i] = i;
        b[i] = 1/double(i+3);
    }

    double d = dot_product(a.data(), b.data(), n);
    std::cout << "a.b = " << d << "\n";
}
