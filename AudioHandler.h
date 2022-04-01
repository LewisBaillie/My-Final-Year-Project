#pragma once
#include "AudioFile.h"
#include <iostream>
#include <vector>

class AudioHandler
{
public:
	AudioFile<double> audioObj;
	bool LoadFile();
private:
	bool mSuccess = false;
};