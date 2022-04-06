#include "AudioHandler.h"
#include "FFT.h"

bool AudioHandler::LoadFile()
{
	mSuccess = audioObj.load("AMinorBird.wav");
	if (mSuccess)
	{
		audioObj.printSummary();
		return mSuccess;
	}
}

std::vector<double> AudioHandler::CombineChannels(AudioHandler handle)
{
	std::cout << handle.audioObj.getNumSamplesPerChannel() << std::endl;
	std::cout << handle.audioObj.getNumChannels() << std::endl;
	std::cout << (handle.audioObj.getNumSamplesPerChannel() * handle.audioObj.getNumChannels()) << std::endl;

	std::vector<double> usingSamples(handle.audioObj.getNumSamplesPerChannel());
	std::vector<std::vector<double>> workingSamples(handle.audioObj.getNumChannels());

	for (int x = 0; x < handle.audioObj.getNumChannels(); x++)
	{
		workingSamples[x] = std::vector<double>(handle.audioObj.getNumSamplesPerChannel());
	}

	for (int j = 0; j < handle.audioObj.getNumChannels(); j++) //j is channel
	{
		for (int i = 0; i < handle.audioObj.getNumSamplesPerChannel(); i++)
		{
			workingSamples[j][i] = handle.audioObj.samples[j][i];
		}
	}

	float sampleAverage = 0.0f;

	for (int s = 0; s < handle.audioObj.getNumSamplesPerChannel(); s++)
	{
		sampleAverage = 0.0f;
		for (int c = 0; c < handle.audioObj.getNumChannels(); c++)
		{
			sampleAverage += workingSamples[c][s];
		}
		sampleAverage = sampleAverage / handle.audioObj.getNumChannels();
		usingSamples[s] = sampleAverage;
	}

	std::cout << usingSamples.size() << std::endl;
	return usingSamples;
}

int main(int argc, char* argv[])
{
	/*char* dir = _getcwd(NULL, 0);
	printf("%s \nLength: %zu\n",dir,strlen(dir));*/

	std::cout << "\n" << std::endl;
	AudioHandler handle;
	bool success = handle.LoadFile();
	
	std::vector<double> samples;

	if (success)
	{
		FFT fftHandle;
		ComplexArray workingSpectrum;
		samples = handle.CombineChannels(handle);
		
		while (samples.size() > 0)
		{
			std::cout << samples.size() << std::endl;
			if (samples.size() < handle.bucketSize)
			{
				while (samples.size() < handle.bucketSize)
				{
					samples.push_back(0);
				}
			}

			std::complex<double> workingSample;
			ComplexArray sampleArray;
			for (int i = 0; i < handle.bucketSize; i++)
			{
				workingSample = Complex(samples[i]);
				sampleArray.push_back(workingSample);
			}
			fftHandle.fft(sampleArray);

			for (int j = 0; j < sampleArray.size(); j++)
			{
				//workingSpectrum will be the finalized FFT result for the whole song
				workingSpectrum.push_back(sampleArray[j]);
			}

			samples.erase(samples.begin(), samples.begin() + handle.bucketSize);
		}

		std::cout << "FFT COMPLETE" << std::endl;

	}
}