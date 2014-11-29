#pragma once
#include "AppRenderer.h"

class PolyLineRenderer : public AppRenderer {
	Color currentColor;
public:
	PolyLineRenderer();
	~PolyLineRenderer();
	void draw(Matrix33f aTransform, Model * model);
	void drawPolyLineEntity(PolyLineEntity * polyLineEntity);
};