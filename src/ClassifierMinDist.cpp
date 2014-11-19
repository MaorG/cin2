#include "ClassifierMinDist.h"
#include "PolyLineEntity.h"
#include "PolyLineProcessor.h"
#include "floatfann.h"
#include "fann_cpp.h"

#include <algorithm>    // std::shuffle
#include <array>        // std::array
#include <random>       // std::default_random_engine
#include <chrono>       // std::chrono::system_clock

using namespace ci;
using namespace std;

Model* ClassifierMinDist::GetPreprocessedModel(Model *model) {
	Model * processedModel = new Model();
	 
	std::vector<Entity*> * entities = model->getEntities();

	//isPolyLineEntity
	PolyLineEntity* first = ((PolyLineEntity*)*(entities->begin()))->clone();
	
	
	for (std::vector<Entity*>::iterator it = entities->begin()+1; it != entities->end(); ++it) {

		if ((*it)->isPolyLineEntity()) {
			PolyLineEntity* polyLineEntity = (PolyLineEntity*)(*it);
			PolyLineProcessor::chainPolyLines(first, polyLineEntity);
		}
	}

	processedModel->addEntity(first);
	processedModel->normalizeBoundingBox();
	first = (PolyLineEntity*)processedModel->getEntityByIndex(0);
	processedModel->popEntity();

	PolyLineEntity* result = PolyLineProcessor::prepareForNN(first, true, 10);
	delete first;
	processedModel->addEntity(result);
	return processedModel;
}

void ClassifierMinDist::prepareTrainingData(std::vector<Model*> * inputModels)
{
	trainingModels.clear();
	for (std::vector<Model*>::iterator it = inputModels->begin(); it != inputModels->end(); it++){
		Model * temp = GetPreprocessedModel(*it);
		trainingModels.push_back(temp);
	}
}


void ClassifierMinDist::train()
{
	test(0);
}

void ClassifierMinDist::test(float ratio)
{
	ci::app::console() << endl;
}

void ClassifierMinDist::classify(Model * model) 
{
	Model * inputModel = GetPreprocessedModel(model);

	float minDist = 1000;
	char digit = '?';

	// show matched model.. nasty

	Model * match;

	for (std::vector<Model*>::iterator it = trainingModels.begin(); it != trainingModels.end(); it++){
		float dist = getDistanceBetweenModels(inputModel, *it);
		if (dist < minDist) {
			minDist = dist;
			digit = (*it)->getDigit();
			match = (*it);
		}
	}
	model->setDigit(digit);

	return;
	PolyLineEntity* a = (PolyLineEntity*)model->getEntityByIndex(0);
	PolyLineEntity* b = (PolyLineEntity*)match->getEntityByIndex(0);
	
	a->concat(b);

}

float ClassifierMinDist::getDistanceBetweenModels(Model * first, Model * second) {

	// todo - check if polyline

	// check if reverse needed

	PolyLineEntity* firstEntity = (PolyLineEntity*)first->getEntityByIndex(0);
	PolyLineEntity* secondEntity = (PolyLineEntity*)second->getEntityByIndex(0);


	PolyLine2f * firstPolyLine = firstEntity->getObject();
	PolyLine2f * secondPolyLine = secondEntity->getObject();
	if (firstPolyLine->size() < 2 || secondPolyLine->size() < 2) {
		return false;
	}
	float distSumHH= 
		(*(firstPolyLine->begin()) - *(secondPolyLine->begin())).length() +
		(*(firstPolyLine->end()-1) - *(secondPolyLine->end()-1)).length();
	float distSumHT =
		(*(firstPolyLine->begin()) - *(secondPolyLine->end()-1)).length() +
		(*(firstPolyLine->end()-1) - *(secondPolyLine->begin())).length();

	if (distSumHH > distSumHT) {
		PolyLineProcessor::reverse(firstEntity);
	}

	float lengthFirst = PolyLineProcessor::calcLength(firstPolyLine);
	float lengthSecond = PolyLineProcessor::calcLength(secondPolyLine);

	float totalDist = 0.0f;

	for (float tRel = 0.0f; tRel <= 1.0f; tRel += 0.1) {

		totalDist += (
			firstPolyLine->getPosition(tRel * lengthFirst) -
			secondPolyLine->getPosition(tRel * lengthSecond)
			).length();
	}
	// todo - calc area
	//firstEntity->concat(secondEntity);
	
	return totalDist;
}


