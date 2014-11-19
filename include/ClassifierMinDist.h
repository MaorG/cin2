#pragma once
#include "common.h"
#include "Classifier.h"
#include "Model.h"
#include "Entity.h"
#include "floatfann.h"
#include "fann_cpp.h"

class Classifier;

class ClassifierMinDist : public Classifier
{
protected:
	std::vector<Model*> trainingModels;
	float getDistanceBetweenModels(Model * first, Model * second);


public:

	~ClassifierMinDist();
	ClassifierMinDist(){};


	// todo: these belong in another class of classifier
	Model* GetPreprocessedModel(Model *model);
	void prepareTrainingData(std::vector<Model*> * inputModels);
	void train();
	void test(float ratio);
	void classify(Model * model);

};

