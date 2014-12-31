#include "Model.h"
#include <math.h>

Model::Model() {
	entities = new std::vector<Entity*>();
	symbol = "";
};

Model::~Model() { 
	entities->clear(); 
	delete entities; 
};

void Model::normalizeBoundingBox()
{
	ci::Rectf boundingBox = getBoundingBox();
	if (boundingBox.getWidth() * boundingBox.getHeight() == 0.0f) {
		return;
	}

	ci::Rectf normalBoundingBox = ci::Rectf(0, 0, 1, 1);

	float width = fmax (boundingBox.getWidth(), 0.3);
	float height = fmax (boundingBox.getHeight(), 0.3);

	float scaleX = fmax(boundingBox.getWidth(), 0.3);
	float scaleY = fmax(boundingBox.getHeight(), 0.3);

	ci::Vec2f center = boundingBox.getCenter();
	ci::Vec2f scale = ci::Vec2f(0.7f / width, 1.0f / height);

	for (std::vector<Entity*>::iterator it = entities->begin(); it != entities->end(); it++) {
		Entity *entity = *it;
		(*it)->scaleFromCenter(scale, center);
		(*it)->move(ci::Vec2f(0.5 - center.x, 0.5 - center.y));
	}

}

ci::Rectf Model::getBoundingBox() {

	if (entities->size() == 0) {
		return ci::Rectf(0, 0, 0, 0);
	}

	ci::Rectf BB = (*entities->begin())->getBoundingBox();
	
	for (std::vector<Entity*>::iterator it = entities->begin()+1; it != entities->end(); it++) {
		Entity *entity = *it;
		ci::Rectf entityBB = entity->getBoundingBox();
		BB.include(entityBB);		
	}
	return BB;
}


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

