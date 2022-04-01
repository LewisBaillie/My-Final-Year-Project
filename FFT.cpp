#include "FFT.h"

void FFT::fft(std::vector<float>& samples)
{
	if (samples.size() <= 1)
	{
		return;
	}
	else
	{
		int halfSize = samples.size() / 2;
		if (halfSize % 2 != 0)
		{
			halfSize -= 0.5;
		}
		for (int i = 0; i < halfSize; i++)
		{
			//rearrange the array to frequency spectra instead of time domains by using bit reversal sort algorithm
			int pos = INT_SIZE - 1;

			int reverse = 0;

			while (pos >= 0 && i)
			{
				if (i & 1)
				{
					reverse = reverse | (1 << pos);
				}

				i >>= 1;
				pos--;
			}

			float temp = samples[i];
			samples[i] = samples[reverse];
			samples[reverse] = temp;
		}
	}
}