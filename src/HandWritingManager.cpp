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

HandWritingManager::HandWritingManager(int sampleSize) {
	ClassifierNNBattery * NNBattery = new ClassifierNNBattery(sampleSize);
	ClassifierMinDist * minDist = new ClassifierMinDist(sampleSize);

	classifiers["NN"] = NNBattery;
	classifiers["MinDist"] = minDist;
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

ClassificationResult HandWritingManager::classifyToResult(Model * model)
{
	ClassificationResult result = std::vector <float>(10);
	ClassificationResult NNresult =
		classifiers["NN"]->classify(model);
	ClassificationResult MinDistResult =
		classifiers["MinDist"]->classify(model);

	for (int i = 0; i < result.size(); i++) {
		result[i] = NNresult[i] * MinDistResult[i];
	}
	return result;
}

void HandWritingManager::classify(Model * model)
{
	ClassificationResult result = classifyToResult(model);
	model->setDigit(getDigitFromResult(result));
}

void printResultMatrix(std::string name, int resultMatrix[10][10]){
	int amountCorrect = 0;
	int amountTotal = 0;

	ci::app::console() << name << std::endl;
	for (int i = 0; i < 10; i++) {
		ci::app::console() << "(" << i << ")";

		for (int j = 0; j < 10; j++) {

			amountTotal += resultMatrix[i][j];
			if (i == j) {
				amountCorrect += resultMatrix[i][j];
			}

			ci::app::console() << "\t" << resultMatrix[i][j];
		}
		ci::app::console() << std::endl;
	}

	float rate = (float)amountCorrect / (float)amountTotal;
	ci::app::console() << " total: " << rate;
	ci::app::console() << std::endl;
}

void HandWritingManager::test()
{
	int resultMatrix[10][10];
	int NNResultMatrix[10][10];
	int minDistResultMatrix[10][10];

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			resultMatrix[i][j] = 0;
			NNResultMatrix[i][j] = 0;
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

		result = classifyToResult(*it);
		finalOutput = getDigitFromResult(result) - '0';
		resultMatrix[expectedOutput][finalOutput] ++;
	}

	printResultMatrix("NN", NNResultMatrix);
	printResultMatrix("MinDist", minDistResultMatrix);
	printResultMatrix("Combined", resultMatrix);

}