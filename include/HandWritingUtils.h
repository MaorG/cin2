#pragma once

#include <string.h>
#include <sstream>

class HandWritingUtils
{
public:
	HandWritingUtils();
	~HandWritingUtils();

	static float normalizeValue(float value, float oldMin, float oldMax, float newMin, float newMax);
	static void printResultMatrix(std::string name, int resultMatrix[10][10]);
};