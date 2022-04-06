#pragma once
#include "AudioFile.h"
#include <iostream>
#include <vector>

class AudioHandler
{
public:
	AudioFile<double> audioObj;
	bool LoadFile();
	std::vector<double> CombineChannels(AudioHandler handle);
	
	int bucketSize = 1024;
private:
	bool mSuccess = false;
	
};