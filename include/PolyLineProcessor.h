#pragma once
#include "common.h"
#include "PolyLineEntity.h"


class PolyLineProcessor
{
	ci::PolyLine2f * object;

public:
	PolyLineProcessor();
	~PolyLineProcessor();

	void PolyLineProcessor::orientPolylines(std::vector<PolyLineEntity*> * entities);

	static void chainPolyLines(PolyLineEntity* firstEntity, PolyLineEntity* secondEntity);
	static PolyLineEntity* unitePolyLines(std::vector<PolyLineEntity*> * entities, int pointAmount);



	static PolyLineEntity * prepareForNN(PolyLineEntity* polyLineEntity, bool relative, int segments);
	static PolyLineEntity * process2(PolyLineEntity* polyLineEntity);
	static ci::PolyLine2f* uniformResample(ci::PolyLine2f* polyLine, int amount);
	static void reverse(PolyLineEntity* polyLineEntity);
	static ci::PolyLine2f* uniformResampleByLength(ci::PolyLine2f* polyLine, float approximateSegmentLength);
	static ci::PolyLine2f* toAngles(ci::PolyLine2f* polyLine);
	static std::vector<float> toAnglesVector(ci::PolyLine2f* polyLine);

	static ci::Vec2f pointAlongPolyLine(ci::PolyLine2f* polyLine, float along);

	static float calcLength(ci::PolyLine2f* polyLine);

	static float getStartAngle(PolyLineEntity* polyLineEntity);
	static ci::Vec2f getCenterOfGravity(PolyLineEntity* polyLineEntity);
	static ci::Vec2f getFarthestFromCenter(PolyLineEntity* polyLineEntity);

	static bool isPolylineClosed(PolyLineEntity* polyLineEntity);

	static float PolyLineProcessor::getAngleBetweenVectors(ci::Vec2f a, ci::Vec2f b);

	static float normalizeValue(float value, float oldMin, float oldMax, float newMin, float newMax);
};