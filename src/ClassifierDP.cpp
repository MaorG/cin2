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
	processedModel->normalizeBoundingBox();
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


ClassificationResult ClassifierDP::classify(Model * model)
{

	ClassificationResult result = std::vector <float>(10);

	if (trainingModels.size() == 0) {
		return result;
	}
	Model * processedInputModel = getPreprocessedModel(model);



	float maxScore = -INFINITY;
	char digit = '?';

	// show matched model.. nasty

	Model * match;

	float maxScores[10];
	
	for (int i = 0; i < 10; i++) {
		maxScores[i] = -INFINITY;
	}
	std::vector<int> modelVector = getSequenceFromModel(processedInputModel);

	delete processedInputModel;

	for (std::vector<Model*>::iterator it = trainingModels.begin(); it != trainingModels.end(); it++){

		std::vector<int> exampleVector = getSequenceFromModel(*it);

		float score = getSequenceAlignScore(modelVector, exampleVector);
		int digitIndex = (*it)->getDigit()-'0';
		if (score > maxScores[digitIndex]) {
			maxScores[digitIndex] = score;
		}
	}

	// prepare result from minDistances
	// get minimal dist over all digits

	float minScore = INFINITY;
	for (int i = 0; i < 10; i++) {
		if (maxScore < maxScores[i]) {
			maxScore = maxScores[i];
		}
		if (minScore > maxScores[i]) {
			minScore = maxScores[i];
		}
	}
	//if (maxScore == 0.0f) {
	//	// shouldn't get here
	//	return result;
	//} todo:: !! - avoid getting 0 at each max scores


	// normalize
	for (int i = 0; i < 10; i++) {
		result[i] = PolyLineProcessor::normalizeValue(maxScores[i], minScore , maxScore, 0.01, 1.0);
	}

	return result;
}


std::vector<int> ClassifierDP::getSequenceFromModel(Model * model)
{


	std::vector<Entity*> * entities = model->getEntities();

	//isPolyLineEntity
	PolyLineEntity* first = ((PolyLineEntity*)*(entities->begin()))->clone();

	std::vector<float> anglesVector = PolyLineProcessor::toAnglesVector(first->getObject());

	vector<int> sequence(0);

	for (int i = 0; i < anglesVector.size(); i++) {

		float val = anglesVector[i] * 10; // todo parametrize

		int intVal = (int)round(val);

		sequence.push_back(intVal);
	}

	return sequence;
}

void ClassifierDP::initAligner() 
{

	//std::map<int, std::map<int, float>> * matchScoreMatrix_;

	aligner.setGapScore(-1.0);

}

float ClassifierDP::getSequenceAlignScore(std::vector<int> A, std::vector<int> B)
{

	//std::map<int, std::map<int, float>> * matchScoreMatrix_;

	float score = aligner.getTotalScore(A, B);
	return score;

}