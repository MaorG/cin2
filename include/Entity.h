#pragma once

#include "common.h"

class Entity
{
protected:
	char digit;
public:

	char getDigit() { return digit; };
	void setDigit(char aDigit) { digit = aDigit; };

	virtual ~Entity(){};
	virtual void draw()=0;
    virtual void appendPoint(ci::Vec2f newPoint)=0;
	virtual bool isPolyLineEntity() { return false; };
	virtual Entity* toPolyLineEntity() { return NULL; };
};
