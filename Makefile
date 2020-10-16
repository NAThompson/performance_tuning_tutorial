CXX = g++
CPPFLAGS = --std=c++11 -g -fno-omit-frame-pointer -O3 -march=native -fno-finite-math-only

all: dot

dot: src/mwe.cpp
	$(CXX) $< -o $@
