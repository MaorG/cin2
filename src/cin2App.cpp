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
#include "HandWritingManager.h"
#include "AppContext.h";
#include <random>       // std::default_random_engine

#define SAMPLE_SIZE 10
#define TESTING_SIZE 40


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
	void classifyModel(Model* model);

	void getTrainingDataFromFile();

	AppContext * context;

	//vector<AppWindow*> windows;
	Model *inputModel;
	Model *processedModel;
	Model *anglesModel;
	Model *minDistModel;

	FileManager fileManager;
	HandWritingManager * handWritingManager;

	bool pressed = false;

	int index = 0;
	vector<Model*> trainingModel[2];
	vector<Model*> testingModel;
};

void cin2App::setup()
{
	AppRenderer * polyLineRenderer = new PolyLineRenderer();
	context = new AppContext(); //  todo - move renderers to context
	handWritingManager = new HandWritingManager(context, SAMPLE_SIZE);

	inputModel = new Model();
	processedModel = new Model();
	anglesModel = new Model();

	AppWindow * window1 = new AppWindowInput();
	window1->setModel(inputModel);
	window1->setRect(Rectf(0, 0, 200, 200));
	window1->addRenderer(polyLineRenderer);
	context->AddWindow((window1));

	AppWindow * window2 = new AppWindow();
	window2->setModel(processedModel);
	window2->setRect(Rectf(210, 0, 410, 200));
	window2->addRenderer(polyLineRenderer);
	context->AddWindow((window2));

	AppWindow * window3 = new AppWindow();
	window3->setModel(anglesModel);
	window3->setRect(Rectf(0, 210, 200, 410));
	window3->addRenderer(polyLineRenderer);
	context->AddWindow((window3));

	AppWindow * window4 = new AppWindow();
	window4->setModel(anglesModel);
	window4->setRect(Rectf(210, 210, 410, 410));
	window4->addRenderer(polyLineRenderer);
	context->AddWindow((window4));

	AppWindow * window5 = new AppWindow();
	window5->setModel(anglesModel);
	window5->setRect(Rectf(420, 210, 620, 410));
	window5->addRenderer(polyLineRenderer);
	context->AddWindow((window5));

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

		//if (trainingModel[0].size() > index) {

		//	inputModel = trainingModel[1].at(index++);
		//	{
		//		std::vector<Entity*> *entities = inputModel->getEntities();
		//		for (std::vector<Entity*>::iterator it = entities->begin(); it != entities->end(); it++) {
		//			if ((*it)->isPolyLineEntity()) {
		//				PolyLineEntity* processedEntity = PolyLineProcessor::prepareForNN((PolyLineEntity*)*it, true, SAMPLE_SIZE);
		//				processedModel->addEntity(processedEntity);
		//				PolyLineEntity* anglesEntity = PolyLineProcessor::process2(processedEntity);
		//				anglesModel->addEntity(anglesEntity);
		//			}
		//		}
		//	}
		//}
		//else {
		//	index = 0;
		//}
		break;
	case 't':
		trainClassifier();
		break;
	case 'r':
		testClassifier(0.15);
		break;
	case 'c':
		classifyModel(inputModel);
		//		clearModels();		
		break;
	}
}

void cin2App::classifyModel(Model* model) {
	//classifiers[0]->classify(model);
	//classifiers[1]->classify(model);
	handWritingManager->classify(model, true);
}

void cin2App::getTrainingDataFromFile() {
	stringstream ss;
	std::string fileName;

	ss << "myDigits.json";
	ss >> fileName;

	vector<Model*> * temp;
	
	temp = fileManager.getDigitsFromJSONFile(fileName);
	trainingModel[0].insert(trainingModel[0].end(), temp->begin(), temp->end());

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	shuffle(trainingModel[0].begin(), trainingModel[0].end(), std::default_random_engine(seed));

	int testingAmount = min(TESTING_SIZE, (int)floor(trainingModel[0].size() * 0.3));
	for (int i = 0; i < testingAmount; i++) {
		testingModel.push_back(trainingModel[0].back());
		trainingModel[0].pop_back();
	}

	// just inserting some extra random digits to the dist
	// todo: make nice
	//trainingModel[1].insert(trainingModel[1].end(), 
	//	trainingModel[0].begin(),
	//	trainingModel[0].begin() + min(50, (int)floor(temp->size() * 0.3)));

	//trainingModel[1].insert(trainingModel[1].end(),
	//	trainingModel[0].begin(),
	//	trainingModel[0].end());

	fileManager.setFlippedInput(true);
	for (int i = 0; i < 10; i++) {
		stringstream ss;
		std::string fileName;

		ss << "digit_" << i << ".json";
		ss >> fileName;


		vector<Model*> * temp = fileManager.getDigitsFromJSONFile(fileName);
		//trainingModel[0].insert(trainingModel[0].end(), temp->begin(), temp->end());
		trainingModel[1].insert(trainingModel[1].end(), temp->begin(), temp->end());
		//trainingModel[1].insert(trainingModel[1].end(), temp->begin(), temp->begin() + 2);
	}
	
	


}

void cin2App::trainClassifier()
{
	getTrainingDataFromFile();
//	handWritingManager->setExampleModels("NN", &trainingModel[0]);
	handWritingManager->setExampleModels("MinDist", &trainingModel[1]);
	handWritingManager->setExampleModels("Dynamic", &trainingModel[1]);
}

void cin2App::testClassifier(float ratio)
{
	getTrainingDataFromFile();
//	handWritingManager->setExampleModels("NN", &trainingModel[0]);
	handWritingManager->setExampleModels("MinDist", &trainingModel[1]);
	handWritingManager->setExampleModels("Dynamic", &trainingModel[1]);
	handWritingManager->setTestModels(&testingModel);
	handWritingManager->test();
}

void cin2App::mouseDown( MouseEvent event )
{
//	clearModels();

	std::vector<AppWindow*> * windows = context->getWindows();
	pressed = true;
	for (std::vector<AppWindow*>::iterator it = windows->begin(); it != windows->end(); ++it) {
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
	std::vector<AppWindow*> * windows = context->getWindows();
	for (std::vector<AppWindow*>::iterator it = windows->begin(); it != windows->end(); ++it) {
		(*it)->mouseDrag(event);
	}


	// todo: give this a name, put in in a function with a parameter function. probabaly there's a pattern for this
	PolyLineEntity* Last = (PolyLineEntity*)(inputModel->getEntityByIndex(inputModel->size() - 1));

	PolyLineEntity* processedEntity = PolyLineProcessor::prepareForNN(Last, true, SAMPLE_SIZE);
//	PolyLineEntity* processedEntity2 = PolyLineProcessor::prepareForNN(processedEntity, true, SAMPLE_SIZE);
	PolyLineEntity* anglesEntity = PolyLineProcessor::process2(processedEntity);

	if (!pressed) {
		//processedModel->popEntity();
		anglesModel->popEntity();
	}
	delete processedEntity;
	processedModel = handWritingManager->getPreprocessedModel("NN", inputModel);
	context->putModelInWindowByIndex(1, processedModel);
	context->putModelInWindowByIndex(3, processedModel);
	anglesModel->addEntity(anglesEntity);
	pressed = false;
	
}

void cin2App::clearModels() {
	inputModel->clear();
	processedModel->clear();
	anglesModel->clear();
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
	std::vector<AppWindow*> * windows = context->getWindows();

	gl::clear(Color(0.5, 0.5, 0.5));
	for (std::vector<AppWindow*>::iterator it = windows->begin(); it != windows->end(); ++it) {
		(*it)->AppWindow::draw();
	}

	gl::color(Color(0, 0, 0));
	gl::drawLine(Vec2f(0, 100), Vec2f(700, 100));;
}

CINDER_APP_NATIVE( cin2App, RendererGl )
