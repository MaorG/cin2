#include "Classifier.h"
#include "PolyLineEntity.h"
#include "PolyLineProcessor.h"
#include "floatfann.h"
#include "fann_cpp.h"

#include <algorithm>    // std::shuffle
#include <array>        // std::array
#include <random>       // std::default_random_engine
#include <chrono>       // std::chrono::system_clock

using namespace ci;
using namespace std;

Model* Classifier::GetPreprocessedModel(Model *model, int sampleSize) {
	Model * processedModel = new Model();
	 
	std::vector<Entity*> * entities = model->getEntities();

	//isPolyLineEntity
	PolyLineEntity* first = ((PolyLineEntity*)*(entities->begin()))->clone();
	
	
	for (std::vector<Entity*>::iterator it = entities->begin()+1; it != entities->end(); ++it) {

		if ((*it)->isPolyLineEntity()) {
			PolyLineEntity* polyLineEntity = (PolyLineEntity*)(*it);
			PolyLineProcessor::chainPolyLines(first, polyLineEntity);
		}
	}
	PolyLineEntity* result = PolyLineProcessor::process1(first, true, sampleSize);
	delete first;
	processedModel->addEntity(result);
	return processedModel;
	
	
	
}

std::vector <float> Classifier::convertEntityToInputVector(Entity * entity, int sampleSize)
{
	std::vector <float> inputVector = std::vector <float>();

	//processing polyLine
	if (entity->isPolyLineEntity()) {
		PolyLineEntity * resampledEntity = PolyLineProcessor::process1((PolyLineEntity*)entity, true, sampleSize + 1);

		if (PolyLineProcessor::isPolylineClosed(resampledEntity)) {
			int a = 0;
		}

		inputVector = PolyLineProcessor::toAnglesVector(resampledEntity->getObject());
//		inputVector.push_back(PolyLineProcessor::getStartAngle(resampledEntity));
		inputVector.push_back(PolyLineProcessor::isPolylineClosed(resampledEntity) ? 1.0f : -1.0f);

		// add delta between CoG and Farthest
		Vec2f CoG = PolyLineProcessor::getCenterOfGravity(resampledEntity);
		Vec2f farthest = PolyLineProcessor::getFarthestFromCenter(resampledEntity);

		inputVector.push_back((CoG - farthest).x);
		inputVector.push_back((CoG - farthest).y);

		delete resampledEntity;
	}

	return inputVector;
}

std::vector <float> Classifier::convertModelToInputVector(Model * model, int sampleSize)
{
	std::vector <float> inputVector = std::vector <float>();
	if (model->size() > 0){



		inputVector = convertEntityToInputVector(model->getEntityByIndex(0), sampleSize);
	}
	return inputVector;
}

void Classifier::prepareTrainingData(std::vector<Model*> * inputModels, int sampleSize)
{
	setSampleSize(sampleSize);
	setInputVectorSize((convertModelToInputVector(*(inputModels->begin()), sampleSize)).size());

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	shuffle(inputModels->begin(), inputModels->end(), std::default_random_engine(seed));

	for (std::vector<Model*>::iterator it = inputModels->begin(); it != inputModels->end(); it++) {
		Model * processedModel = GetPreprocessedModel(*it, m_sampleSize);
		std::vector <float> inputVector = convertModelToInputVector(processedModel, m_sampleSize);
		delete processedModel;

		trainingInput.push_back(inputVector);


		int outputValue = (*it)->getDigit() - '0';

		std::vector<float> outputVector;
		for (int i = 0; i < 10; i++) {
			outputVector.push_back(outputValue == i ? 1.0 : 0.0);
		}

		trainingOutput.push_back(outputVector);
	}
}

//void Classifier::createTrainingDataFromVectors(int sampleSize)
//{
//setInputVectorSize
//
//}

//void Classifier::createTrainingDataFromOneVector(int sampleSize)
//{
//setInputVectorSize
//
//}

void Classifier::train()
{
	//struct fann* 
	//ann = fann_create_sparse(0.5, 4, inputVectorSize, 10, 10, 10);
	ann = fann_create_standard(4, m_inputVectorSize, 10, 10, 10);
	struct fann_train_data ftd;


	// todo; move to an aux func
	fann_type **inputArray;
	inputArray = (float**)malloc(trainingInput.size() *sizeof(float *));
	for (int i = 0; i < trainingInput.size(); i++) {
		inputArray[i] = (float*)malloc(m_inputVectorSize * sizeof(float));
	}

	fann_type **outputArray;
	outputArray = (float**)malloc(trainingOutput.size() *sizeof(float *));
	for (int i = 0; i < trainingOutput.size(); i++) {
		outputArray[i] = (float*)malloc(10 * sizeof(float));
	}

	for (int i = 0; i < trainingInput.size(); i++) {
		for (int ip = 0; ip < m_inputVectorSize; ip++) {
			inputArray[i][ip] = trainingInput[i][ip];
		}
		for (int id = 0; id < 10; id++) {
			outputArray[i][id] = trainingOutput[i][id];
		}
	}

	ftd.num_data = trainingInput.size();
	ftd.num_input = m_inputVectorSize;
	ftd.num_output = 10;
	ftd.input = inputArray;
	ftd.output = outputArray;


	fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
	//fann_set_activation_function_hidden(ann, FANN_LINEAR);

	fann_train_on_data(ann, &ftd, 1000, 10, 0.02);

	printf("Mean Square Error: %f\n", fann_get_MSE(ann));
	
}

void Classifier::classify(Model * model) {

	if (ann == NULL) {
		return;
	}
	std::vector <float> inputVector = convertModelToInputVector(model , m_sampleSize);
	std::vector <float> outputVector = std::vector <float>(10);
	// todo; move to an aux func
	fann_type *inputArray;
	inputArray = (float*)malloc(inputVector.size() *sizeof(float));
	for (int i = 0; i < inputVector.size(); i++) {
		inputArray[i] = inputVector.at(i);
	}

	fann_type *outputArray;
	outputArray = (float*)malloc(outputVector.size() *sizeof(float));
	for (int i = 0; i < outputVector.size(); i++) {
		outputArray[i] = 0;
	}


	outputArray = fann_run(ann, inputArray);
	int output = -1;
	float max = -10;
	for (int i = 0; i < outputVector.size(); i++) {
		if (outputArray[i] > max) {
			max = outputArray[i];
			output = i;
		}
	}

	model->setDigit('0' + output);
}

void Classifier::prepareNNBattery()
{
	fannBattery.clear();

	for (int i = 0; i < 10; i++) {
		struct fann* ann;
		ann = fann_create_standard(4, m_inputVectorSize, 7, 5, 1);
		fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
//		fann_set_activation_function_hidden(ann, FANN_LINEAR);
	
		fannBattery.push_back(ann);
	}
}

fann_type ** Classifier::convertTrainingInputToVectors()
{
	fann_type **inputArray = (float**)malloc(trainingInput.size() *sizeof(float *));
	for (int i = 0; i < trainingInput.size(); i++) {
		inputArray[i] = (float*)malloc(m_inputVectorSize * sizeof(float));
	}

	for (int i = 0; i < trainingInput.size(); i++) {
		for (int ip = 0; ip < m_inputVectorSize; ip++) {
			inputArray[i][ip] = trainingInput[i][ip];
		}
	}
	return inputArray;
}

fann_type ** Classifier::convertTrainingOutputToVectors(int digitIndex)
{
	fann_type ** outputArray = (float**)malloc(trainingOutput.size() *sizeof(float *));
	for (int i = 0; i < trainingOutput.size(); i++) {
		outputArray[i] = (float*)malloc(1 * sizeof(float));
	}
	for (int i = 0; i < trainingInput.size(); i++) {
		if (trainingOutput[i][digitIndex] > 0) {
			outputArray[i][0] = 1.0;
		}
		else {
			outputArray[i][0] = 0.0;
		}
	}
	return outputArray;
}



void Classifier::trainBattery()
{
	testBattery(0);
}

void Classifier::testBattery(float ratio)
{

	prepareNNBattery();

	struct fann_train_data ftd;

	fann_type **inputArray = convertTrainingInputToVectors();

	fann_type **outputArray[10];
	for (int digitIndex = 0; digitIndex < 10; digitIndex++) {
		// todo; move to an aux func, with a function that converts outputs
		outputArray[digitIndex] = convertTrainingOutputToVectors(digitIndex);
	}

	int trainingAmount = (int)((1.0-ratio)* trainingInput.size());


	for (int digitIndex = 0; digitIndex < 10; digitIndex++) {
//		ftd.num_data = trainingInput.size();
		ftd.num_data = trainingAmount;
		ftd.num_input = m_inputVectorSize;
		ftd.num_output = 1;
		ftd.input = inputArray;
		ftd.output = outputArray[digitIndex];
		fann_train_on_data(fannBattery[digitIndex], &ftd, 100, 10, 0.02);
		ci::app::console() << "Mean Square Error: " << digitIndex << ":  "<< fann_get_MSE(fannBattery[digitIndex]) << endl;
		ci::app::console() << endl;
	}

	if (ratio <= 0) {
		return;
	}
	// testing

	int result[10][10];

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			result[i][j] = 0;
		}
	}

	for (int exampleIndex = trainingAmount; exampleIndex < trainingInput.size(); exampleIndex++) {

		std::vector <float> outputVector = std::vector <float>(10);
		fann_type *output;
		output = (float*)malloc(1 * sizeof(float));

		for (int digitIndex = 0; digitIndex < 10; digitIndex++) {
			output = fann_run(fannBattery[digitIndex], inputArray[exampleIndex]);
			outputVector[digitIndex] = (float)*output;
		}

		int finalOutput = -1;
		float max = -10;
		for (int i = 0; i < outputVector.size(); i++) {
			if (outputVector[i] > max) {
				max = outputVector[i];
				finalOutput = i;
			}
		}

		int expectedOutput = -1;
		max = -10;
		for (int i = 0; i < 10; i++) {
			if (trainingOutput.at(exampleIndex).at(i) > max) {
				max = trainingOutput.at(exampleIndex).at(i);
				expectedOutput = i;
			}
		}

		result[expectedOutput][finalOutput] ++ ;


	}

	int amountCorrect = 0;
	int amountTotal = 0;

	for (int i = 0; i < 10; i++) {
		ci::app::console() << "(" << i << ")";

		for (int j = 0; j < 10; j++) {

			amountTotal += result[i][j];
			if (i == j) {
				amountCorrect += result[i][j];
			}

			ci::app::console() << "\t" << result[i][j] ;
		}
		ci::app::console() << endl;
	}


	float rate = (float)amountCorrect / (float)amountTotal;
	ci::app::console() << endl;
//	ci::app::console() << " total: " << rate;
	ci::app::console() << " ____ ";


}

void Classifier::classifyBattery(Model * model) {

	if (fannBattery.size() < 10) {
		return;
	}
	std::vector <float> inputVector = convertModelToInputVector(model, m_sampleSize);
	std::vector <float> outputVector = std::vector <float>(10);
	// todo; move to an aux func
	fann_type *inputArray;
	inputArray = (float*)malloc(inputVector.size() *sizeof(float));
	for (int i = 0; i < inputVector.size(); i++) {
		inputArray[i] = inputVector.at(i);
	}

	fann_type *outputArray;
	outputArray = (float*)malloc(1 *sizeof(float));

	for (int digitIndex = 0; digitIndex < 10; digitIndex++) {
		outputArray = fann_run(fannBattery[digitIndex], inputArray);
		outputVector[digitIndex] = outputArray[0];
	}

	
	int output = -1;
	float max = -10;
	for (int i = 0; i < outputVector.size(); i++) {
		if (outputVector[i] > max) {
			max = outputVector[i];
			output = i;
		}
	}

	model->setDigit('0' + output);
}
