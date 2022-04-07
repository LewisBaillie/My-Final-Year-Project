#include "AudioHandler.h"

AudioHandler::AudioHandler(int NumOfChannels, int SamplesPerChannel, int SampleRate, std::vector<double> Samples, std::string FileName)
{
	numOfChannels = numOfChannels;
	samplesPerChannel = SamplesPerChannel;
	sampleRate = SampleRate;
	samples = Samples;
	fileName = FileName;
}

bool AudioHandler::SaveBeatMap(std::vector<float> BeatMap)
{
	std::string fileString = fileName + ".csv";
	std::ofstream mapFile(fileString);

	for (int i = 0; i < BeatMap.size(); i++)
	{
		mapFile << std::to_string(BeatMap[i]) + "\n";
	}

	mapFile.close();

	return true;
}

std::vector<double> AudioHandler::CombineChannels(AudioHandler handle)
{
	std::vector<double> usingSamples(handle.samplesPerChannel);
	std::vector<double> workingSamples(handle.numOfChannels);

	double average = 0.0;
	int index = 0;

	for (int i = 0; i < handle.numOfChannels * handle.samplesPerChannel; i++)
	{
		average += handle.samples[i];
		if ((i+1) % handle.numOfChannels == 0.0)
		{
			workingSamples[index] = (average / handle.numOfChannels);
			average = 0.0;
			index++;
		}
	}

	return workingSamples;
}

void AudioHandler::fft(FFT fftHandle, ComplexArray& sampleArray)
{
	fftHandle.fft(sampleArray);
	fftHandle.window(sampleArray);
}

std::vector<double> AudioHandler::ConvertToAmplitude(ComplexArray sampleArray)
{
	std::vector<double> newSamples;
	double normalizedAmp;
	for (int i = 0; i < sampleArray.size(); i++)
	{
		normalizedAmp = sqrt(((sampleArray[i].real() * sampleArray[i].real()) + (sampleArray[i].imag() * sampleArray[i].imag()))) / bucketSize;
		newSamples.push_back(normalizedAmp);
	}

	return newSamples;
}

bool AudioHandler::performBeatMapping(AudioHandler handle)
{

	SpectralFluxAnalyser sfa;

	std::vector<double> samples;

	void (*p_fft)(FFT, ComplexArray&);
	p_fft = handle.fft;

		FFT fftHandle;
		std::vector<double> workingSpectrum;
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

			std::complex<double> workingSample;
			ComplexArray sampleArray;
			ComplexArray sampleArray2;
			ComplexArray sampleArray3;

			std::vector<double> arrayToAdd;

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
					else if (i >= handle.bucketSize * 2 && i < handle.bucketSize * 3)
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


				arrayToAdd = handle.ConvertToAmplitude(sampleArray);
				for (int j = 0; j < sampleArray.size(); j++)
				{
					workingSpectrum.push_back(arrayToAdd[j]);
				}
				arrayToAdd = handle.ConvertToAmplitude(sampleArray2);
				for (int j = 0; j < sampleArray.size(); j++)
				{
					workingSpectrum.push_back(arrayToAdd[j]);
				}
				arrayToAdd = handle.ConvertToAmplitude(sampleArray3);
				for (int j = 0; j < sampleArray.size(); j++)
				{
					workingSpectrum.push_back(arrayToAdd[j]);
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

				arrayToAdd = handle.ConvertToAmplitude(sampleArray);
				for (int j = 0; j < sampleArray.size(); j++)
				{
					workingSpectrum.push_back(arrayToAdd[j]);
				}
				arrayToAdd = handle.ConvertToAmplitude(sampleArray2);
				for (int j = 0; j < sampleArray.size(); j++)
				{
					workingSpectrum.push_back(arrayToAdd[j]);
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

				arrayToAdd = handle.ConvertToAmplitude(sampleArray);
				for (int j = 0; j < sampleArray.size(); j++)
				{
					workingSpectrum.push_back(arrayToAdd[j]);
				}
			}
		}

		std::cout << "FFT COMPLETE" << std::endl;
		int index = 0;

		while (workingSpectrum.size() > 0)
		{
			std::vector<double> sendSpectrum;
			for (int i = 0; i < handle.bucketSize; i++)
			{
				sendSpectrum.push_back(workingSpectrum[i]);
				index++;
			}
			workingSpectrum.erase(workingSpectrum.begin(), workingSpectrum.begin() + handle.bucketSize);
			sfa.AnalyseSpectrum(sendSpectrum, handle.sampleRate, ((1.f / handle.sampleRate) * index) * handle.bucketSize);
		}

		std::cout << "sfa done" << std::endl;

		for (int i = 0; i < sfa.FluxSamples.size(); i++)
		{
			if (sfa.FluxSamples[i].isPeak)
			{
				if (handle.beatMap.empty())
				{
					handle.beatMap.push_back(sfa.FluxSamples[i].time);
				}
				else
				{
					handle.beatMap.push_back(sfa.FluxSamples[i].time - handle.beatMap[handle.beatMap.size() - 1]);
				}
			}
		}



		return SaveBeatMap(handle.beatMap);
	
}