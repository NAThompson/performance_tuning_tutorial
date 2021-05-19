#include <iostream>
#include <vector>
#include <cstdlib>
#include <cmath>

double dot_product(double* a, double* b, long long n) {
   double c[4];
    c[0] = 0;
    c[1] = 0;
    c[2] = 0;
    c[3] = 0;
    long long m = n/4;
    for (long long i = 0; i < 4*m; i += 4) {
        c[0] = std::fma(a[i], b[i], c[0]);
        c[1] = std::fma(a[i+1], b[i+1], c[1]);
        c[2] = std::fma(a[i+2], b[i+2], c[2]);
        c[3] = std::fma(a[i+3], b[i+3], c[3]);
    }
    double d = c[0] + c[1] + c[2] + c[3];
    for (long long i = 4*m; i < n; ++i) {
        d = std::fma(a[i], b[i], d);
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
    double scale = 1.0/n;
    for (size_t i = 0; i < n; ++i) {
        a[i] = 1.0*scale;
        b[i] = 1.0;
    }

    double d;
    for (int i = 0; i < 100; ++i) {
        asm volatile ("" ::: "memory");
        d = dot_product(a.data(), b.data(), n);
    }
    std::cout << "a.b = " << d << "\n";
}
