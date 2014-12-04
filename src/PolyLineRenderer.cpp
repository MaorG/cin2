#include "PolyLineRenderer.h"
#include "PolyLineProcessor.h"

PolyLineRenderer::PolyLineRenderer():
AppRenderer()
{
}

PolyLineRenderer::~PolyLineRenderer()
{
}


void PolyLineRenderer::draw(Matrix33f transform, Model * model)
{
	if (model == nullptr) {
		return;
	}

	AppRenderer::draw(transform, model);
	setTransform(transform);

	std::vector<Entity*> * entities = model->getEntities();
	for (std::vector<Entity*>::iterator it = entities->begin(); it != entities->end(); ++it) {
		if ((*it)->isPolyLineEntity()) {
			drawPolyLineEntity((PolyLineEntity*)*it);
		}
	}
}

void PolyLineRenderer::drawPolyLineEntity(PolyLineEntity* polyLineEntity)
{

	PolyLine2f * object = polyLineEntity->getObject();

	if (object->size() < 2 ) {
		return;
	}
	gl::color(polyLineEntity->getColor());
	PolyLine<Vec2f> * polyLine = new PolyLine<Vec2f>(object->getPoints());

	for (std::vector<Vec2f>::iterator it = polyLine->begin(); it != polyLine->end(); ++it) {
		Vec3f temp = transform.transformVec(Vec3f(*it, 1.0));
		*it = Vec2f(temp.x, temp.y);
	}

	float maxLength = 0;

	for (std::vector<Vec2f>::iterator it = polyLine->begin(); it != polyLine->end() - 1; ++it) {
		float dist = (Vec2f(it->x, it->y) - Vec2f((it + 1)->x, (it + 1)->y)).length();
		if (dist > maxLength) {
			maxLength = dist;
		}
	}

	for (std::vector<Vec2f>::iterator it = polyLine->begin(); it != polyLine->end(); ++it) {
		Vec2f point;

		gl::drawSolidCircle(Vec2f(it->x, it->y), 2);
		
	}

	for (std::vector<Vec2f>::iterator it = polyLine->begin(); it != polyLine->end() - 1; ++it) {
		Vec2f point;
		float dist = (Vec2f(it->x, it->y) - Vec2f((it + 1)->x, (it + 1)->y)).length();
		if (dist > 0 && maxLength > 0) {
			gl::drawLine(Vec2f(it->x, it->y), Vec2f((it + 1)->x, (it + 1)->y));
		}
	}

	// extra info
	//gl::color(Color(1.0, 0.0, 0.0));
	//Vec3f temp;

	//Vec2f CoG = PolyLineProcessor::getCenterOfGravity(polyLineEntity);
	//temp = transform.transformVec(Vec3f(CoG, 1.0));
	//CoG = Vec2f(temp.x, temp.y);
	//gl::drawSolidCircle(CoG, 2);

	//Vec2f farthest = PolyLineProcessor::getFarthestFromCenter(polyLineEntity);
	//temp = transform.transformVec(Vec3f(farthest, 1.0));
	//farthest = Vec2f(temp.x, temp.y);
	//gl::drawSolidCircle(farthest, 2);

	delete polyLine;
}
