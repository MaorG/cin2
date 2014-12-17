#include "ClassifierDP.h"
#include "PolyLineEntity.h"
#include "PolyLineProcessor.h"

#include <algorithm>    // std::shuffle
#include <array>        // std::array
#include <random>       // std::default_random_engine
#include <chrono>       // std::chrono::system_clock

using namespace ci;
using namespace std;

Model* ClassifierDP::getPreprocessedModel(Model *model) {
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
	//processedModel->normalizeBoundingBox();
	first = (PolyLineEntity*)processedModel->getEntityByIndex(0);
	processedModel->popEntity();

	processedModel->setDigit(model->getDigit());
	PolyLineEntity* result = PolyLineProcessor::prepareForNN(first, true, sampleSize);
	delete first;
	processedModel->addEntity(result);
	return processedModel;
}

void ClassifierDP::prepareTrainingData(std::vector<Model*> * inputModels)
{
	trainingModels.clear();
	for (std::vector<Model*>::iterator it = inputModels->begin(); it != inputModels->end(); it++){
		Model * temp = getPreprocessedModel(*it);
		trainingModels.push_back(temp);
	}
}


ClassificationResult ClassifierDP::classifyAndPreview(Model * model)
{
	Model * match;
	ClassificationResult result;
	std::tuple<ClassificationResult, Model*> resultAndMatch = classifyDP(model);

	Model * temp = getPreprocessedModel(model);

	result = std::get<0>(resultAndMatch);
	match = std::get<1>(resultAndMatch);

	if (match == NULL) {
		return result;
	}

	Model * preview = new Model();
	preview->setDigit(match->getDigit());

	Entity * modelEntity = temp->getEntityByIndex(0);
	Entity * matchEntity = match->getEntityByIndex(0);
	Entity * modelEntityClone = modelEntity->clone();
	Entity * matchEntityClone = matchEntity;

	matchEntityClone->setColor(ci::Color(0.0, 1.0, 0.0));

	preview->addEntity(modelEntityClone);
	preview->addEntity(matchEntityClone);

	// drawing matches between models:

	std::vector<float> modelVector = getSequenceFromModel(temp);
	std::vector<float> matchVector = getSequenceFromModel(match);

	std::tuple<std::vector<int>, std::vector<int>> alignments = 
		aligner->getBestAlignment(modelVector, matchVector);

	std::vector<int> alignmentA = std::get<0>(alignments);
	std::vector<int> alignmentB = std::get<1>(alignments);

	for (int i = 0; i, i < alignmentA.size(); i++) {
		PolyLineEntity * polyLineEntity = new PolyLineEntity();
		polyLineEntity->setColor(ci::Color(0.0, 0.0, 1.0));
		int indexA = alignmentA.at(i) + 1;
		int indexB = alignmentB.at(i) + 1;
		Vec2f point1 = ((PolyLineEntity*)modelEntityClone)->getObject()->getPoints().at(indexA);
		Vec2f point2 = ((PolyLineEntity*)matchEntityClone)->getObject()->getPoints().at(indexB);
		polyLineEntity->appendPoint(point1);
		polyLineEntity->appendPoint(point2);
		preview->addEntity(polyLineEntity);
	}

	context->putModelInWindowByIndex(4, preview);

	delete temp;

	return result;

}

ClassificationResult ClassifierDP::classify(Model * model) {
	return std::get<0>(classifyDP(model));
}


std::tuple<ClassificationResult, Model*> ClassifierDP::classifyDP(Model * model)
{

	ClassificationResult result = std::vector <float>(10);
	std::tuple<ClassificationResult, Model*> resultAndMatch;

	Model * match = NULL;
	if (trainingModels.size() == 0) {
		resultAndMatch = std::make_tuple(result, match);
		return resultAndMatch;
	};
	Model * processedInputModel = getPreprocessedModel(model);

	float maxScore = -INFINITY;
	char digit = '?';

	for (int i = 0; i < 10; i++) {
		result[i] = -INFINITY;
	}
	std::vector<float> modelVector = getSequenceFromModel(processedInputModel);

	delete processedInputModel;

	for (std::vector<Model*>::iterator it = trainingModels.begin(); it != trainingModels.end(); it++){

		std::vector<float> exampleVector = getSequenceFromModel(*it);

		float score = getSequenceAlignScore(modelVector, exampleVector);

		if (maxScore < score) {
			maxScore = score;
			match = (*it);
		}

		int digitIndex = (*it)->getDigit()-'0';
		if (score > result[digitIndex]) {
			result[digitIndex] = score;
		}
	}

	context->putModelInWindowByIndex(4, match);

	resultAndMatch = std::make_tuple(result, match);
	return resultAndMatch;
}


std::vector<float> ClassifierDP::getSequenceFromModel(Model * model)
{


	std::vector<Entity*> * entities = model->getEntities();

	//isPolyLineEntity
	PolyLineEntity* first = ((PolyLineEntity*)*(entities->begin()))->clone();

	std::vector<float> anglesVector = PolyLineProcessor::toAnglesVector(first->getObject());

	return anglesVector;
}

float ClassifierDP::getSequenceAlignScore(std::vector<float> A, std::vector<float> B)
{

	float score = aligner->getTotalScore(A, B);
	return score;

}

float matchScoreFunction(float a, float b)
{
	float score = abs(a - b);
	if (score > 1.0) {
		score = abs(2.0 - score);
	}

	return 10.0 * (0.2 - score);
}

float gapScoreFunction(float a)
{
	float score = abs(a);

	if (score < 0.1) {
		score = 0.1;
	}	

	return - 5.0 * score;

}

void ClassifierDP::initAligner()
{

	//std::map<int, std::map<int, float>> * matchScoreMatrix_;
	aligner = new SequenceAligner();

	aligner->setMatchScoreFunction(matchScoreFunction);
	aligner->setGapScoreFunction(gapScoreFunction);
}