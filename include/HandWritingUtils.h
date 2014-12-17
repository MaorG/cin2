#pragma once

class HandWritingUtils
{
public:
	HandWritingUtils();
	~HandWritingUtils();

	static float normalizeValue(float value, float oldMin, float oldMax, float newMin, float newMax);
};