#ifndef _Audio_Handler_H
#define _Audio_Handler_H
#endif

#include "FFT.h"
#include "SpectralFluxAnalyser.h"
#include <iostream>
#include <vector>
#include <thread>
#include <fstream>

class AudioHandler
{
public:
	AudioHandler(int NumOfChannels, int SamplesPerChannel, int SampleRate, std::vector<double> Samples, std::string FileName);
	std::vector<double> CombineChannels();
	std::vector<double> ConvertToAmplitude(ComplexArray sampleArray);
	std::vector<float> beatMap;

	bool performBeatMapping();
	bool SaveBeatMap(std::vector<float> BeatMap);

	int bucketSize = 1024;

	std::vector<double> sendSpectrum;

private:

	int numOfChannels;
	int samplesPerChannel;
	int sampleRate;
	std::vector<double> samples;
	std::string fileName;

};