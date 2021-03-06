#include "PolyLineEntity.h"

using namespace ci;
using namespace ci::gl;

PolyLineEntity::PolyLineEntity()
{
	color = ci::Color(1.0, 0.0, 0.0);
	object = new PolyLine2f();
}

PolyLineEntity::~PolyLineEntity()
{
	delete object;
}

void PolyLineEntity::move(ci::Vec2f movement)
{
	object->offset(movement);
}

void PolyLineEntity::scaleFromCenter(ci::Vec2f scale, ci::Vec2f center)
{
	object->scale(scale, center);
}

ci::Rectf PolyLineEntity::getBoundingBox()
{
	Rectf BB = Rectf(object->getPoints());
	return BB;
}

PolyLineEntity * PolyLineEntity::clone() {
	PolyLineEntity * cloned = new PolyLineEntity();
	PolyLine2f * clonedObject = new PolyLine2f();
	*clonedObject = *object;
	cloned->setObject(clonedObject);
	return cloned;
}

void PolyLineEntity::setObject(PolyLine2f * polyLine)
{
	delete PolyLineEntity::object;
	PolyLineEntity::object = polyLine;
}
PolyLine2f * PolyLineEntity::getObject()
{
	return object;
}

void PolyLineEntity::draw()
{
}

void PolyLineEntity::appendPoint(Vec2f newPoint)
{
	object -> push_back(Vec2f(newPoint.x, newPoint.y));
}

void PolyLineEntity::concat(PolyLineEntity* tail)
{
	PolyLine2f * tailObject = tail->getObject();

	for (std::vector<Vec2f>::iterator it = tailObject->begin(); it != tailObject->end(); ++it) {
		object->push_back(Vec2f((*it).x, (*it).y));
	}

}