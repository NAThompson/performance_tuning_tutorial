CXX = g++
CPPFLAGS = --std=c++11 -g -fno-omit-frame-pointer -O3 -march=native -fno-finite-math-only -ffast-math

all: dot

dot: src/mwe.cpp
	$(CXX) $(CPPFLAGS) $< -o $@

clean:
	rm -f dot perf.data perf.data.old
