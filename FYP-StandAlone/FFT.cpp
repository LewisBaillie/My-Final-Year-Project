#include "FFT.h"

void FFT::fft(ComplexArray& samples)
{
	size_t N = samples.size();
	std::vector<Complex> evenSamples;
	std::vector<Complex> oddSamples;

	//Samples doesn't need to change if N==1
	if (N <= 1)
	{
		return;
	}

	//Decimate
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

	//Conquer
	//When they reach the right size (1 each) recursion will automatically stop as it returns at N<=1
	fft(evenSamples);
	fft(oddSamples);
	
	//Combine
	for (size_t k = 0; k < N / 2; ++k)
	{
		//Using polar and complex exponential to multiply by sinusoid of appropriate frequency
		Complex t = std::polar(1.0, -2 * PI * k / N) * oddSamples[k];

		//Applying butterfly structure
		samples[k] = evenSamples[k] + t;
		samples[k + (N / 2)] = evenSamples[k] - t;
	}
}

void FFT::window(ComplexArray& windowSamples)
{
	for (int i = 0; i < windowSamples.size(); i++)
	{
		windowSamples[i] = windowSamples[i] * (0.5 * (1 - cos((2*PI*(i / windowSamples.size())))));
	}
}