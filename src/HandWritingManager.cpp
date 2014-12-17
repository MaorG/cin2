#include "HandWritingManager.h"

char getDigitFromResult(ClassificationResult result) 
{
	int output = -1;
	float max = -10;
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

	ClassifierNNBattery * NNBattery = new ClassifierNNBattery(context, sampleSize);
	classifiers["NN"] = NNBattery;

	ClassifierMinDist * minDist = new ClassifierMinDist(context, sampleSize);
	classifiers["MinDist"] = minDist;

	ClassifierDP * dynamic = new ClassifierDP(context, sampleSize);
	classifiers["Dynamic"] = dynamic;
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
	std::map<std::string, Classifier*>::iterator it = classifiers.find(classifierName);

	if (it == classifiers.end()) {
		return NULL;
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

void classifySequence(Model * model, bool preview);


void HandWritingManager::test()

// todo - push into vector of classifiers. also - make an output struct
{
	int resultMatrix[10][10];
	int NNResultMatrix[10][10];
	int minDistResultMatrix[10][10];
	int dynamicResultMatrix[10][10];

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			resultMatrix[i][j] = 0;
			NNResultMatrix[i][j] = 0;
			dynamicResultMatrix[i][j] = 0;
			minDistResultMatrix[i][j] = 0;
		}
	}

	for (std::vector<Model*>::iterator it = testModels->begin(); it != testModels->end(); it++) {
		int expectedOutput = (*it)->getDigit() - '0';
		int finalOutput;
		ClassificationResult result;

		result = classifiers["NN"]->classify(*it);
		finalOutput = getDigitFromResult(result) - '0';
		NNResultMatrix[expectedOutput][finalOutput] ++;

		result = classifiers["MinDist"]->classify(*it);
		finalOutput = getDigitFromResult(result) - '0';
		minDistResultMatrix[expectedOutput][finalOutput] ++;

		result = classifiers["Dynamic"]->classify(*it);
		finalOutput = getDigitFromResult(result) - '0';
		dynamicResultMatrix[expectedOutput][finalOutput] ++;

		result = classifyToResult(*it, false);
		finalOutput = getDigitFromResult(result) - '0';
		resultMatrix[expectedOutput][finalOutput] ++;
	}

	HandWritingUtils::printResultMatrix("NN", NNResultMatrix);
	HandWritingUtils::printResultMatrix("MinDist", minDistResultMatrix);
	HandWritingUtils::printResultMatrix("Dynamic", dynamicResultMatrix);
	HandWritingUtils::printResultMatrix("Combined", resultMatrix);

}