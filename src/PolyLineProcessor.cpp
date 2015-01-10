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

PolyLineEntity* PolyLineProcessor::unitePolyLines(std::vector<PolyLineEntity*> * entities, int pointAmount)
{

	if (pointAmount < 2) {
		return NULL;
	}
	PolyLine2f * result = new PolyLine2f();
	std::vector<PolyLine2f *> polyLines;
	std::vector<PolyLine2f *> sampledPolyLines;
	std::vector<int> pointAmounts;
	std::vector<float> lengths;

	float lengthSum = 0;

	for (auto entity : *entities) {
		polyLines.push_back(entity->getObject());
		float length = calcLength(entity->getObject());
		lengths.push_back(length);
		lengthSum += length;
	}
	float segmentLength = lengthSum / (float)(pointAmount - 1);
	float lengthInEntity = 0;

	int currentEntityIndex = 0;


	for (int i = 0; i < pointAmount - 1; i++) {
				

		while (lengthInEntity > lengths[currentEntityIndex]) {
			lengthInEntity -= lengths[currentEntityIndex];
			currentEntityIndex++;
		}

		Vec2f point = pointAlongPolyLine(polyLines[currentEntityIndex], lengthInEntity);
//		Vec2f point = polyLines[currentEntityIndex]->getPosition(lengthInEntity / lengths[currentEntityIndex]);

		result->push_back(point);

		lengthInEntity += segmentLength;

	}

	Vec2f point = polyLines.back()->getPosition(1.0);
	result->push_back(point);

	PolyLineEntity* resultEntity = new PolyLineEntity();
	resultEntity->setObject(result);

	return resultEntity;



}

std::pair<bool, bool> getPolyLinesPairOrientationsByEndpoints(Vec2f a1, Vec2f a2, Vec2f b1,	Vec2f b2)
{
	float gaps[4];

	gaps[0] = (a1 - b1).length();
	gaps[1] = (a1 - b2).length();
	gaps[2] = (a2 - b1).length();
	gaps[3] = (a2 - b2).length();

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

	return std::pair<bool, bool>(reverseFirst, reverseSecond);

}

std::pair<bool, bool> getPolyLinesPairOrientations(PolyLineEntity* firstEntity, PolyLineEntity* secondEntity)
{
	PolyLine2f * first = firstEntity->getObject();
	PolyLine2f * second = secondEntity->getObject();

	if (first->size() <= 0) {
		return std::pair<bool, bool>(false, false);
	}

	if (second->size() <= 0) {
		return std::pair<bool, bool>(false, false);
	}

	return getPolyLinesPairOrientationsByEndpoints(
		*(first->begin()), *(first->end() - 1),
		*(second->begin()), *(second->end() - 1));
}

void PolyLineProcessor::orientPolylines(std::vector<PolyLineEntity*> * entities)
{ 
	if (entities->size() < 2) {
		return;
	}
	if ((*entities)[0]->getObject()->size() == 0) {
		return;
	}

	std::vector<bool> reversals = std::vector<bool>(entities->size());
	for (int i = 0; i < entities->size(); i++) {
		reversals[i] = false;
	}

	Vec2f headStart = *(*entities)[0]->getObject()->begin();
	Vec2f headEnd = *((*entities)[0]->getObject()->end()-1);
	for (int i = 1; i < entities->size(); i++) {
		if ((*entities)[i]->getObject()->size() == 0) {
			continue;
		}
		Vec2f tailStart = *(*entities)[i]->getObject()->begin();
		Vec2f tailEnd = *((*entities)[i]->getObject()->end() - 1);

		auto pairReversals = getPolyLinesPairOrientationsByEndpoints(
			headStart, headEnd, tailStart, tailEnd);

		if (pairReversals.first) {
			for (int j = 0; j < i; j++) {
				PolyLineEntity* temp = entities->at(j);
				entities->at(j) = entities->at(i - j - 1);
				entities->at(i - j - 1) = temp;
				reversals[j] = !reversals[j];
			}
			headStart = headEnd;
		}

		if (pairReversals.second) {
			reversals[i] = !reversals[i];
			tailEnd = tailStart;
		}

		headEnd = tailEnd;
	}

	for (int i = 0; i < entities->size(); i++) {
		if (reversals[i]){
			reverse((*entities)[i]);
		}
	}

}

void PolyLineProcessor::chainPolyLines(PolyLineEntity* firstEntity, PolyLineEntity* secondEntity)
{
	std::pair<bool, bool> toReverse = getPolyLinesPairOrientations(firstEntity, secondEntity);

	PolyLine2f * first = firstEntity->getObject();
	PolyLine2f * second = secondEntity->getObject();

	bool reverseFirst = toReverse.first;
	bool reverseSecond = toReverse.second;

	if (reverseFirst) {
		reverse(firstEntity);
	}
	if (reverseSecond) {
		reverse(secondEntity);
	}

	firstEntity->concat(secondEntity);
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
	//int degree = points.size() - 1 < 4 ? points.size() - 1 : 4;
	int degree = 1;
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

	for (std::vector<Vec2f>::iterator it = polyLine->begin()+1; it != polyLine->end(); it++) {
		Vec2f d = Vec2f(*it - *(it-1));
		float segmentLength = d.length();

		if (accumulatedDist + segmentLength == along) {
			return (*it);
		}
		else if (accumulatedDist + segmentLength > along) {
			float remainder = along - accumulatedDist;

			if (segmentLength > 0) {

				Vec2f v = *(it-1) + d * (remainder / segmentLength);

				if (remainder / segmentLength > 1) {
					int a = 0;
				}
				return v;
			}
		}
		else {
			accumulatedDist += segmentLength;
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
