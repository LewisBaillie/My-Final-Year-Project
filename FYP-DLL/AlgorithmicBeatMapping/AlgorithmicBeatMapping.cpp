// AlgorithmicBeatMapping.cpp : Defines the exported functions for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "AlgorithmicBeatMapping.h"
#include "AudioHandler.h"


// This is an example of an exported variable
ALGORITHMICBEATMAPPING_API int nAlgorithmicBeatMapping=0;

bool MapFile(std::string fileName, int sampleRate, int numberOfChannels, int samplesPerChannel, std::vector<double> samples)
{
	AudioHandler handle(numberOfChannels, samplesPerChannel, sampleRate, samples, fileName);
	return handle.performBeatMapping(handle);
}