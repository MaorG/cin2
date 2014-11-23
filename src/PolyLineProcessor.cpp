#include "PolyLineProcessor.h"
#include <cinder/PolyLine.h>
#include <cinder/path2d.h>
#include <cinder/BSpline.h>
#include <math.h>
#include <stdio.h>
using namespace ci;

PolyLineProcessor::PolyLineProcessor()
{
}

PolyLineProcessor::~PolyLineProcessor()
{
}

void PolyLineProcessor::chainPolyLines(PolyLineEntity* firstEntity, PolyLineEntity* secondEntity)
{
	PolyLine2f * first = firstEntity->getObject();
	PolyLine2f * second = secondEntity->getObject();

//	if (first->size() == 0) {
//		firstEntity->concat(secondEntity);
//	}
		
	if (second->size() == 0) {
		return;
	}

	float gaps[4];

	gaps[0] = (*(first->begin()) - *(second->begin())).length();
	gaps[1] = (*(first->begin()) - *(second->end() - 1)).length();
	gaps[2] = (*(first->end() - 1) - *(second->begin())).length();
	gaps[3] = (*(first->end() - 1) - *(second->end() - 1)).length();

	int minGapIndex = 0;
	float minGap = gaps[0];
	for (int i = 1; i<4; i++) {
		if (gaps[i] < minGap) {
			minGap = gaps[i];
			minGapIndex = i;
		}
	}

	bool reverseFirst = (minGapIndex == 0 || minGapIndex == 1);
	bool reverseSecond = (minGapIndex == 1 || minGapIndex == 3);

	if (reverseFirst) {
		reverse(firstEntity);
	}
	if (reverseSecond) {
		reverse(secondEntity);
	}

	firstEntity->concat(secondEntity);
	//if (reverseFirst) {
	//	reverse(firstEntity);
	//}
	//if (reverseSecond) {
	//	reverse(secondEntity);
	//}

}

void PolyLineProcessor::reverse(PolyLineEntity* polyLineEntity)
{
	PolyLine2f * polyLine = polyLineEntity->getObject();
	if (polyLine->size() < 2) {
		return;
	}
	PolyLine2f * reversePolyLine = new PolyLine2f();

	std::vector<Vec2f> points = polyLine->getPoints();

	for (int i = points.size() - 1; i>=0; i--) {
		reversePolyLine->push_back(points.at(i));
	}
	polyLineEntity->setObject(reversePolyLine);

	//delete polyLine;
}


PolyLineEntity * PolyLineProcessor::prepareForNN(PolyLineEntity* polyLineEntity, bool relative, int segments)
{
	PolyLineEntity * proecessedPolyLineEntity = new PolyLineEntity();

	PolyLine2f * polyLine = polyLineEntity->getObject();
	PolyLine2f * processedPolyLine;// = new PolyLine<Vec2f>();

	if (relative) {
		PolyLine2f * tempPolyLine;
		tempPolyLine = uniformResample(polyLine, 2 * segments);
		processedPolyLine = uniformResample(tempPolyLine, segments);
		delete tempPolyLine;
	}
	else{
		processedPolyLine = uniformResampleByLength(polyLine, 0.05f);
	}
	proecessedPolyLineEntity->setObject(processedPolyLine);

	return proecessedPolyLineEntity;

}

Vec2f PolyLineProcessor::getFarthestFromCenter(PolyLineEntity* polyLineEntity)
{
	Vec2f farthest = Vec2f(0.0, 0.0);
	PolyLine2f * polyLine = polyLineEntity->getObject();

	if (polyLine->size() == 0) {
		return farthest;
	}
	Vec2f CoG = getCenterOfGravity(polyLineEntity);

	float maxDist = 0;
	for (std::vector<Vec2f>::iterator it = polyLine->begin(); it != polyLine->end(); ++it) {
		if ((*it - CoG).length() > maxDist) {
			maxDist = (*it - CoG).length();
			farthest = *it;
		}
	}
	return farthest;
}

Vec2f PolyLineProcessor::getCenterOfGravity(PolyLineEntity* polyLineEntity) 
{
	Vec2f CoG = Vec2f(0.0, 0.0);
	PolyLine2f * polyLine = polyLineEntity->getObject();
	if (polyLine->size() == 0) {
		return CoG;
	}

	for (std::vector<Vec2f>::iterator it = polyLine->begin(); it != polyLine->end(); ++it) {
		CoG += (*it);
	}

	CoG *= 1.0f / (float)polyLine->size();

	return CoG;

}

float PolyLineProcessor::getStartAngle(PolyLineEntity* polyLineEntity) 
{
	PolyLine2f * polyLine = polyLineEntity->getObject();
	if (polyLine->size() < 3) {
		return 0;
	}
	float angle = getAngleBetweenVectors(Vec2f(1.0, 0.0), *(polyLine->begin() + 1) - *(polyLine->begin()));
	return angle;

}

bool PolyLineProcessor::isPolylineClosed(PolyLineEntity* polyLineEntity) 
{
	PolyLine2f * polyLine = polyLineEntity->getObject();
	if (polyLine->size() < 3) {
		return false;
	}
	float segmentLength = (*(polyLine->begin() + 1) - *(polyLine->begin())).length();
	segmentLength += (*(polyLine->end() - 1) - *(polyLine->end() - 2)).length();
	segmentLength *= 0.5f;

	float gap = (*(polyLine->begin()) - *(polyLine->end() - 1)).length();

	if (gap < 1.5*segmentLength) {
		return true;
	}
	else {
		return false;
	}
}

float PolyLineProcessor::getAngleBetweenVectors(Vec2f first, Vec2f second)
{

	float angle = 0;
	float normalize = first.length()*second.length();

	if (normalize != 0) {
		float dot = first.dot(second);
		float det = first.cross(second);
		angle = atan2(det, dot);
	}

	return angle;
}

float PolyLineProcessor::normalizeValue(float value, float oldMin, float oldMax, float newMin, float newMax)
{
	float midValue = (value - oldMin) / (oldMax - oldMin);
	float newValue = (midValue)* (newMax - newMin) + newMin;
	return newValue;
}

PolyLineEntity * PolyLineProcessor::process2(PolyLineEntity* polyLineEntity)
{
	PolyLineEntity * proecessedPolyLineEntity = new PolyLineEntity();

	PolyLine2f * polyLine = polyLineEntity->getObject();
	PolyLine2f * processedPolyLine = new PolyLine<Vec2f>();

	processedPolyLine = toAngles(polyLine);

	proecessedPolyLineEntity->setObject(processedPolyLine);
	return proecessedPolyLineEntity;

}

std::vector<float> PolyLineProcessor::toAnglesVector(PolyLine2f* polyLine)
{
	std::vector<float> angles;

	if (polyLine->size() < 3) {
		return angles;
	}

	for (std::vector<Vec2f>::iterator it = polyLine->begin(); it != polyLine->end() - 2; ++it) {
		float angle = getAngleBetweenVectors((*it) - (*(it + 1)), (*(it + 1)) - (*(it + 2)));
		angle = normalizeValue(angle, -(float)M_PI, (float)M_PI, -1.0, 1.0);

		if (angle > 0) {
			angle = pow(abs(angle), 1.0);
		}
		else {
			angle = -pow(abs(angle), 1.0);
		}
		
		angles.push_back(angle);
	}

	return angles;

}


PolyLine2f* PolyLineProcessor::uniformResampleByLength(PolyLine2f* polyLine, float approximateSegmentLength)
{
	float length = calcLength(polyLine);
	if (length == 0) {
		return new PolyLine2f();
	}
	float relativeSegmentLength = length / approximateSegmentLength;

	int numberOfSegments = (int)ceil(relativeSegmentLength);

	numberOfSegments = std::max(numberOfSegments, 5);
	return uniformResample(polyLine, numberOfSegments);

}


PolyLine2f* PolyLineProcessor::uniformResample(PolyLine2f* polyLine, int amount) 
{

	if (polyLine->size() < 2) {
		return new PolyLine2f();
	}
	double length = calcLength(polyLine);
	if (length == 0) {
		return new PolyLine2f();
	}



	std::vector<Vec2f> points = polyLine->getPoints();
	int degree = points.size() - 1 < 4 ? points.size() - 1 : 4;
	Path2d path = Path2d(BSpline2f(points, degree, false, true));

	float splineLength = path.calcLength();
	if (splineLength == 0) {
		return new PolyLine2f();
	}
	float sampleLength = splineLength / (float)amount;

	PolyLine2f* sampled = new PolyLine2f();

	for (int i = 0; i <= amount; i++) {

		float ratio = (float)i / (float)amount;
		float time = 0;
		if (ratio >= 1.0f) {
			time = 1.0f;
		}
		else {
			time = path.calcNormalizedTime(ratio);
			if (time >= 1.0f) {
				time = 1.0f;
			}
		}
		Vec2f point = path.getPosition(time);
		sampled->push_back(point);
	}

	path.clear();
	points.clear();

	return sampled;
	/*




	double accumulatedLength = 0;


	PolyLine2f* polyLineCopy = new PolyLine2f(*polyLine);


	while (accumulatedLength <= length) {

		sampled->push_back(pointAlongPolyLine(polyLine, accumulatedLength));
		accumulatedLength += sampleLength;
	}

	delete polyLineCopy;
	return sampled;

	*/
}

// todo: delete
ci::PolyLine2f*  PolyLineProcessor::toAngles(ci::PolyLine2f* polyLine) {
	PolyLine2f* angles = new PolyLine2f();

	if (polyLine->size() < 3) {
		return angles;
	}

	for (std::vector<Vec2f>::iterator it = polyLine->begin(); it != polyLine->end() - 2; ++it) {
		float xVal = (float)(it - polyLine->begin()) / (float)(polyLine->size() - 2);

		float angle = getAngleBetweenVectors((*it) - (*(it + 1)), (*(it + 1)) - (*(it + 2)));
		float yVal = normalizeValue(angle, -(float)M_PI, (float)M_PI, 0.0, 1.0);

		angles->push_back(Vec2f(xVal, yVal));
	}

	return angles;
}

Vec2f PolyLineProcessor::pointAlongPolyLine(PolyLine2f* polyLine, float along)
{
	if (along <= 0 || polyLine->size() < 2) {
		return *polyLine->begin();
	}
	float accumulatedDist = 0;
	for (std::vector<Vec2f>::iterator it = polyLine->begin(); it != polyLine->end() - 1; it++) {
		float dist = (Vec2f(it->x, it->y) - Vec2f((it + 1)->x, (it + 1)->y)).length();
		if (accumulatedDist + dist == along) {
			return (*it);
		}
		else if (accumulatedDist + dist > along) {
			float remainder = along - accumulatedDist;

			Vec2f d = Vec2f(*it - *(it+1));
			float segmentLength = d.length();
			if (segmentLength > 0) {

				Vec2f v = *it + d * (remainder / segmentLength);

				if (remainder / segmentLength > 1) {
					int a = 0;
				}
				return v;
			}
		}
		else {
			accumulatedDist += dist;
		}
	}

	return *(--polyLine->end());
}

float PolyLineProcessor::calcLength(PolyLine2f* polyLine)
{
	float length = 0;
	if (polyLine->size() < 2) {
		return 0;
	}
	for (std::vector<Vec2f>::iterator it = polyLine->begin(); it != polyLine->end() - 1; ++it) {
		float dist = (Vec2f(it->x, it->y) - Vec2f((it + 1)->x, (it + 1)->y)).length();
		length += dist;
	}
	
	return length;
}
