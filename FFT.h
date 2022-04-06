#pragma once
#include <vector>
#include <complex>

#define PI 3.14159265358979323846

typedef std::complex<double> Complex;
typedef std::vector<Complex> ComplexArray;

class FFT
{
public:
	void fft(ComplexArray& samples);
private:
};