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

	PolyLineProcessor::orientPolylines((std::vector<PolyLineEntity*>*)entities);
	
	PolyLineEntity* polyLineEntity = PolyLineProcessor::unitePolyLines((std::vector<PolyLineEntity*>*)entities, sampleSize);

	processedModel->addEntity(polyLineEntity);
	processedModel->setSymbol(model->getSymbol());
	processedModel->normalizeBoundingBox();
	return processedModel;
}

Model* getPreprocessedModelold(Model *model) {
	Model * processedModel = new Model();

	std::vector<Entity*> * entities = model->getEntities();

	//isPolyLineEntity


	PolyLineEntity* first = ((PolyLineEntity*)*(entities->begin()))->clone();


	for (std::vector<Entity*>::iterator it = entities->begin() + 1; it != entities->end(); ++it) {

		if ((*it)->isPolyLineEntity()) {
			PolyLineEntity* polyLineEntity = (PolyLineEntity*)(*it);
			PolyLineProcessor::chainPolyLines(first, polyLineEntity);
		}
	}

	processedModel->addEntity(first);
	processedModel->normalizeBoundingBox();
	first = (PolyLineEntity*)processedModel->getEntityByIndex(0);
	processedModel->popEntity();

	processedModel->setSymbol(model->getSymbol());
	PolyLineEntity* result = PolyLineProcessor::prepareForNN(first, true, 10/*sampleSize*/);
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

ClassificationResult  ClassifierMinDist::classifyAndPreview(Model * model) {

	Model * match;
	ClassificationResult result;
	std::tuple<ClassificationResult, Model*> resultAndMatch = classifyMinDist(model);

	Model * temp = getPreprocessedModel(model);

	result = std::get<0>(resultAndMatch);
	match = std::get<1>(resultAndMatch);

	if (match == NULL) {
		return result;
	}

	Model * preview = new Model();
	preview->setSymbol(match->getSymbol());

	Entity * modelEntity = temp->getEntityByIndex(0);
	Entity * matchEntity = match->getEntityByIndex(0);
	Entity * modelEntityClone = modelEntity->clone();
	Entity * matchEntityClone = matchEntity;

	matchEntityClone->setColor(ci::Color(0.0, 1.0, 0.0));

	preview->addEntity(modelEntityClone);
	preview->addEntity(matchEntityClone);

	context->putModelInWindowByIndex(3, preview);


	return result;
}

ClassificationResult ClassifierMinDist::classify(Model * model)
{
	return std::get<0>(classifyMinDist(model));
}

std::tuple<ClassificationResult, Model*>  ClassifierMinDist::classifyMinDist(Model * model)
{

	ClassificationResult result;

	std::tuple<ClassificationResult, Model*> resultAndMatch;

	Model * match = NULL;
	if (trainingModels.size() == 0) {
		resultAndMatch = std::make_tuple(result, match);
		return resultAndMatch;
	};
	Model * processedInputModel = getPreprocessedModel(model);



	float minDist = INFINITY;
	std::string symbol = "?";

	result.classifiactionMap.clear();

	for (std::vector<Model*>::iterator it = trainingModels.begin(); it != trainingModels.end(); it++){
		float dist = getDistanceBetweenModels(processedInputModel, *it);
		std::string matchSymbol = (*it)->getSymbol();

		if (result.classifiactionMap.find(matchSymbol) == result.classifiactionMap.end()) {
			result.classifiactionMap.insert(std::pair<std::string, float>(matchSymbol, dist));
		}

		if (dist < result.classifiactionMap[matchSymbol]) {
			result.classifiactionMap[matchSymbol] = dist;
		}

		if (dist < minDist) {
			minDist = dist;
			match = (*it);
		}
	}


	// converting "cost" to "score"
	for (std::map<std::string, float>::iterator it = result.classifiactionMap.begin(); 
		it != result.classifiactionMap.end(); ++it) {
		(*it).second *= -1.0f;
	}

	resultAndMatch = std::make_tuple(result, match);

	delete processedInputModel;

	return resultAndMatch;
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


