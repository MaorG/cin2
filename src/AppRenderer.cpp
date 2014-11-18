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

void AppRenderer::setModel(Model *aModel)
{
	model = aModel;
}

AppRenderer::~AppRenderer()
{
}

void AppRenderer::draw() {
	if (model->getDigit() != ' ') {

		std::ostringstream oss(2);

		oss << "digit: " << model->getDigit() << std::ends;
		std::string word;
		
		Vec3f temp = transform.transformVec(Vec3f(0, 1.0, 1.0));
		Vec2f BL = Vec2f(temp.x, temp.y);
		cinder::Font textfont = Font("Courier New", 20);

		gl::drawString(oss.str(), BL, ColorA(1.0, 1.0, 1.0, 1.1), textfont);
	}
}
