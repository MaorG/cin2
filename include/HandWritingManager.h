#pragma once
#include "common.h"
#include "Model.h"
#include "Classifier.h"
#include "ClassifierNNBattery.h"
#include "ClassifierMinDist.h"
#include "string.h"

// todo: use Model* as input/testing, 
// and vector<float> (or map<key digit, score float >) for output
// classifierManager will actually classify the Model
class HandWritingManager
{
protected:
	std::map<std::string, Classifier*> classifiers;
	std::vector<Model*> * testModels;
	ClassificationResult classifyToResult(Model * model);
public:
	HandWritingManager(int sampleSize);

	Model* getPreprocessedModel(std::string classifierName, Model *model);

	void setExampleModels(std::string classifierName, std::vector<Model*> * models);
	void setTestModels(std::vector<Model*> * models);
	void test();
	void classify(Model * model);
};

