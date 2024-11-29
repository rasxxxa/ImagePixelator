#pragma once
#include "RawImageHandler.h"
#include <vector>
#include <algorithm>
#include <utility>
#include <array>

class ResolutionChanger
{
public:
	static Image ChangeResolution(const Image& original, unsigned int newWidth, unsigned int newHeight);
};

