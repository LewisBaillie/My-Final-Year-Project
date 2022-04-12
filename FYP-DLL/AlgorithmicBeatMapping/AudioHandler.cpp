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
		mapFile << std::to_string(BeatMap[i] / 1000.f) + "\n";
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

bool AudioHandler::performBeatMapping()
{

	SpectralFluxAnalyser sfa;

	std::vector<double> workSamples;

	FFT fftHandle;
	std::vector<double> workingSpectrum;
	workSamples = CombineChannels();

	int threads = 3;
	while (workSamples.size() > 0)
	{
		if (workSamples.size() > bucketSize * 3)
		{
			threads = 3;
		}

		if (workSamples.size() < (bucketSize * 3))
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
				}
				else if (workSamples.size() > (bucketSize))
				{
					while (workSamples.size() < (bucketSize * 2))
					{
						workSamples.push_back(0);
					}
					threads = 2;
				}
			}
			else if (workSamples.size() > (bucketSize * 2))
			{
				while (workSamples.size() < (bucketSize * 3))
				{
					workSamples.push_back(0);
				}
				threads = 3;
			}
		}

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
			//fftHandle.window(sampleArray);
			fftHandle.fft(sampleArray2);
			//fftHandle.window(sampleArray2);
			fftHandle.fft(sampleArray3);
			//fftHandle.window(sampleArray3);

			workSamples.erase(workSamples.begin(), workSamples.begin() + (bucketSize * 3));

			/*std::thread th1(p_fft, fftHandle, sampleArray);
			std::thread th2(p_fft, fftHandle, sampleArray2);
			std::thread th3(p_fft, fftHandle, sampleArray3);


			th1.join();
			th2.join();
			th3.join();*/

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
			//fftHandle.window(sampleArray);
			fftHandle.fft(sampleArray2);
			//fftHandle.window(sampleArray2);

			workSamples.erase(workSamples.begin(), workSamples.begin() + (bucketSize * 2));

			/*std::thread th1(p_fft, fftHandle, std::ref(sampleArray));
			std::thread th2(p_fft, fftHandle, std::ref(sampleArray2));
			samples.erase(samples.begin(), samples.begin() + handle.bucketSize * 2);

			th1.join();
			th2.join();*/

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
			//fftHandle.window(sampleArray);

			workSamples.erase(workSamples.begin(), workSamples.begin() + (bucketSize));

			/*std::thread th1(p_fft, fftHandle, std::ref(sampleArray));
			samples.erase(samples.begin(), samples.begin() + handle.bucketSize);

			th1.join();*/

			arrayToAdd.clear();
			arrayToAdd = ConvertToAmplitude(sampleArray);
			for (int j = 0; j < arrayToAdd.size(); j++)
			{
				workingSpectrum.push_back(arrayToAdd[j]);
			}
		}
	}


	int index = 0;

	while (workingSpectrum.size() > 0)
	{
		sendSpectrum.clear();
		for (int i = 0; i < bucketSize; i++)
		{
			sendSpectrum.push_back(workingSpectrum[i]);
			index++;
		}
		sfa.AnalyseSpectrum(sendSpectrum, ((1.0 / sampleRate) * index) * bucketSize);
		workingSpectrum.erase(workingSpectrum.begin(), workingSpectrum.begin() + bucketSize);
	}

	std::cout << "sfa done" << std::endl;

	for (int i = 0; i < sfa.FluxSamples.size(); i++)
	{
		if (i == 0)
		{
			if (sfa.FluxSamples[i].isPeak)
			{
				beatMap.push_back(sfa.FluxSamples[i].time);
			}
		}
		else
		{
			if (sfa.FluxSamples[i].isPeak)
			{
				beatMap.push_back(sfa.FluxSamples[i].time - sfa.FluxSamples[i - 1].time);
			}
		}
	}

	std::cout << beatMap.size() << std::endl;
	return SaveBeatMap(beatMap);
}
	
