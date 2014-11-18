#include "Model.h"

Model::Model() {
	entities = new std::vector<Entity*>();
	digit = ' ';
};

Model::~Model() { 
	entities->clear(); 
	delete entities; 
};

int Model::size() 
{ 
	return entities->size(); 
}

void Model::addEntity(Entity* entity) { 
	entities->push_back(entity); 
}

void  Model::popEntity() {
	entities->pop_back();
}

Entity* Model::getEntityByIndex(int index) {
	return entities->at(index);
};

void Model::clear()
{
	entities->clear();
}

