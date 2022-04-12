// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the ALGORITHMICBEATMAPPING_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// ALGORITHMICBEATMAPPING_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef ALGORITHMICBEATMAPPING_EXPORTS
#define ALGORITHMICBEATMAPPING_API __declspec(dllexport)
#else
#define ALGORITHMICBEATMAPPING_API __declspec(dllimport)
#endif

#include <iostream>
#include <vector>

extern "C"
{
	ALGORITHMICBEATMAPPING_API bool MapFile(const char* fileNameLPT, int sampleRate, int numberOfChannels, int samplesPerChannel);
	ALGORITHMICBEATMAPPING_API bool PassArray(float* SamplesArray, int Size);
}

extern ALGORITHMICBEATMAPPING_API int nAlgorithmicBeatMapping;

ALGORITHMICBEATMAPPING_API int fnAlgorithmicBeatMapping(void);

