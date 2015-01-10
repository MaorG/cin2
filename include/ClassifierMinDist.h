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
	int sampleSize;
	std::tuple<ClassificationResult, Model*> classifyMinDist(Model * model);

public:

	ClassifierMinDist(AppContext * context, int sampleSize) :
		Classifier(context),
		sampleSize(sampleSize) {};
	~ClassifierMinDist(){};


	// todo: these belong in another class of classifier
	Model* getPreprocessedModel(Model *model);
	void prepareTrainingData(std::vector<Model*> * inputModels);
	void train();
	void test(float ratio);
	ClassificationResult classify(Model * model);
	ClassificationResult classifyAndPreview(Model * model);
};

