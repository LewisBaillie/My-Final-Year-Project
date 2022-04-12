// AlgorithmicBeatMapping.cpp : Defines the exported functions for the DLL.
//

#include "pch.h"
#include "framework.h"

#include "AudioHandler.h"
#include <locale>
#include <codecvt>
#include "AlgorithmicBeatMapping.h"

std::vector<double> samples;

ALGORITHMICBEATMAPPING_API bool MapFile(const char* fileNameChar, int sampleRate, int numberOfChannels, int samplesPerChannel)
{
	std::string fileName(fileNameChar);
	AudioHandler handle(numberOfChannels, samplesPerChannel, sampleRate, samples, fileName);
	return handle.performBeatMapping();
}

ALGORITHMICBEATMAPPING_API bool PassArray(float* SamplesArray, int Size)
{

	for (int i = 0; i < Size; i++)
	{
		samples.push_back(static_cast<double>(*(SamplesArray + i)));
	}

	return samples.size() > 0;
}

