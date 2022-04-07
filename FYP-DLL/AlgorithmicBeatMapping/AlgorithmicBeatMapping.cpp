// AlgorithmicBeatMapping.cpp : Defines the exported functions for the DLL.
//

#include "pch.h"
#include "framework.h"

#include "AudioHandler.h"
#include <locale>
#include <codecvt>
#include "AlgorithmicBeatMapping.h"

std::string ConvertString(LPTSTR fileNameLPT)
{
	std::wstring wFileName = fileNameLPT;

	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;

	std::string converted = converter.to_bytes(wFileName);
	return converted;
}

bool MapFile(LPTSTR fileNameLPT, int sampleRate, int numberOfChannels, int samplesPerChannel, float* samplesArray, int size)
{
	std::vector<double> samples;

	for (int i = 0; i < size; i++)
	{
		samples.push_back(samplesArray[i]);
	}

	std::string fileName = ConvertString(fileNameLPT);
	AudioHandler handle(numberOfChannels, samplesPerChannel, sampleRate, samples, fileName);
	return handle.performBeatMapping(handle);
}

