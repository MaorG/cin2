#pragma once

#include "common.h"

#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "AppRenderer.h"

class AppWindow
{
protected:
	Rectf rect;
	Matrix33f *transform;
	Matrix33f *inverseTransform;
	vector<AppRenderer *> renderers;
	Model *model;
public:
	void addRenderer(AppRenderer * renderer);
	void setRect(Rectf rect);
	void setup();
	void setModel(Model * aModel);
	Model * getModel() { return model; };
	Matrix33f * getTransformMatrix();

	virtual void mouseDown(MouseEvent event);
	virtual void mouseDrag(MouseEvent event);
	void update();
	void draw();


	AppWindow();
	~AppWindow();
};

