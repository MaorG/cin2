#pragma once
#include "common.h"
#include "Classifier.h"
#include "Model.h"
#include "Entity.h"
#include "floatfann.h"
#include "fann_cpp.h"

class Classifier;

class ClassifierNNBattery : public Classifier
{
protected:
	std::vector<std::vector <float>> trainingInput;
	std::vector<std::vector<float>> trainingOutput;

	std::vector<std::vector <float>> testingInput;
	std::vector<std::vector<float>> testingOutput;

	std::vector<std::vector <float>> classifyInput;
	std::vector<std::vector<float>> classifyOutput;

	int m_inputVectorSize;
	int m_sampleSize;
	struct fann* ann = NULL;

	std::vector<struct fann*> fannBattery;

	std::vector <float> convertEntityToInputVector(Entity * entity);
	std::vector <float> convertModelToInputVector(Model * model); 
	void prepareNNBattery();

	fann_type ** convertTrainingInputToVectors();
	fann_type ** convertTrainingOutputToVectors(int digitIndex);
	
	ClassificationResult classifyBattery(Model * model);
	void setInputVectorSize(int inputVectorSize_) { m_inputVectorSize = inputVectorSize_; };
	void setSampleSize(int sampleSize_) { m_sampleSize = sampleSize_; };

public:

	~ClassifierNNBattery();
	ClassifierNNBattery(int sampleSize) :
		m_sampleSize(sampleSize){};


	// todo: these belong in another class of classifier
	Model* getPreprocessedModel(Model *model);
	void prepareTrainingData(std::vector<Model*> * inputModels);
	void train();
	void test(float ratio);
	ClassificationResult classify(Model * model);

};

