#include "PolyLineEntity.h"

using namespace ci;
using namespace ci::gl;

PolyLineEntity::PolyLineEntity()
{
	object = new PolyLine2f();
}

PolyLineEntity::~PolyLineEntity()
{
	delete object;
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