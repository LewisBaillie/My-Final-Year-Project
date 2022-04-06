#ifndef _Audio_Handler_H
#define _Audio_Handler_H

#include "AudioFile.h"
#include "FFT.h"
#include "SpectralFluxAnalyser.h"
#include <iostream>
#include <vector>
#include <thread>

class AudioHandler
{
public:
	AudioFile<double> audioObj;
	bool LoadFile();
	std::vector<double> CombineChannels(AudioHandler handle);
	static void fft(FFT fftHandle, ComplexArray& sampleArray);
	
	int bucketSize = 2048;
private:
	bool mSuccess = false;
	
};

#endif