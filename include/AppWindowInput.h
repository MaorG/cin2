#pragma once
#include "AppWindow.h"

class AppWindowInput : public AppWindow {
public:
	virtual void mouseDown(MouseEvent event);
	virtual void mouseDrag(MouseEvent event);

};
