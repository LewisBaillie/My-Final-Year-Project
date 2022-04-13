#include "SpectralFluxAnalyser.h"

SpectralFluxAnalyser::SpectralFluxAnalyser()
{
	indexToProcess = thresholdWindowSize / 2;
}

void SpectralFluxAnalyser::AnalyseSpectrum(std::vector<double> workingSamples, float time)
{
	previousSpectrum = currentSpectrum;
	currentSpectrum = workingSamples;
	//EXAMPLE of how to work out hz per bin
		//samplerate / nyquist frequency / size of spectrum to analyze
		//samplerate/2/handle.bucketsize = hz per bin
		//48000 / 2 = 24000 
		//24000 / 1024 = 23.47 hz per bin
		//Sub Bass: 20 to 60hz
		//Bass: 60 to 250hz
		//Low Midrange: 250 to 500hz
		//Midrange: 500 to 2000hz
		//Upper Midrange: 2000 to 4000hz
		//Presence: 4000hz to 6000hz
		//Brilliance: 6000hz to 20,000hz

	SpectralFluxInfo currentInfo;
	currentInfo.time = time;
	currentInfo.spectralFlux = CalculateFlux();
	//std::cout << currentInfo.spectralFlux << std::endl;

	FluxSamples.push_back(currentInfo);
	
	if (FluxSamples.size() >= thresholdWindowSize)
	{
		//Threshold of window surrounding sample
		FluxSamples[indexToProcess].threshold = CalculateThreshold(indexToProcess);

		//Only store amplitudes over threshold to allow peak filtering
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
		//not enough samples yet
	}

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

			//By checking if the difference is positive, it checks if the amplitude is rising
			float diff = currentSpectrum[i] - previousSpectrum[i];
			
			float larger = (0.f > diff) ? 0.f : diff;

			//std::cout << std::to_string(larger) << std::endl;
			sum += larger;
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