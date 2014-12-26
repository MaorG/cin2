#include "HandWritingManager.h"

char getDigitFromResult(ClassificationResult result) 
{
	int output = -1;
	float max = -INFINITY;
	for (int i = 0; i < result.size(); i++) {
		if (result[i] > max) {
			max = result[i];
			output = i;
		}
	}

	return output + '0';
}

HandWritingManager::HandWritingManager(AppContext * context, int sampleSize):
	context(context)
{

	//ClassifierNNBattery * NNBattery = new ClassifierNNBattery(context, sampleSize);
	//classifiers["NN"] = NNBattery;

	ClassifierMinDist * minDist = new ClassifierMinDist(context, sampleSize);
	classifiers["MinDist"] = minDist;

	//ClassifierDP * dynamic = new ClassifierDP(context, sampleSize);
	//classifiers["Dynamic"] = dynamic;
}


void HandWritingManager::setExampleModels(std::string classifierName, std::vector<Model*> * models)
{
	std::map<std::string, Classifier*>::iterator it = classifiers.find(classifierName);

	if (it == classifiers.end()) {
		return;
	}
	Classifier * classifier = it->second;
	classifier->prepareTrainingData(models);
	classifier->train();
}

void HandWritingManager::setTestModels(std::vector<Model*> * models) {
	testModels = models;
}

Model* HandWritingManager::getPreprocessedModel(std::string classifierName, Model *model) {
	
	if (classifiers.size() == 0) {
		return NULL;
	}
	
	std::map<std::string, Classifier*>::iterator it = classifiers.find(classifierName);

	if (it == classifiers.end()) {
		
		it = classifiers.begin();
	}
	Classifier * classifier = it->second;
	return classifier->getPreprocessedModel(model);
}

ClassificationResult HandWritingManager::classifyToResult(Model * model, bool preview)
{
	ClassificationResult result = std::vector <float>(10);
	std::vector <ClassificationResult> resultVector;

	for (std::map<std::string, Classifier*>::iterator it = classifiers.begin();
		it != classifiers.end(); ++it) {

		Classifier * classifier = it->second;

		ClassificationResult classifierResult;
		if (!preview) {
			classifierResult = classifier->classify(model);
		}
		else {
			classifierResult = classifier->classifyAndPreview(model);
		}


		// normalizing result
		float minScore = INFINITY;
		float maxScore = -INFINITY;
		for (int i = 0; i < 10; i++) {
			if (maxScore < classifierResult[i]) {
				maxScore = classifierResult[i];
			}
			if (minScore > classifierResult[i]) {
				minScore = classifierResult[i];
			}
		}

		// normalize
		for (int i = 0; i < 10; i++) {
			result[i] = HandWritingUtils::normalizeValue(classifierResult[i], minScore, maxScore, 0.01, 1.0);
		}


		resultVector.push_back(classifierResult);

	}

	for (int i = 0; i < result.size(); i++) {
		result[i] = 0;
	}

	for (int resultindex = 0; resultindex < resultVector.size(); resultindex++) {
		for (int i = 0; i < result.size(); i++) {
			result[i] += (0.5 + resultVector[resultindex][i]);
		}
	}
	return result;
}

void HandWritingManager::classify(Model * model, bool preview)
{
	ClassificationResult result = classifyToResult(model, preview);
	model->setDigit(getDigitFromResult(result));
}

void HandWritingManager::classifySequence(Model * model, bool preview) 
{
	std::vector<Entity*> * entities = model->getEntities();

	std::vector < std::pair<float, std::vector<int>>> compositionScores;

	std::vector<int> composition;
	//greedy.. O(n^2), still intensive

	int suffixIndex = 0;
	while (suffixIndex < entities->size()) {
		int indexInSuffix = getBestPrefixSizeInSuffix(model, suffixIndex);
		composition.push_back(indexInSuffix + 1);
		suffixIndex += indexInSuffix + 1;
	}
	composition.push_back(suffixIndex + 1);
	int a = 0;
}

void HandWritingManager::classifySequenceRec(Model * model)
{
	std::map < std::pair<int, int>, float> subsequenceScoreMap;
	// prepare a map of all subsequences
	std::vector<Entity*> * entities = model->getEntities();
	for (int i = 0; i < entities->size(); i++) {
		for (int j = i; i < entities->size(); i++) {
			subsequenceScoreMap[std::pair<int, int>(i, j)] =
				classifySubSequence(model, i, j);
		}
	}

}

void classifySequenceAux(
	Model * model,
	int recIndex,
	std::map < std::pair<int, int>, float> subsequenceScoreMap,
	std::pair<float, std::vector<int>> composition,
	std::pair<float, std::vector<int>> bestComposition)
{
	std::vector<Entity*> * entities = model->getEntities();

	// check if leaf
	if (recIndex >= entities->size()) {
		//overwrite best
		if (composition.first > bestComposition.first) {
			bestComposition.first = composition.first;
			bestComposition.second.clear();
			for each (auto val in composition.second) {
				bestComposition.second.push_back(val);
			}
		}

		return;
	}

	// recursion. two options;
	// 1. continue last sequence;
	int lastNumOfSubsequences = composition.second.size();
	int lastSubsequenceLength = 
		composition.second.at(composition.second.size() - 1);
	int lastSubsequenceScore =
		subsequenceScoreMap[
			std::pair<int, int>(recIndex - lastSubsequenceLength + 1, recIndex)];
	int updatedLastSubsequenceScore = 
		subsequenceScoreMap[
			std::pair<int, int>(recIndex - lastSubsequenceLength + 1, recIndex + 1)];

	composition.first = composition.first - lastSubsequenceScore + updatedLastSubsequenceScore;
	composition.second.at(composition.second.size() - 1)++;
	classifySequenceAux(model, recIndex + 1, subsequenceScoreMap, composition, bestComposition);

	//roll baxk changes
	composition.first = composition.first + lastSubsequenceScore - updatedLastSubsequenceScore;
	composition.second.at(composition.second.size() - 1)--;


	// 2. breakLastSequence


	
}

int HandWritingManager::getBestPrefixSizeInSuffix(Model* model, int startIndex)
{

	float bestScore = -INFINITY;
	int bestCut;

	std::vector<Entity*> * entities = model->getEntities();

	for (int i = startIndex; i < entities->size(); i++) {
		float score = classifySubSequence(model, startIndex, i);
		if (score > bestScore) {
			bestScore = score;
			bestCut = i;
		}

	}

	return bestCut - startIndex;
	
}

float HandWritingManager::classifySubSequence(Model * model, int startIndex, int endIndex) {

	Model * subModel = new Model();
	std::vector<Entity*> * entities = subModel->getEntities();

	for (int i = startIndex; i <= endIndex; i++) {
		subModel->addEntity(model->getEntityByIndex(i)->clone());
	}

	ClassificationResult result = classifiers.begin()->second->classify(subModel);

	float max = -INFINITY;
	for (int i = 0; i < result.size(); i++) {
		if (result[i] > max) {
			max = result[i];
		}
	}

	return max;
	
}



void HandWritingManager::test()

// todo - push into vector of classifiers. also - make an output struct
{
	int resultMatrix[10][10] = {0};

	for each (auto classifierPair in classifiers) {
		Classifier * classifier = classifierPair.second;
		for (std::vector<Model*>::iterator it = testModels->begin(); it != testModels->end(); it++) {
			int expectedOutput = (*it)->getDigit() - '0';
			int finalOutput;
			ClassificationResult result;
			for each (auto classifierPair in classifiers) {

				Classifier * classifier = classifierPair.second;
				result = classifier->classify(*it);
				finalOutput = getDigitFromResult(result) - '0';
				resultMatrix[expectedOutput][finalOutput] ++;
			}
		}
		HandWritingUtils::printResultMatrix(classifierPair.first, resultMatrix);
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
				resultMatrix[i][j] = 0;
			}
		}
	}

	for (std::vector<Model*>::iterator it = testModels->begin(); it != testModels->end(); it++) {
		int expectedOutput = (*it)->getDigit() - '0';
		int finalOutput;
		ClassificationResult result;
		result = classifyToResult(*it, false);
		finalOutput = getDigitFromResult(result) - '0';
		resultMatrix[expectedOutput][finalOutput] ++;
	}


	HandWritingUtils::printResultMatrix("Combined", resultMatrix);

}