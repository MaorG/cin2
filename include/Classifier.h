#pragma once
#include "common.h"
#include "AppContext.h"
#include "Model.h"

struct Classification2Result
{
	std::map<std::string, float> classifiactionMap;
	std::string resultSymbol;
	float confidence;
};

// todo: use Model* as input/testing, 
// and vector<float> (or map<key digit, score float >) for output
// classifierManager will actually classify the Model
class Classifier
{
protected:
	AppContext * context;

public:

	Classifier(AppContext * aContext) :
		context(aContext) {};

	virtual Model* getPreprocessedModel(Model *model)=0;
	virtual void prepareTrainingData(std::vector<Model*> * inputModels) = 0;	
	virtual void train() = 0;
	virtual void test(float ratio) = 0;
	virtual Classification2Result classify(Model * model) = 0;
	virtual Classification2Result classifyAndPreview(Model * model) = 0;

};

