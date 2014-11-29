#pragma once
#include "common.h"
#include "Classifier.h"
#include "Model.h"
#include "Entity.h"
#include "floatfann.h"
#include "SequenceAligner.h"

class Classifier;

class ClassifierDP : public Classifier
{
protected:
	std::vector<Model*> trainingModels;
	float getSequenceAlignScore(std::vector<int> A, std::vector<int> B);
	std::vector<int> getSequenceFromModel(Model * model);
	int sampleSize;
	void initAligner();
	SequenceAligner aligner;

public:

	ClassifierDP(int sampleSize) :
		sampleSize(sampleSize){
		initAligner();
	};
	~ClassifierDP(){};


	// todo: these belong in another class of classifier
	Model* getPreprocessedModel(Model *model);
	void prepareTrainingData(std::vector<Model*> * inputModels);
	void train(){};
	void test(float ratio){};
	ClassificationResult classify(Model * model);

};

