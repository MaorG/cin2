#pragma once
#include "json/json.h"
#include "common.h"
#include "Model.h"
#include "Entity.h"
#include "PolyLineEntity.h"


#include <fstream>
#include <sstream>

class FileManager
{
protected:
	Entity* getSingleEntityFromJSONValue(Json::Value value);
	Model* getSingleDigitFromJSONValue(Json::Value value);
	std::vector<Model*>* getDigitsFromJSONValue(Json::Value value);
	PolyLineEntity * parsePolyLineEntity(std::istringstream &iss);

	Json::Value writeDigitToJSONValue(Model* model);
	Json::Value writeEntityToJSONValue(Entity* entity);

	Entity * parseEntity(std::istringstream &iss);
	void closeBank();
	std::fstream f;
	int locationInFile;

public:
	std::vector<Model*>* getDigitsFromJSONFile(std::string filename);
	void setBank(std::string name);
	void writeToBank(Model *model);
	Model * pullFromBank();
	FileManager();
	~FileManager();

	void writeDigitToJSONFile(Model* model, std::string filename);
};

