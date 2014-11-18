#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "Entity.h"
#include "Model.h"
#include "PolyLineEntity.h"
#include <cinder/PolyLine.h>

using namespace ci;
using namespace ci::app;
using namespace std;

class AppRenderer {
protected:
	void drawPolyLineEntity(PolyLineEntity * polyLineEntity);
	Matrix33f transform;
public:
	AppRenderer();
	~AppRenderer();
	void setTransform(Matrix33f aTransform);
	virtual void draw(Model * model);


};