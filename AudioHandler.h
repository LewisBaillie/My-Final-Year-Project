#pragma once
#include "AudioFile.h"
#include "FFT.h"
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
	

	int bucketSize = 1024;
private:
	bool mSuccess = false;
	
};