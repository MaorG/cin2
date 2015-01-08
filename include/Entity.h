#pragma once

#include "common.h"

class Entity
{
protected:
	char digit;
public:

	virtual ci::Rectf getBoundingBox() = 0;
	virtual void scaleFromCenter(ci::Vec2f scale, ci::Vec2f center) = 0;
	virtual void move(ci::Vec2f movement) = 0;
	char getDigit() { return digit; };
	void setDigit(char aDigit) { digit = aDigit; };
	virtual Entity * clone() { return NULL; };
	virtual void setColor(ci::Color color) = 0;

	virtual ~Entity(){};
	virtual void draw()=0;
    virtual void appendPoint(ci::Vec2f newPoint)=0;
	virtual bool isPolyLineEntity() { return false; };
	virtual Entity* toPolyLineEntity() { return NULL; };
};
