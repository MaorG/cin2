#include "AppWindowInput.h"

void AppWindowInput::mouseDown(MouseEvent event)
{


	if (rect.contains(event.getPos())) {
		Entity* newPolyLine = new PolyLineEntity();

		model->addEntity(newPolyLine);
	}


}

void AppWindowInput::mouseDrag(MouseEvent event)
{

	if (model->size() == 0) {
		return;
	}
	Entity* activePolyLineEntity = model->getEntityByIndex(model->size()-1);

	if (rect.contains(event.getPos())) {

		Vec3f pos = Vec3f(event.getPos(), 0);

		pos = inverseTransform -> transformVec(pos);

		activePolyLineEntity->appendPoint(Vec2f(pos.x, pos.y));
	}
}