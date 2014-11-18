#pragma once
#include "common.h"
#include "Model.h"
#include "Entity.h"
#include "floatfann.h"
#include "fann_cpp.h"

class Classifier
{
protected:
	std::vector<std::vector <float>> trainingInput;
	std::vector<std::vector<float>> trainingOutput;
	int m_inputVectorSize;
	int m_sampleSize;
	struct fann* ann = NULL;

	std::vector<struct fann*> fannBattery;

	std::vector <float> convertEntityToInputVector(Entity * entity, int sampleSize);
	std::vector <float> convertModelToInputVector(Model * model, int sampleSize); 
	void prepareNNBattery();

	fann_type ** Classifier::convertTrainingInputToVectors();
	fann_type ** convertTrainingOutputToVectors(int digitIndex);


public:
	void setInputVectorSize(int inputVectorSize_) { m_inputVectorSize = inputVectorSize_; };
	void setSampleSize(int sampleSize_) { m_sampleSize = sampleSize_; };
	void prepareTrainingData(std::vector<Model*> * inputModels, int sampleSize);
	Model* GetPreprocessedModel(Model *model, int sampleSize);
	void train();

	// todo: these belong in another class of classifier
	void trainBattery();
	void classifyBattery(Model * model);
	void testBattery(float ratio);
	void classify(Model * model);

};

