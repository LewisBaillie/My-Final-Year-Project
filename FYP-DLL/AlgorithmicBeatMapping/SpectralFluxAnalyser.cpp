#include "pch.h"
#include "SpectralFluxAnalyser.h"

std::vector<double> previousSpectrum;
std::vector<double> currentSpectrum;

SpectralFluxAnalyser::SpectralFluxAnalyser()
{
	indexToProcess = thresholdWindowSize / 2;
}

void SpectralFluxAnalyser::AnalyseSpectrum(std::vector<double> workingSamples, int sampleRate, float time)
{
	currentSpectrum = workingSamples;

	double hertzPerSample = sampleRate / 2.0 / workingSamples.size();

	SpectralFluxInfo* currentInfo = new SpectralFluxInfo();
	currentInfo->time = time;
	currentInfo->spectralFlux = CalculateFlux();

	FluxSamples.push_back(*currentInfo);

	if (FluxSamples.size() >= thresholdWindowSize)
	{
		FluxSamples[indexToProcess].threshold = CalculateThreshold(indexToProcess);

		FluxSamples[indexToProcess].prunedFlux = CalculatePrunedFlux(indexToProcess);

		int lowerIndex = indexToProcess - 1;

		bool currentPeak = CalculateIsPeak(lowerIndex);

		if (currentPeak)
		{
			FluxSamples[lowerIndex].isPeak = true;
		}
		indexToProcess++;
	}
	else
	{
		
	}
	previousSpectrum = workingSamples;
}

float SpectralFluxAnalyser::CalculateFlux()
{
	float sum = 0.f;


	for (int i = 0; i < currentSpectrum.size(); i++)
	{
		if (previousSpectrum.empty())
		{
			sum += currentSpectrum[i];
		}
		else
		{
			float diff = currentSpectrum[i] - previousSpectrum[i];
			float larger = (0.f > diff) ? 0.f : diff;
		}
	}

	return sum;
}

float SpectralFluxAnalyser::CalculateThreshold(int index)
{
	int windowStart = (0 > index - thresholdWindowSize / 2) ? 0 : index - thresholdWindowSize / 2;
	int windowEnd = (FluxSamples.size() - 1 < index + thresholdWindowSize / 2) ? FluxSamples.size() - 1 : index + thresholdWindowSize / 2;

	float sum = 0.f;
	for (int i = windowStart; i < windowEnd; i++)
	{
		sum += FluxSamples[i].spectralFlux;
	}

	float average = sum / (windowEnd - windowStart);
	return average * thresholdMultiplier;

}

float SpectralFluxAnalyser::CalculatePrunedFlux(int index)
{
	float bigger = (0.f > FluxSamples[index].spectralFlux - FluxSamples[index].threshold) ? 0.f : FluxSamples[index].spectralFlux - FluxSamples[index].threshold;
	return bigger;
}

bool SpectralFluxAnalyser::CalculateIsPeak(int index)
{
	if (FluxSamples[index].prunedFlux > FluxSamples[index + 1].prunedFlux && FluxSamples[index].prunedFlux > FluxSamples[index - 1].prunedFlux)
	{
		return true;
	}
	else
	{
		return false;
	}
}