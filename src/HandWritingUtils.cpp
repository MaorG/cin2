#include <math.h>
#include <stdio.h>
#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "HandWritingUtils.h"

float HandWritingUtils::normalizeValue(float value, float oldMin, float oldMax, float newMin, float newMax)
{
	float midValue = (value - oldMin) / (oldMax - oldMin);
	float newValue = (midValue)* (newMax - newMin) + newMin;
	return newValue;
}

void HandWritingUtils::printResultMatrix(std::string name, std::vector<std::vector<int>> resultMatrix)
{
	int amountCorrect = 0;
	int amountTotal = 0;

	ci::app::console() << name << std::endl;
	for (int i = 0; i < resultMatrix.size(); i++) {
		ci::app::console() << "(" << i << ")"; // todo: use symbol string

		for (int j = 0; j < resultMatrix[i].size(); j++) {

			amountTotal += resultMatrix[i][j];
			if (i == j) {
				amountCorrect += resultMatrix[i][j];
			}

			ci::app::console() << "\t" << resultMatrix[i][j];
		}
		ci::app::console() << std::endl;
	}

	float rate = (float)amountCorrect / (float)amountTotal;
	ci::app::console() << " total: " << rate;
	ci::app::console() << std::endl;
}

