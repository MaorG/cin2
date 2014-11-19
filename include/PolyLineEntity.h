#pragma once
#include "Entity.h"
#include "common.h"

class Entity;
class PolyLineEntity : public Entity
{
	ci::PolyLine2f * object;

public:
	PolyLineEntity();
	~PolyLineEntity();

	void PolyLineEntity::move(ci::Vec2f movement);
	void scaleFromCenter(ci::Vec2f scale, ci::Vec2f center);
	ci::Rectf getBoundingBox();
	PolyLineEntity * clone();
	bool isPolyLineEntity() { return true; };
	PolyLineEntity * toPolyLineEntity() { return this; };
	void setObject(ci::PolyLine2f * polyLine);
	ci::PolyLine2f * getObject();
	void draw();
	void appendPoint(ci::Vec2f newPoint);
	void concat(PolyLineEntity* tail);
};