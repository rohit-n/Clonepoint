/*
Copyright 2013-2015 Rohit Nirmal

This file is part of Clonepoint.

Clonepoint is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Clonepoint is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Clonepoint.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef BUTTON_H
#define BUTTON_H
#include <string>
#include <cstdio>

class TextLabel
{
public:
	TextLabel(int x, int y, std::string text, float r, float g, float b);
	~TextLabel();
	std::string& getText();
	virtual void update(unsigned int dT);
	bool isMouseIntersecting(int mx, int my);
	void setText(std::string text);
	void getPosition(int* x, int* y);
	void getColors(float* r, float* g, float* b, float* a);
	void setPosition(int x, int y);
	void setPositionWithOffset(int x, int y, int xOff, int yOff);
	bool isVisible();
	void setVisible(bool b);
protected:
	std::string _text;
	float _red, _green, _blue, _alpha;
	int _x, _y;
	bool _visible;
};

class FloatingMessage : public TextLabel
{
public:
	FloatingMessage(int x, int y, std::string text, float r, float g, float b);
	~FloatingMessage();
	void update(unsigned int dT);
	void init(int x, int y, std::string text, unsigned int timeRemaining);
	void setTimeRemaining(unsigned int timeRemaining);
private:
	int _timeRemaining;
	int _setTime;
};

class Button
{
public:
	Button(int x, int y, int w, int h);
	Button(int x, int y, int w, int h, bool clickable);
	virtual ~Button();
	bool isMouseIntersecting(int mx, int my);
	bool isHighlighted();
	void setHighlighted(bool b);
	virtual void handleMouseIntersection(int mx, int my) = 0;
	int getX();
	int getY();
	int getW();
	int getH();
	void setW(int w);
	void setPosition(int x, int y);
	void setPositionWithOffset(int x, int y, int xOff, int yOff);
	bool isClickable();
protected:
	int _x, _y;
	int _w, _h;
	bool _highlighted;
	bool _clickable;
};

class TextButton : public Button
{
public:
	TextButton(int x, int y, int w, int h, std::string text);
	TextButton(int x, int y, int w, int h, std::string text, float r, float g, float b);
	~TextButton();
	void changeColor(float r, float g, float b);
	const char* getText();
	void setText(std::string text);
	void getColors(float* r, float* g, float* b);
	void handleMouseIntersection(int mx, int my);
private:
	std::string _text;
	float _red, _green, _blue;
};

class ImageButton : public Button
{
public:
	ImageButton(int x, int y, int w, int h, unsigned int index);
	ImageButton(int x, int y, int w, int h, bool clickable, unsigned int index);
	ImageButton(int x, int y, int w, int h, unsigned int index, unsigned int selectedIndex);
	~ImageButton();
	unsigned int getSpriteIndex();
	void handleMouseIntersection(int mx, int my);
	void changeSprites(unsigned int index, unsigned int selectedIndex);
private:
	unsigned int _currIndex;
	unsigned int _defaultIndex;
	unsigned int _selectedIndex;
};

#endif