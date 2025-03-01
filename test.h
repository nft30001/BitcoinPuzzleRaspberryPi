#pragma once

#include <string>

struct Timer
{
	static unsigned long long time();
	
	unsigned long long timer;
	
	Timer();
	unsigned long long stop(int iterations);
	double stop();
};

bool check(unsigned char* array1, unsigned char* array2, int length);
int round(double number, int digits);
int test();
