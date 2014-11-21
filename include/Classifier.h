#pragma once
#include "common.h"
#include "Model.h"


// todo: use Model* as input/testing, 
// and vector<float> (or map<key digit, score float >) for output
// classifierManager will actually classify the Model
class Classifier
{
protected:
public:

	virtual Model* GetPreprocessedModel(Model *model)=0;
	virtual void prepareTrainingData(std::vector<Model*> * inputModels) = 0;	
	virtual void train() = 0;
	virtual void test(float ratio) = 0;
	virtual void classify(Model * model) = 0;

};

