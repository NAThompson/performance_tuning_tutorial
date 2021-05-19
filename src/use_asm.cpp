#include <iostream>
#include <cstdlib>

extern "C" double dot(double* a, double* b, long long n);

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: ./dot 10\n";
        return 1;
    }
    size_t n = atoi(argv[1]);

    double* a = (double*) malloc(n*sizeof(double));
    double* b = (double*) malloc(n*sizeof(double));
    for (size_t i = 0; i < n; ++i) {
        a[i] = i;
        b[i] = 1.0/n;
    }

    double d;
    for (int i = 0; i < 100; ++i) {
	asm volatile ("" ::: "memory");
        d = dot(a, b, n);
    }
    std::cout << "a.b = " << d << "\n";
}
