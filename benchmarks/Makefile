CXX = g++-10
INCFLAGS = -I/usr/local/include
CPPFLAGS = --std=c++17 -g -fno-omit-frame-pointer -O3 -march=native -fno-finite-math-only -ffast-math
LINKFLAGS = -lbenchmark -pthread -L/usr/local/lib -ltbb

all: bench

bench: bench.cpp
	$(CXX) $(INCFLAGS) $(CPPFLAGS) $< -o $@ $(LINKFLAGS)

clean:
	rm -f bench perf.data perf.data.old
