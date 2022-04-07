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
	bool isPeak;
};

class SpectralFluxAnalyser
{
public:
	SpectralFluxAnalyser();
	void AnalyseSpectrum(std::vector<double> workingSamples, int sampleRate, float time);
	float CalculateFlux();
	float CalculateThreshold(int index);
	float CalculatePrunedFlux(int index);
	bool CalculateIsPeak(int index);
	std::vector<SpectralFluxInfo> FluxSamples;
private:
	float thresholdMultiplier = 1.25f;
	int thresholdWindowSize = 40;
	int indexToProcess;
};

#endif