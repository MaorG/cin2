#pragma once
#include "Entity.h"


class Model
{
protected:
	std::vector<Entity*> *entities;
	//char digit; // todo - lose digit, move to symbol
	std::string symbol;

public:

	void normalizeBoundingBox();
	ci::Rectf getBoundingBox();
	//char getDigit() { return digit; };
	//void setDigit(char aDigit) { digit = aDigit; };

	std::string getSymbol() { return symbol; };
	void setSymbol(std::string aSymbol) { symbol = aSymbol; };

	std::vector<Entity*> *getEntities() { return entities; };
	int size();
	void addEntity(Entity* entity);
	void popEntity();
	Entity* getEntityByIndex(int index);
	void clear();
	Model();
	~Model();

};
