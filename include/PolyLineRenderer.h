#pragma once
#include "AppRenderer.h"

class PolyLineRenderer : public AppRenderer {
	Color currentColor;
public:
	PolyLineRenderer();
	~PolyLineRenderer();
	void draw(Model * model);
	void drawPolyLineEntity(PolyLineEntity * polyLineEntity);
};