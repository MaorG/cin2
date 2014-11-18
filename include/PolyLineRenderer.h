#pragma once
#include "AppRenderer.h"

class PolyLineRenderer : public AppRenderer {
	Color currentColor;
public:
	PolyLineRenderer();
	~PolyLineRenderer();
	void draw();
	void drawPolyLineEntity(PolyLineEntity * polyLineEntity);
};