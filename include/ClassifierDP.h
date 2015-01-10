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

	std::tuple<ClassificationResult, Model*> classifyDP(Model * model);

	float getSequenceAlignScore(std::vector<float> A, std::vector<float> B);
	std::vector<float> getSequenceFromModel(Model * model);
	int sampleSize;
	void initAligner();
	SequenceAligner * aligner;

	//float matchScoreFunction(float a, float b);
	//float gapScoreFunction(float a);

public:

	ClassifierDP(AppContext * context, int sampleSize) :
		Classifier(context),
		sampleSize(sampleSize) {
		initAligner();
	};
	~ClassifierDP(){
		delete aligner;
	};


	// todo: these belong in another class of classifier
	Model* getPreprocessedModel(Model *model);
	void prepareTrainingData(std::vector<Model*> * inputModels);
	void train(){};
	void test(float ratio){};
	ClassificationResult classify(Model * model);
	ClassificationResult classifyAndPreview(Model * model);

};

