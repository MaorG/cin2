#include <math.h>
#include <stdio.h>
#include "HandWritingUtils.h"

float HandWritingUtils::normalizeValue(float value, float oldMin, float oldMax, float newMin, float newMax)
{
	float midValue = (value - oldMin) / (oldMax - oldMin);
	float newValue = (midValue)* (newMax - newMin) + newMin;
	return newValue;
}

