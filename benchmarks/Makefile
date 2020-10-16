CXX = g++-10
INCFLAGS = -I/usr/local/include
CPPFLAGS = -fopenmp --std=c++11 -g -fno-omit-frame-pointer -O3 -march=native -fno-finite-math-only -ffast-math
LINKFLAGS = -lbenchmark -pthread -L/usr/local/lib

all: dot_bench

dot_bench: bench.cpp
	$(CXX) $(INCFLAGS) $(CPPFLAGS) $< -o $@ $(LINKFLAGS)

clean:
	rm -f dot_bench perf.data perf.data.old
