#ifndef _SPECTRAL_FLUX_ANALYSER_H
#define _SPECTRAL_FLUX_ANALYSER_H

#include <vector>
#include <complex>
#include <iostream>

class SpectralFluxInfo {
public:
	float time;
	float spectralFlux;
	float threshold;
	float prunedFlux;
	bool isPeak = false;
	int biggestIndex;
};

class SpectralFluxAnalyser
{
public:
	SpectralFluxAnalyser();
	void AnalyseSpectrum(std::vector<double> workingSamples, float time, int SampleRate);
	float CalculateFlux(SpectralFluxInfo& current);
	float CalculateThreshold(int index);
	float CalculatePrunedFlux(int index);
	bool CalculateIsPeak(int index);
	std::vector<SpectralFluxInfo> FluxSamples;
private:
	int sampleRate;
	float thresholdMultiplier = 1.5f;
	int thresholdWindowSize = 50;
	int indexToProcess;
	std::vector<double> previousSpectrum;
	std::vector<double> currentSpectrum;
};

#endif