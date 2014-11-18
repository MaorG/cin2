#pragma once
#include "common.h"
#include "Model.h"

class Classifier
{
protected:
public:

	virtual ~Classifier(){};
	virtual void prepareTrainingData(std::vector<Model*> * inputModels, int sampleSize) = 0;	
	virtual void trainBattery() = 0;
	virtual void classifyBattery(Model * model);
	virtual void testBattery(float ratio);
	virtual void classify(Model * model);

};

