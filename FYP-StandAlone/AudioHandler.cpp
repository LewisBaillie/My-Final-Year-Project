#include "AudioHandler.h"

std::vector<double> sendSpectrum;

bool AudioHandler::LoadFile()
{
	mSuccess = audioObj.load("60BPM.wav");
	if (mSuccess)
	{
		return mSuccess;
	}
}

bool AudioHandler::SaveBeatMap(std::vector<float> BeatMap)
{
	std::string fileString = "60BPM.txt";
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

	return usingSamples;
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
		normalizedAmp = sqrt(((sampleArray[i].real() * sampleArray[i].real()) + (sampleArray[i].imag() * sampleArray[i].imag())));
		newSamples.push_back(normalizedAmp);
	}

	return newSamples;
}

bool AudioHandler::performBeatMapping(AudioHandler handle)
{
	bool success = handle.LoadFile();

	SpectralFluxAnalyser sfa;

	std::vector<double> samples;

	std::vector<double> workingSpectrum;

	if (success)
	{
		FFT fftHandle;

		samples = handle.CombineChannels(handle);
		std::cout << std::to_string(samples.size()) << std::endl;
		std::cout << std::string("Starting FFT") << std::endl;

		int threads = 3;
		while (samples.size() > 0)
		{
			if (samples.size() > bucketSize * 3)
			{
				threads = 3;
			}

			//PADDING

			if (samples.size() < (bucketSize * 3))
			{
				if (samples.size() < (bucketSize * 2))
				{
					if (samples.size() < (bucketSize))
					{
						while (samples.size() < (bucketSize))
						{
							samples.push_back(0);
						}
						threads = 1;
					}
					else if (samples.size() > (bucketSize))
					{
						while (samples.size() < (bucketSize * 2))
						{
							samples.push_back(0);
						}
						threads = 2;
					}
				}
				else if (samples.size() > (bucketSize * 2))
				{
					while (samples.size() < (bucketSize * 3))
					{
						samples.push_back(0);
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
					workingSample = Complex(samples[i], 0);
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

				samples.erase(samples.begin(), samples.begin() + (bucketSize * 3));

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
					workingSample = Complex(samples[i], 0);
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

				samples.erase(samples.begin(), samples.begin() + (bucketSize * 2));

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
					workingSample = Complex(samples[i], 0);
					if (i < bucketSize)
					{
						sampleArray.push_back(workingSample);
					}
				}

				fftHandle.fft(sampleArray);
				//fftHandle.window(sampleArray);
				//fftHandle.window(sampleArray);

				samples.erase(samples.begin(), samples.begin() + (bucketSize));

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
		std::cout << "FFT DONE" << std::endl;

		//int index = 0;

		///*sendSpectrum.clear();
		//for (int i = 0; i < workingSpectrum.size(); i++)
		//{
		//	sendSpectrum.push_back(workingSpectrum[i]);
		//	index++;
		//}*/

		//while (workingSpectrum.size() > 0)
		//{
		//	sendSpectrum.clear();
		//	for (int i = 0; i < handle.bucketSize; i++)
		//	{
		//		sendSpectrum.push_back(workingSpectrum[i]);
		//	}
		//	index++;
		//	sfa.AnalyseSpectrum(sendSpectrum, ((1.0 / handle.audioObj.getSampleRate()) * index) * handle.bucketSize);
		//	workingSpectrum.erase(workingSpectrum.begin(), workingSpectrum.begin() + handle.bucketSize);
		//}

		float capacity = workingSpectrum.size() / handle.bucketSize;

		for (int i = 0; i < capacity; i++)
		{
			sendSpectrum.clear();
			for (int j = 0; j < handle.bucketSize; j++)
			{
				sendSpectrum.push_back(workingSpectrum[j]);
			}
			sfa.AnalyseSpectrum(sendSpectrum, (1.0f / handle.audioObj.getSampleRate())* i * handle.bucketSize);
			workingSpectrum.erase(workingSpectrum.begin(), workingSpectrum.begin() + handle.bucketSize);
		}

		std::cout << "sfa done" << std::endl;
		int counter = 0;
		float prevTime = 0.f;
		handle.beatMap.clear();
		for (int i = 0; i < sfa.FluxSamples.size(); i++)
		{
			if (i == 0)
			{
				if (sfa.FluxSamples[i].isPeak)
				{
					counter++;
					handle.beatMap.push_back(sfa.FluxSamples[i].time);
					prevTime = sfa.FluxSamples[i].time;
				}
			}
			else
			{
				if (sfa.FluxSamples[i].isPeak)
				{
					counter++;
					handle.beatMap.push_back(sfa.FluxSamples[i].time - prevTime);
					prevTime = sfa.FluxSamples[i].time;
				}
			}
		}

		std::cout << "Number of samples" + std::to_string(sfa.FluxSamples.size()) << std::endl;
		std::cout << "Number of beats" + std::to_string(handle.beatMap.size()) << std::endl;

		return SaveBeatMap(handle.beatMap);
	}
}


int main(int argc, char* argv[])
{
	AudioHandler audioHandle;
	if (audioHandle.performBeatMapping(audioHandle))
	{
		std::cout << "Beat Mapped" << std::endl;
	}
	
}