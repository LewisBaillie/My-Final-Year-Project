#pragma once
#include <vector>
#include <complex>

#define PI 3.14159265358979323846;
#define INT_SIZE sizeof(int) * 8;

class FFT
{
public:
	void fft(std::vector<float>& samples);
private:
};