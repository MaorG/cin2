#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include <cinder/PolyLine.h>
#include "AppWindow.h"
#include "AppWindowInput.h"
#include "Entity.h"
#include "FileManager.h"
#include "PolyLineRenderer.h"
#include "PolyLineProcessor.h"
#include "Model.h"
#include "Classifier.h"
#include "ClassifierNNBattery.h"

#define SAMPLE_SIZE 10
using namespace ci;
using namespace ci::app;
using namespace std;

class cin2App : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void mouseDrag( MouseEvent event );	
	void mouseUp(MouseEvent event);	
	void keyDown(KeyEvent event);
	void update();
	void draw();
	void clearModels();
	void storeModel();
	void storeModel(char digit);
	void addToFile(Model * model);
	void trainClassifier();
	void testClassifier(float ratio);
	void getTrainingDataFromFile();

	vector<AppWindow*> windows;
	Model *inputModel;
	Model *processedModel;
	Model *anglesModel;

	vector<Model*> trainingModel;

	FileManager fileManager;
	Classifier * classifier;

	bool pressed = false;

	int index = 0;

};

void cin2App::setup()
{
	AppRenderer * polyLineRenderer1 = new PolyLineRenderer();
	AppRenderer * polyLineRenderer2 = new PolyLineRenderer();
	AppRenderer * polyLineRenderer3 = new PolyLineRenderer();

	classifier = new ClassifierNNBattery(SAMPLE_SIZE);

	inputModel = new Model();
	processedModel = new Model();
	anglesModel = new Model();

	AppWindow * window1 = new AppWindowInput();
	window1->setModel(inputModel);
	window1->setRect(Rectf(0, 0, 200, 200));
	window1->addRenderer(polyLineRenderer1);
	windows.push_back(window1);

	AppWindow * window2 = new AppWindow();
	window2->setModel(processedModel);
	window2->setRect(Rectf(210, 0, 410, 200));
	window2->addRenderer(polyLineRenderer2);
	windows.push_back(window2);

	AppWindow * window3 = new AppWindow();
	window3->setModel(anglesModel);
	window3->setRect(Rectf(420, 0, 620, 200));
	window3->addRenderer(polyLineRenderer3);
	windows.push_back(window3);

}

void cin2App::keyDown(KeyEvent event) 
{
	char c = event.getChar();

	if (c >= '0' && c <= '9') {
		inputModel -> setDigit(c);

		storeModel(c);
		clearModels();
	}
	switch (c) {
	case 'x':
		clearModels();
		break;
	case 's':
		storeModel();
		clearModels();
		break;
	case 'l':
		clearModels();

		if (trainingModel.size() > index) {

			inputModel = trainingModel.at(index++);
			{
				std::vector<Entity*> *entities = inputModel->getEntities();
				for (std::vector<Entity*>::iterator it = entities->begin(); it != entities->end(); it++) {
					if ((*it)->isPolyLineEntity()) {
						PolyLineEntity* processedEntity = PolyLineProcessor::process1((PolyLineEntity*)*it, true, SAMPLE_SIZE);
						processedModel->addEntity(processedEntity);
						PolyLineEntity* anglesEntity = PolyLineProcessor::process2(processedEntity);
						anglesModel->addEntity(anglesEntity);
					}
				}
			}
		}
		else {
			index = 0;
		}
		break;
	case 't':
		trainClassifier();
		break;
	case 'r':
		testClassifier(0.15);
		break;
	case 'c':
		classifier->classify(inputModel);
//		clearModels();		
		break;
	}
}

void cin2App::getTrainingDataFromFile() {
	stringstream ss;
	std::string fileName;

	ss << "myDigits.json";
	ss >> fileName;


	vector<Model*> * temp = fileManager.getDigitsFromJSONFile(fileName);
	trainingModel.insert(trainingModel.end(), temp->begin(), temp->end());

	fileManager.setFlippedInput(true);
	for (int i = 0; i < 10; i++) {
		stringstream ss;
		std::string fileName;

		ss << "digit_" << i << ".json";
		ss >> fileName;


		vector<Model*> * temp = fileManager.getDigitsFromJSONFile(fileName);
		trainingModel.insert(trainingModel.end(), temp->begin(), temp->end());
	}

	classifier->prepareTrainingData(&trainingModel);

}

void cin2App::trainClassifier()
{
	getTrainingDataFromFile();
//	classifier.train();
	classifier->train();
}

void cin2App::testClassifier(float ratio)
{
	getTrainingDataFromFile();
	//	classifier.train();
	classifier->test(ratio);
}

void cin2App::mouseDown( MouseEvent event )
{
//	clearModels();
	pressed = true;
	for (std::vector<AppWindow*>::iterator it = windows.begin(); it != windows.end(); ++it) {
		(*it) -> mouseDown(event);
	}	
}

void cin2App::mouseUp(MouseEvent event)
{
	//classifier.classify(&inputModel);
	//classifier.classifyArray(inputModel);
}

void cin2App::mouseDrag(MouseEvent event)
{
	for (std::vector<AppWindow*>::iterator it = windows.begin(); it != windows.end(); ++it) {
		(*it)->mouseDrag(event);
	}


	// todo: give this a name, put in in a function with a parameter function. probabaly there's a pattern for this
	PolyLineEntity* Last = (PolyLineEntity*)(inputModel->getEntityByIndex(inputModel->size() - 1));

	PolyLineEntity* processedEntity = PolyLineProcessor::process1(Last, true, SAMPLE_SIZE);
//	PolyLineEntity* processedEntity2 = PolyLineProcessor::process1(processedEntity, true, SAMPLE_SIZE);
	PolyLineEntity* anglesEntity = PolyLineProcessor::process2(processedEntity);

	if (!pressed) {
		//processedModel->popEntity();
		anglesModel->popEntity();
	}
	delete processedEntity;
	processedModel = classifier->GetPreprocessedModel(inputModel);
	windows.at(1)->setModel(processedModel);
	anglesModel->addEntity(anglesEntity);
	pressed = false;
	
}

void cin2App::clearModels() {
	inputModel->clear();
//	delete inputModel;
	processedModel->clear();
//	delete processedModel;
	anglesModel->clear();
//	delete anglesModel;
}

void cin2App::storeModel(char digit) 
{
	processedModel->setDigit(digit);
	addToFile(inputModel);
}

void cin2App::storeModel() 
{
	processedModel->setDigit('!');
	addToFile(processedModel);
}

void cin2App::addToFile(Model * model)
{
	fileManager.writeDigitToJSONFile(model, "myDigits.json");
}

void cin2App::update()
{

}

void cin2App::draw()
{
	gl::clear(Color(0.5, 0.5, 0.5));
	for (std::vector<AppWindow*>::iterator it = windows.begin(); it != windows.end(); ++it) {
		(*it)->AppWindow::draw();
	}

	gl::color(Color(0, 0, 0));
	gl::drawLine(Vec2f(0, 100), Vec2f(700, 100));;
}

CINDER_APP_NATIVE( cin2App, RendererGl )
