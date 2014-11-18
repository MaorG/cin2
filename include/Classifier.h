#pragma once
#include "common.h"
#include "Model.h"

class Classifier
{
protected:
public:

	virtual Model* GetPreprocessedModel(Model *model) = 0;
	virtual void prepareTrainingData(std::vector<Model*> * inputModels) = 0;	
	virtual void train() = 0;
	virtual void test(float ratio) = 0;
	virtual void classify(Model * model) = 0;

};

