#include "pch.h"
#include "AudioHandler.h"

AudioHandler::AudioHandler(int NumOfChannels, int SamplesPerChannel, int SampleRate, std::vector<double> Samples, std::string FileName)
{
	numOfChannels = NumOfChannels;
	samplesPerChannel = SamplesPerChannel;
	sampleRate = SampleRate;
	samples = Samples;
	fileName = FileName + ".txt";
}

bool AudioHandler::SaveBeatMap(std::vector<float> BeatMap)
{
	std::string fileString = fileName;
	std::ofstream mapFile(fileString);

	for (int i = 0; i < BeatMap.size(); i++)
	{
		mapFile << std::to_string(BeatMap[i]) + "\n";
	}

	mapFile.close();

	return true;
}

std::vector<double> AudioHandler::CombineChannels()
{
	std::vector<double> workingSamples(samplesPerChannel);

	double average = 0.0;
	int index = 0;

	for (int i = 0; i < numOfChannels * samplesPerChannel; i++)
	{
		average += samples[i];
		if ((i+1) % numOfChannels == 0.0)
		{
			workingSamples[index] = (average / numOfChannels);
			average = 0.0;
			index++;
		}
	}
	return workingSamples;
}

std::vector<double> AudioHandler::ConvertToAmplitude(ComplexArray sampleArray)
{
	std::vector<double> newSamples;
	double normalizedAmp;
	for (int i = 0; i < sampleArray.size(); i++)
	{
		normalizedAmp = sqrt(((sampleArray[i].real() * sampleArray[i].real()) + (sampleArray[i].imag() * sampleArray[i].imag())));
		newSamples.push_back(normalizedAmp);
	}

	return newSamples;
}

bool AudioHandler::performBeatMapping()
{

	SpectralFluxAnalyser sfa;

	std::vector<double> workSamples;
	//workSamples = samples;

	FFT fftHandle;
	std::vector<double> workingSpectrum;
	workSamples = CombineChannels();

	bool threadAlloc = false;
	int threads = 3;
	while (workSamples.size() > 0)
	{
		if (workSamples.size() > bucketSize * 3 && !threadAlloc)
		{
			threads = 3;
			threadAlloc = true;
		}

		if (workSamples.size() == bucketSize * 2 && !threadAlloc)
		{
			threads = 2;
			threadAlloc = true;
		}

		if (workSamples.size() == bucketSize && !threadAlloc)
		{
			threads = 1;
			threadAlloc = true;
		}

		//PADDING

		if (workSamples.size() < (bucketSize * 3) && !threadAlloc)
		{
			if (workSamples.size() < (bucketSize * 2))
			{
				if (workSamples.size() < (bucketSize))
				{
					while (workSamples.size() < (bucketSize))
					{
						workSamples.push_back(0);
					}
					threads = 1;
					threadAlloc = true;
				}
				else if (workSamples.size() > (bucketSize) && !threadAlloc)
				{
					while (workSamples.size() < (bucketSize * 2))
					{
						workSamples.push_back(0);
					}
					threads = 2;
					threadAlloc = true;
				}
			}
			else if (workSamples.size() > (bucketSize * 2) && !threadAlloc)
			{
				while (workSamples.size() < (bucketSize * 3))
				{
					workSamples.push_back(0);
				}
				threads = 3;
				threadAlloc = true;
			}
		}

		threadAlloc = false;

		Complex workingSample;
		ComplexArray sampleArray;
		ComplexArray sampleArray2;
		ComplexArray sampleArray3;

		std::vector<double> arrayToAdd;

		if (threads == 3)
		{
			for (int i = 0; i < bucketSize * 3; i++)
			{
				workingSample = Complex(workSamples[i], 0);
				if (i < bucketSize)
				{
					sampleArray.push_back(workingSample);
				}
				else if (i >= bucketSize && i < bucketSize * 2)
				{
					sampleArray2.push_back(workingSample);
				}
				else if (i >= bucketSize * 2 && i < bucketSize * 3)
				{
					sampleArray3.push_back(workingSample);
				}
			}

			fftHandle.fft(sampleArray);
			fftHandle.fft(sampleArray2);
			fftHandle.fft(sampleArray3);

			workSamples.erase(workSamples.begin(), workSamples.begin() + (bucketSize * 3));

			arrayToAdd.clear();
			arrayToAdd = ConvertToAmplitude(sampleArray);
			for (int j = 0; j < arrayToAdd.size(); j++)
			{
				workingSpectrum.push_back(arrayToAdd[j]);
			}
			arrayToAdd.clear();
			arrayToAdd = ConvertToAmplitude(sampleArray2);
			for (int k = 0; k < arrayToAdd.size(); k++)
			{
				workingSpectrum.push_back(arrayToAdd[k]);
			}
			arrayToAdd.clear();
			arrayToAdd = ConvertToAmplitude(sampleArray3);
			for (int l = 0; l < arrayToAdd.size(); l++)
			{
				workingSpectrum.push_back(arrayToAdd[l]);
			}
			sampleArray.clear();
			sampleArray2.clear();
			sampleArray3.clear();
		}
		else if (threads == 2)
		{
			for (int i = 0; i < bucketSize * 2; i++)
			{
				workingSample = Complex(workSamples[i], 0);
				if (i < bucketSize)
				{
					sampleArray.push_back(workingSample);
				}
				else if (i >= bucketSize && i < bucketSize * 2)
				{
					sampleArray2.push_back(workingSample);
				}
			}

			fftHandle.fft(sampleArray);
			fftHandle.fft(sampleArray2);

			workSamples.erase(workSamples.begin(), workSamples.begin() + (bucketSize * 2));

			arrayToAdd.clear();
			arrayToAdd = ConvertToAmplitude(sampleArray);
			for (int j = 0; j < arrayToAdd.size(); j++)
			{
				workingSpectrum.push_back(arrayToAdd[j]);
			}
			arrayToAdd.clear();
			arrayToAdd = ConvertToAmplitude(sampleArray2);
			for (int k = 0; k < arrayToAdd.size(); k++)
			{
				workingSpectrum.push_back(arrayToAdd[k]);
			}

			sampleArray.clear();
			sampleArray2.clear();
		}
		else
		{
			for (int i = 0; i < bucketSize; i++)
			{
				workingSample = Complex(workSamples[i], 0);
				if (i < bucketSize)
				{
					sampleArray.push_back(workingSample);
				}
			}

			fftHandle.fft(sampleArray);

			workSamples.erase(workSamples.begin(), workSamples.begin() + (bucketSize));

			arrayToAdd.clear();
			arrayToAdd = ConvertToAmplitude(sampleArray);
			for (int j = 0; j < arrayToAdd.size(); j++)
			{
				workingSpectrum.push_back(arrayToAdd[j]);
			}
			sampleArray.clear();
		}
	}

	float capacity = workingSpectrum.size() / bucketSize;

	for (int i = 0; i < capacity; i++)
	{
		sendSpectrum.clear();
		for (int j = 0; j < bucketSize; j++)
		{
			sendSpectrum.push_back(workingSpectrum[j]);
		}
		sfa.AnalyseSpectrum(sendSpectrum, (1.0f / sampleRate) * i * bucketSize, sampleRate);
		workingSpectrum.erase(workingSpectrum.begin(), workingSpectrum.begin() + bucketSize);
	}

	float prevTime = 0.f;

	for (int i = 0; i < sfa.FluxSamples.size(); i++)
	{
		if (i == 0)
		{
			if (sfa.FluxSamples[i].isPeak)
			{
				
				beatMap.push_back(sfa.FluxSamples[i].time);
				prevTime = sfa.FluxSamples[i].time;
			}
		}
		else
		{
			if (sfa.FluxSamples[i].isPeak)
			{

				beatMap.push_back(sfa.FluxSamples[i].time - prevTime);
				prevTime = sfa.FluxSamples[i].time;
			}
		}
	}

	
	std::cout << beatMap.size() << std::endl;
	return SaveBeatMap(beatMap);
}
	
