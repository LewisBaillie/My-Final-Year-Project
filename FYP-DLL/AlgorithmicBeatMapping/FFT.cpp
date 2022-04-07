#include "pch.h"
#include "FFT.h"


void FFT::fft(ComplexArray& samples)
{
	size_t N = samples.size();
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
	}
}

void FFT::window(ComplexArray& windowSamples)
{
	for (int i = 0; i < windowSamples.size(); i++)
	{
		windowSamples[i] = 0.5 - (0.5 * cos((2 * PI * i) / (windowSamples.size() - 1)));
	}
}