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

Model* ClassifierMinDist::getPreprocessedModel(Model *model) {
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

	processedModel->setDigit(model->getDigit());
	PolyLineEntity* result = PolyLineProcessor::prepareForNN(first, true, sampleSize);
	delete first;
	processedModel->addEntity(result);
	return processedModel;
}

void ClassifierMinDist::prepareTrainingData(std::vector<Model*> * inputModels)
{
	trainingModels.clear();
	for (std::vector<Model*>::iterator it = inputModels->begin(); it != inputModels->end(); it++){
		Model * temp = getPreprocessedModel(*it);
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

ClassificationResult ClassifierMinDist::classify(Model * model)
{

	ClassificationResult result = std::vector <float>(10);

	if (trainingModels.size() == 0) {
		return result;
	}
	Model * processedInputModel = getPreprocessedModel(model);



	float minDist = 1000;
	char digit = '?';

	// show matched model.. nasty

	Model * match;

	float minDistances[10];
	
	for (int i = 0; i < 10; i++) {
		minDistances[i] = 1000;
	}

	for (std::vector<Model*>::iterator it = trainingModels.begin(); it != trainingModels.end(); it++){
		float dist = getDistanceBetweenModels(processedInputModel, *it);
		int digitIndex = (*it)->getDigit()-'0';
		if (dist < minDistances[digitIndex]) {
			minDistances[digitIndex] = dist;
		}
	}

	// prepare result from minDistances
	// get minimal dist over all digits
	for (int i = 0; i < 10; i++) {
		if (minDist > minDistances[i]) {
			minDist = minDistances[i];
		}
	}
	if (minDist == 0.0f) {
		// shouldn't get here
		return result;
	}
	// normalize
	for (int i = 0; i < 10; i++) {
		result[i] = minDist / minDistances[i];
	}

	delete processedInputModel;

	return result;
	
	//model->setDigit(digit);

	// temp!!

	//model->clear();

	//PolyLineEntity* a = (PolyLineEntity*)processedInputModel->getEntityByIndex(0);
	//PolyLineEntity* b = (PolyLineEntity*)match->getEntityByIndex(0);
	//
	//PolyLineProcessor::chainPolyLines(a, b);
	//model->addEntity(a);
//	model->addEntity(b);

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
		firstPolyLine = firstEntity->getObject();
	}

	float lengthFirst = PolyLineProcessor::calcLength(firstPolyLine);
	float lengthSecond = PolyLineProcessor::calcLength(secondPolyLine);

		//  todo - use actual points  (?)

	float totalDist = 0.0f;

	for (float tRel = 0.0f; tRel <= 1.0f; tRel += 0.1) {
		totalDist += (
			firstPolyLine->getPosition(tRel * lengthFirst) -
			secondPolyLine->getPosition(tRel * lengthSecond)
			).length();
	}
	// todo - calc area
	//first->addEntity(secondEntity);
	
	return totalDist;
}


