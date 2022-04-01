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

int main(int argc, char* argv[])
{
	/*char* dir = _getcwd(NULL, 0);
	printf("%s \nLength: %zu\n",dir,strlen(dir));*/

	std::cout << "\n" << std::endl;
	AudioHandler handle;
	bool success = handle.LoadFile();

	if (success)
	{
		std::cout << handle.audioObj.getNumSamplesPerChannel()<< std::endl;
		std::cout << handle.audioObj.getNumChannels() << std::endl;
		std::cout << (handle.audioObj.getNumSamplesPerChannel() * handle.audioObj.getNumChannels()) << std::endl;

		std::vector<float> usingSamples (handle.audioObj.getNumSamplesPerChannel());
		std::vector<std::vector<float>> workingSamples (handle.audioObj.getNumChannels());

		for (int x = 0; x < handle.audioObj.getNumChannels(); x++)
		{
			workingSamples[x] = std::vector<float>(handle.audioObj.getNumSamplesPerChannel());
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
	}
}