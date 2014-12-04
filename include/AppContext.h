#pragma once
#include "common.h"
#include "Model.h"
#include "AppWindow.h"

class AppContext
{
protected:
	std::vector<AppWindow*> * windows;


public:

	void AddWindow(AppWindow * window) {
		windows->push_back(window);
	};

	vector<AppWindow*> *getWindows() {
		return windows;
	}

	AppContext() {
		windows = new std::vector<AppWindow*>();
	};

	~AppContext() {
		windows->clear();
		delete windows;
	};


	void putModelInWindowByIndex(int windowIndex, Model * model) {
		if (windowIndex > 0 && windowIndex < windows->size()) {
			windows->at(windowIndex)->setModel(model);
		}
	}
};

