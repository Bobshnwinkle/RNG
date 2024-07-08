#pragma once
#include <vector>
#include <iostream>
#include <ctime>
#include <cmath>
#include <intrin.h>

using namespace std;
class fstRand {

public:
	fstRand() {
		seed = Next();
	}
	fstRand(float _seed) {
		seed = _seed;
	}

	float Next() {
		float out = distrobution((rdtsc() + seed), 3333 * ( 1.0f + (0.5 - seed)));
		seed = out;
		return out;
	}
private:
	float seed = 0.287124634;
#ifdef _WIN32
	static uint64_t rdtsc() {
		auto val = __rdtsc();
		return val % 10000;
	}
#else
	static uint64_t rdtsc() {
		unsigned int lo, hi;
		__asm__ __volatile__("rdtsc" : "=a" (lo), "=d" (hi));
		auto val = ((uint64_t)hi << 32) | lo;
		return val % 10000;
}
#endif

	static float distrobution(float x, float mult = 3333) {
		float comp = (x * mult) / 10000;
		float out = comp - (truncf(comp));
		return out;
	}
};