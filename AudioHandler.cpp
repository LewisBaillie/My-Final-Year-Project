#include "AudioHandler.h"


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

void AudioHandler::fft(FFT fftHandle, ComplexArray& sampleArray)
{
	fftHandle.fft(sampleArray);
}

int main(int argc, char* argv[])
{
	/*char* dir = _getcwd(NULL, 0);
	printf("%s \nLength: %zu\n",dir,strlen(dir));*/

	

	std::cout << "\n" << std::endl;
	AudioHandler handle;
	bool success = handle.LoadFile();
	
	std::vector<double> samples;

	void (*p_fft)(FFT, ComplexArray&);
	p_fft = handle.fft;

	if (success)
	{
		FFT fftHandle;
		ComplexArray workingSpectrum;
		samples = handle.CombineChannels(handle);
		
		int threads;
		while (samples.size() > 0)
		{
			if (samples.size() > handle.bucketSize * 3)
			{
				threads = 3;
			}

			if (samples.size() < (handle.bucketSize * 3))
			{
				if (samples.size() < (handle.bucketSize * 2))
				{
					if (samples.size() < (handle.bucketSize))
					{
						while (samples.size() < (handle.bucketSize))
						{
							samples.push_back(0);

						}
						threads = 1;
					}
					else if (samples.size() > (handle.bucketSize))
					{
						while (samples.size() < (handle.bucketSize * 2))
						{
							samples.push_back(0);
						}
						threads = 2;
					}
				}
				else if (samples.size() > (handle.bucketSize * 2))
				{
					while (samples.size() < (handle.bucketSize * 3))
					{
						samples.push_back(0);
					}
					threads = 3;
				}
			}
			std::cout << threads << std::endl;

			std::complex<double> workingSample;
			ComplexArray sampleArray;
			ComplexArray sampleArray2;
			ComplexArray sampleArray3;

			if (threads == 3)
			{
				for (int i = 0; i < handle.bucketSize * 3; i++)
				{
					workingSample = Complex(samples[i]);
					if (i < handle.bucketSize)
					{
						sampleArray.push_back(workingSample);
					}
					else if (i >= handle.bucketSize && i < handle.bucketSize * 2)
					{
						sampleArray2.push_back(workingSample);
					}
					else if (i >= handle.bucketSize*2 && i < handle.bucketSize * 3)
					{
						sampleArray3.push_back(workingSample);
					}
				}

				std::thread th1(p_fft, fftHandle, std::ref(sampleArray));
				std::thread th2(p_fft, fftHandle, std::ref(sampleArray2));
				std::thread th3(p_fft, fftHandle, std::ref(sampleArray3));
				samples.erase(samples.begin(), samples.begin() + handle.bucketSize * 3);

				th1.join();
				th2.join();
				th3.join();

				for (int j = 0; j < sampleArray.size(); j++)
				{
					workingSpectrum.push_back(sampleArray[j]);
				}
				for (int j = 0; j < sampleArray2.size(); j++)
				{
					workingSpectrum.push_back(sampleArray2[j]);
				}
				for (int j = 0; j < sampleArray3.size(); j++)
				{
					workingSpectrum.push_back(sampleArray3[j]);
				}
			}
			else if (threads == 2)
			{
				for (int i = 0; i < handle.bucketSize * 2; i++)
				{
					workingSample = Complex(samples[i]);
					if (i < handle.bucketSize)
					{
						sampleArray.push_back(workingSample);
					}
					else if (i >= handle.bucketSize && i < handle.bucketSize * 2)
					{
						sampleArray2.push_back(workingSample);
					}
				}
				std::thread th1(p_fft, fftHandle, std::ref(sampleArray));
				std::thread th2(p_fft, fftHandle, std::ref(sampleArray2));
				samples.erase(samples.begin(), samples.begin() + handle.bucketSize * 2);

				th1.join();
				th2.join();

				for (int j = 0; j < sampleArray.size(); j++)
				{
					workingSpectrum.push_back(sampleArray[j]);
				}
				for (int j = 0; j < sampleArray2.size(); j++)
				{
					workingSpectrum.push_back(sampleArray2[j]);
				}
			}
			else
			{
				for (int i = 0; i < handle.bucketSize; i++)
				{
					workingSample = Complex(samples[i]);
					if (i < handle.bucketSize)
					{
						sampleArray.push_back(workingSample);
					}
					
				}
				std::thread th1(p_fft, fftHandle, std::ref(sampleArray));
				samples.erase(samples.begin(), samples.begin() + handle.bucketSize);

				th1.join();

				for (int j = 0; j < sampleArray.size(); j++)
				{
					//workingSpectrum will be the finalized FFT result for the whole song
					workingSpectrum.push_back(sampleArray[j]);
				}
			}

			
			for (int j = 0; j < sampleArray.size(); j++)
			{
				//workingSpectrum will be the finalized FFT result for the whole song
				workingSpectrum.push_back(sampleArray[j]);
			}
			


		}

		std::cout << "FFT COMPLETE" << std::endl;

	}
}