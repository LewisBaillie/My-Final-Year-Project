#include "pch.h"
#include "FFT.h"


void FFT::fft(ComplexArray& samples)
{
	unsigned int N = samples.size(), k = N, n;
	double thetaT = PI / N;
	Complex phiT = Complex(cos(thetaT), -sin(thetaT)), T;

	while (k > 1)
	{
		n = k;
		k >>= 1;
		phiT = phiT * phiT;
		T = 1.0L;

		for (unsigned int l = 0; l < k; l++)
		{
			for (unsigned int a = l; a < N; a += n)
			{
				unsigned int b = a + k;
				Complex t = samples[a] - samples[b];
				samples[a] += samples[b];
				samples[b] = t * T;
			}
			T *= phiT;
		}
	}

	unsigned int m = (unsigned int)log2(N);

	for (unsigned int a = 0; a < N; a++)
	{
		unsigned int b = a;
		b = (((b & 0xaaaaaaaa) >> 1) | ((b & 0x55555555) << 1));
		b = (((b & 0xcccccccc) >> 2) | ((b & 0x33333333) << 2));
		b = (((b & 0xf0f0f0f0) >> 4) | ((b & 0x0f0f0f0f) << 4));
		b = (((b & 0xff00ff00) >> 8) | ((b & 0x00ff00ff) << 8));
		b = ((b >> 16) | (b << 16)) >> (32 - m);

		if (b > a)
		{
			Complex t = samples[a];
			samples[a] = samples[b];
			samples[b] = t;
		}
	}






	/*size_t N = samples.size();
	std::vector<Complex> evenSamples;
	std::vector<Complex> oddSamples;

	if (N <= 1)
	{
		return;
	}

	else
	{
		for (int i = 0; i < N; i++)
		{
			if (i % 2 == 0)
			{
				evenSamples.push_back(samples[i]);
			}
			else
			{
				oddSamples.push_back(samples[i]);
			}
		}
	}

	fft(evenSamples);
	fft(oddSamples);

	for (size_t k = 0; k < N / 2; ++k)
	{
		Complex t = std::polar(1.0, -2 * PI * k / N) * oddSamples[k];

		samples[k] = evenSamples[k] + t;
		samples[k + N / 2] = evenSamples[k] - t;
	}*/
}

void FFT::window(ComplexArray& windowSamples)
{
	for (int i = 0; i < windowSamples.size(); i++)
	{
		windowSamples[i] = 0.5 - (0.5 * cos((2 * PI * i) / (windowSamples.size() - 1)));
	}
}