#pragma once
#include "common.h"
#include "Model.h"
#include "Classifier.h"
#include "ClassifierNNBattery.h"
#include "ClassifierMinDist.h"
#include "ClassifierDP.h"
#include "string.h"
#include "AppContext.h"

class HandWritingManager
{
protected:
	AppContext * context;
	std::map<std::string, Classifier*> classifiers;
	std::vector<Model*> * testModels;
	ClassificationResult classifyToResult(Model * model, bool preview);
public:
	HandWritingManager(AppContext * context, int sampleSize);
	void setExampleModels(std::string classifierName, std::vector<Model*> * models);
	void setTestModels(std::vector<Model*> * models);
	void test();
	void classify(Model * model, bool preview);
	void classifySequence(Model * model, bool preview);

	Model* getPreprocessedModel(std::string classifierName, Model *model);
};

