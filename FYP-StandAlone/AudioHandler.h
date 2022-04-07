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
	std::vector<double> ConvertToAmplitude(ComplexArray sampleArray);
	std::vector<float> beatMap;

	bool performBeatMapping(AudioHandler handle);

	int bucketSize = 1024;
private:
	bool mSuccess = false;

	/*TO REPLACE THE AUDIOFILE.H VALUES
	* double numOfChannels;
	* double samplesPerChannel;
	* double sampleRate;
	* std::vector<double> samples
	*/
	
};

#endif