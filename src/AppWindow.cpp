#include "AppWindow.h"

using namespace ci;


void AppWindow::addRenderer(AppRenderer * renderer)
{
	renderer->setTransform(*transform);
	renderers.push_back(renderer);
}

void AppWindow::setModel(Model *aModel)
{
	//for (std::vector<AppRenderer*>::iterator it = renderers.begin(); it != renderers.end(); ++it) {
	//	(*it)->setModel(aModel);
	//}
	model = aModel;
}

void AppWindow::setRect(Rectf aRect)
{
	rect = aRect;

	Matrix33f translateToWindow = Matrix33f(
		1, 0, 0,
		0, 1, 0,
		aRect.x1, aRect.y1, 1
		);
	Matrix33f scaleToWindow = Matrix33f(
		aRect.getWidth(), 0, 0,
		0, aRect.getHeight(), 0,
		0, 0, 1
		);
	transform = new Matrix33f(translateToWindow * scaleToWindow);

	inverseTransform = new Matrix33f(*transform);
	inverseTransform->invert();
}

Matrix33f * AppWindow::getTransformMatrix()
{
	return transform;
}

void AppWindow::mouseDown(MouseEvent event)
{

}

void AppWindow::mouseDrag(MouseEvent event)
{
}


void AppWindow::setup()
{
}

void AppWindow::update()
{

}
void AppWindow::draw()
{
	// clear out the window with black

	gl::color(Color(1.0, 1.0, 1.0));
	gl::drawSolidRect(Rectf(rect.x1, rect.y1, rect.x2, rect.y2));

	for (std::vector<AppRenderer*>::iterator it = renderers.begin(); it != renderers.end(); ++it) {
		(*it)->draw(model);
	}
		


}

AppWindow::AppWindow()
{
}

AppWindow::~AppWindow()
{
	delete transform;
}
