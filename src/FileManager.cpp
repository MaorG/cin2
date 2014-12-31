#include "FileManager.h"
//#include "PolyLineEntity.h"
#include <sstream>
#include <fstream>
using namespace ci;
using namespace std;

void FileManager::closeBank()
{
	f.close();
}

void FileManager::setBank(string name)
{
	f.open(name, std::fstream::in | std::fstream::out | std::fstream::app);
	locationInFile = 0;

}

void FileManager::writeToBank(Model *model)
{

	std::string symbol = model->getSymbol();

	f.seekp(ios::end);
	f << "{" << endl;
	f << "digit: " << symbol << endl;
	f << "count: " << model->size() << endl;

	std::vector<Entity*> * entities = model->getEntities();
	for (std::vector<Entity*>::iterator it = entities->begin(); it != entities->end(); ++it) {
		PolyLineEntity * p = (PolyLineEntity*)(*it)->toPolyLineEntity();
		
		if (p != NULL) {
			f << "PolyLine" << endl;

			PolyLine2f * object = p->getObject();
			f << "count: " << object->size() << endl;
			for (std::vector<Vec2f>::iterator it = object->begin(); it != object->end(); ++it) {
				f << it->x << " " << it->y << endl;
			}
		}
	}

	f << "}" << endl << endl;
}

Model * FileManager::pullFromBank()
{
	Model * model = new Model();
	vector<Entity*> *entities = new vector<Entity*>();
	std::string line;
	std::string s;

	Entity* entity;

	f.seekg(locationInFile);

	while (getline(f, line) && line.compare("") == 0) {
	}

	if (line.compare("{") == 0) {
		while (getline(f, line) && line.compare("}") != 0) {
			line.append("\n");
			s.append(line);
		}

		if (line.compare("}") != 0) {
			entity = NULL;
		}

		// parseEntities..
		istringstream iss(s);
		std::string word;
		std::string digitString;
		std::string countString;
		int entityCount;

		iss >> word >> digitString;

		char digit = digitString.at(0);

		//model->setDigit(digit);
		model->setSymbol(digitString);

		if (word.compare("digit:") == 0 && digitString.size() == 1) {
			
			iss >> word >> countString;
			if (word.compare("count:") == 0) {
				istringstream(countString) >> entityCount;

				for (int i = 0; i < entityCount; i++) {
					entity = parseEntity(iss);
					if (entity != NULL) {
						model->addEntity((Entity*)entity);
					}
				}
			}
		}


	}

	locationInFile = (int)f.tellg();

	return model;

}

Entity * FileManager::parseEntity(istringstream &iss)
{

	string entityTypeline;
	string propertyValue;

	std::getline(iss, entityTypeline);
	istringstream isline(entityTypeline);

	iss >> propertyValue;

	if (propertyValue.compare("PolyLine") == 0) {
		PolyLineEntity * polyLineEntity = parsePolyLineEntity(iss);
		return polyLineEntity;
	}

	return NULL;

}


PolyLineEntity * FileManager::parsePolyLineEntity(istringstream &iss)
{

	string line;
	string propertyValue;
	std::string word;
	std::string countString;
	int pointCount;

	PolyLineEntity * polyLineEntity = new (PolyLineEntity);
	PolyLine2f * polyLine = new PolyLine2f();
	
	iss >> word >> countString;
	if (word.compare("count:") == 0) {
		istringstream(countString) >> pointCount;
		for (int i = 0; i < pointCount; i++) {

			float x, y;
			iss >> x >> y;
			polyLine->push_back(Vec2f(x, y));
		}
	}
	polyLineEntity->setObject(polyLine);

	return polyLineEntity;

	/*
	f << "PolyLine" << endl;
	f << "digit: " << digit << endl;
	f << "count: " << model->size() << endl;

	for (std::vector<Entity*>::iterator it = model->begin(); it != model->end(); ++it) {
	PolyLineEntity * p = (PolyLineEntity*)(*it)->toPolyLineEntity();

	if (p != NULL) {
	f << "PolyLine" << endl;

	PolyLine2f * object = p->getObject();
	f << "count: " << object->size() << endl;
	for (std::vector<Vec2f>::iterator it = object->begin(); it != object->end(); ++it) {
	f << it->x << " " << it->y << endl;
	}
	}
	}

	f << "}" << endl << endl;
	*/
}

FileManager::~FileManager()
{
	closeBank();
}

void FileManager::writeDigitToJSONFile(Model* model, std::string filename)
{
	std::fstream fs;

	char path[40] = "../Data/";
	char filenameArray[20];

	std::size_t length = filename.length();
	filename.copy(filenameArray, length);
	std::strncat(path, filenameArray, length);
	fs.open(path, std::fstream::in | std::fstream::out);
	bool temp = fs.good();
	if (!temp) {
		// if no such file, create new new one with json array
		fs.open(path, std::fstream::out);
		Json::Value value;
		value["type"] = "SymbolsDescriptor";
		Json::Value vec(Json::arrayValue);
		value["digits"] = vec;
		fs.close();
		fs.open(path, std::fstream::in | std::fstream::out);
	}
	Json::Value root;
	Json::Reader jReader = Json::Reader();
	jReader.parse(fs, root);

	Json::Value JsonDigits = root["digits"];
	Json::Value value = writeDigitToJSONValue(model);

	JsonDigits.append(value);
	root["digits"] = JsonDigits;

	std::ostringstream oss;

//	ci::app::console() << root << std::endl;


	Json::StyledWriter writer;
	std::string output = writer.write(root);
	fs.close();
	fs.open(path, std::fstream::out);
	fs << output << std::endl;
	fs.close();

}

Json::Value FileManager::writeDigitToJSONValue(Model* model)
{
	Json::Value value;

	value["type"] = "Stroke";
	//value["digit"] = model->getDigit() - '0';
	value["digit"] = model->getSymbol();

//	ci::app::console() << value;


	Json::Value vec(Json::arrayValue);

	std::vector<Entity*> * entities = model->getEntities();
	for (std::vector <Entity* >::iterator it = entities->begin(); it != entities->end(); ++it) {
		Json::Value entityValue = writeEntityToJSONValue(*it);
		vec.append(entityValue);
	}

	value["paths"] = vec;
	return value;

}

Json::Value FileManager::writeEntityToJSONValue(Entity* entity)
{

	Json::Value value;
	Json::Value vec(Json::arrayValue);

	PolyLineEntity * p = (PolyLineEntity*)entity->toPolyLineEntity();

	if (p != NULL) {
		value["type"] = "Path";

		PolyLine2f * object = p->getObject();

		for (std::vector<Vec2f>::iterator it = object->begin(); it != object->end(); ++it) {
			vec.append(Json::Value(it->x));
			vec.append(Json::Value(it->y));
		}
		value["coordinates"] = vec;;

	}
	return value;

	}

std::vector<Model*>* FileManager::getDigitsFromJSONFile(std::string filename)
{
	std::vector<Model*>* digits;
	std::ifstream fs;

	char path[40] = "../Data/";
	char filenameArray[20];

	std::size_t length = filename.length();
	filename.copy(filenameArray, length);
	std::strncat(path, filenameArray, length);
	fs.open(path, std::fstream::in);

	Json::Value root;

	Json::Reader jReader = Json::Reader();

	jReader.parse(fs, root);

	digits = getDigitsFromJSONValue(root);

	fs.close();
	return digits;
}

std::vector<Model*>* FileManager::getDigitsFromJSONValue(Json::Value value)
{
	std::vector<Model*>* digits = new std::vector<Model*>();

	Json::Value JsonDigits = value["digits"];

	for (int index = 0; index < JsonDigits.size(); ++index) {
		Model* digit = getSingleDigitFromJSONValue(JsonDigits[index]);
		if (digit != NULL) {
			digits->push_back(digit);
		}
	}

	return digits;
}

Model* FileManager::getSingleDigitFromJSONValue(Json::Value value)
{
	Model* model = new Model();

	std::string symbolName;
	Json::Value inputSymbol = value.get("digit", "!");

	std::string inputString = inputSymbol.toStyledString();
	inputString.at(1) = 0;


	model->setSymbol(inputString);

	Json::Value JsonPaths = value["paths"];

	for (int index = 0; index < JsonPaths.size(); ++index) {
		Entity* path = getSingleEntityFromJSONValue(JsonPaths[index]);
		if (path != NULL) {
			model->addEntity(path);
		}
	}
	return model;

}

Entity* FileManager::getSingleEntityFromJSONValue(Json::Value value)
{
	Entity* path = new PolyLineEntity();

	Json::Value JsonCoordinates = value["coordinates"];

	for (int index = 0; index < JsonCoordinates.size(); index+=2) {
		float coordinateX = JsonCoordinates[index].asFloat();
		float coordinateY;
		if (flippedInput){
			coordinateY = 1.0 - JsonCoordinates[index + 1].asFloat();
		}
		else {
			coordinateY = JsonCoordinates[index + 1].asFloat();
		}
		path->appendPoint(Vec2f(coordinateX, coordinateY));
	}


	return path;

}