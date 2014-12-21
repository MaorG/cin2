#pragma once
#include "Entity.h"


class Model
{
protected:
	std::vector<Entity*> *entities;
	char digit;
public:

	void normalizeBoundingBox();
	ci::Rectf getBoundingBox();
	char getDigit() { return digit; };
	void setDigit(char aDigit) { digit = aDigit; };
	std::vector<Entity*> *getEntities() { return entities; };
	int size();
	void addEntity(Entity* entity);
	void popEntity();
	Entity* getEntityByIndex(int index);
	void clear();
	Model();
	~Model();

};
