#include "AppRenderer.h"
#include <sstream>
#include <cinder/Font.h>

AppRenderer::AppRenderer()
{
	transform = Matrix33f();
}


void AppRenderer::setTransform(Matrix33f aTransform)
{
	transform = aTransform;
}

AppRenderer::~AppRenderer()
{
}

void AppRenderer::draw(Matrix33f transform, Model * model) {

	//if (model->getSymbol() != ' ') {

		//std::ostringstream oss(2);

		//oss << "digit: " << model->getSymbol() << std::ends;

		cinder::Font textfont = Font("Courier New", 20);


		//Vec3f temp = transform.transformVec(Vec3f(0, 1.0, 1.0));
		//Vec2f BL = Vec2f(temp.x, temp.y);
		//gl::drawString(oss.str(), BL, ColorA(1.0, 1.0, 1.0, 1.1), textfont);
		Vec3f temp = transform.transformVec(Vec3f(0, 1.0, 1.0));
		Vec2f BL = Vec2f(temp.x, temp.y);
		gl::drawString(model->getSymbol(), BL, ColorA(1.0, 1.0, 1.0, 1.1), textfont);
	//}
}
