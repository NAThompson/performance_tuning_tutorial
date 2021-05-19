CXX = g++
CPPFLAGS = --std=c++17 -g -fno-omit-frame-pointer -O3 -march=native -fno-finite-math-only -ffast-math -fopenmp

all: dot

dot: src/mwe.cpp
	$(CXX) $(CPPFLAGS) $< -o $@

use_asm.x: src/use_asm.cpp
	yasm -f elf64 -g dwarf2 src/dot.asm -o dot.o
	$(CXX) -o $@ dot.o $<

clean:
	rm -f dot *.o perf.data perf.data.old a.out src/*.o src/*.x src/perf.data src/perf.data.old
