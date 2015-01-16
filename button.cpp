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

#include "button.h"

TextLabel::TextLabel(int x, int y, std::string text, float r, float g, float b)
{
	_text = text;
	_x = x;
	_y = y;
	_red = r;
	_green = g;
	_blue = b;
	_alpha = 1.0f;
	_visible = true;
}

TextLabel::~TextLabel()
{

}

std::string& TextLabel::getText()
{
	return _text;
}

void TextLabel::getPosition(int* x, int* y)
{
	*x = _x;
	*y = _y;
}

void TextLabel::getColors(float* r, float* g, float* b, float* a)
{
	*r = _red;
	*g = _green;
	*b = _blue;
	*a = _alpha;
}

void TextLabel::setText(std::string text)
{
	_text = text;
}

void TextLabel::setPosition(int x, int y)
{
	_x = x;
	_y = y;
}

void TextLabel::setPositionWithOffset(int x, int y, int xOff, int yOff)
{
	setPosition(x, y);
	_x += xOff;
	_y += yOff;
}

void TextLabel::update(unsigned int dT) {}

bool TextLabel::isVisible()
{
	return _visible;
}

void TextLabel::setVisible(bool b)
{
	_visible = b;
}

FloatingMessage::FloatingMessage(int x, int y, std::string text, float r, float g, float b) : TextLabel(x, y, text, r, g, b)
{
	_timeRemaining = 0;
	_setTime = 0;

}

FloatingMessage::~FloatingMessage()
{
}

void FloatingMessage::update(unsigned int dT)
{
	if (_visible)
	{
		_timeRemaining -= dT;

		int diff = _setTime - _timeRemaining;

		if (diff < 250)
		{
			_alpha = ((float)(diff) / 250);
		}

		if (_timeRemaining < 500)
		{
			_alpha = (float)(_timeRemaining) / 500;
		}
		if (_timeRemaining <= 0)
		{
			setVisible(false);
			_timeRemaining = 0;
			_setTime = 0;
		}
	}
}

void FloatingMessage::init(int x, int y, std::string text, unsigned int timeRemaining)
{
	setPosition(x, y);
	setText(text);
	setTimeRemaining(timeRemaining);
	_alpha = 0.0f;
}

void FloatingMessage::setTimeRemaining(unsigned int timeRemaining)
{
	_setTime = timeRemaining;
	_timeRemaining = timeRemaining;
	setVisible(_timeRemaining > 0);
}

Button::Button(int x, int y, int w, int h)
{
	_x = x;
	_y = y;
	_w = w;
	_h = h;

	_highlighted = false;
	_clickable = true;
}

Button::Button(int x, int y, int w, int h, bool clickable)
{
	_x = x;
	_y = y;
	_w = w;
	_h = h;

	_highlighted = false;
	_clickable = clickable;
}

Button::~Button()
{
}

bool Button::isMouseIntersecting(int mx, int my)
{
	return (mx >= _x && mx <= _x + _w && my >= _y && my <= _y + _h);
}

bool Button::isHighlighted()
{
	return _highlighted;
}

void Button::setHighlighted(bool b)
{
	_highlighted = b;
}

int Button::getX()
{
	return _x;
}

int Button::getY()
{
	return _y;
}

int Button::getW()
{
	return _w;
}

int Button::getH()
{
	return _h;
}

void Button::setW(int w)
{
	_w = w;
}

void Button::setPosition(int x, int y)
{
	_x = x;
	_y = y;
}

void Button::setPositionWithOffset(int x, int y, int xOff, int yOff)
{
	setPosition(x, y);
	_x += xOff;
	_y += yOff;
}

bool Button::isClickable()
{
	return _clickable;
}

//Text button functions

TextButton::TextButton(int x, int y, int w, int h, std::string text) : Button(x, y, w, h)
{
	_text = text;
	_red = 1.0f;
	_green = 1.0f;
	_blue = 1.0f;
}

TextButton::TextButton(int x, int y, int w, int h, std::string text, float r, float g, float b) : Button(x, y, w, h)
{
	_text = text;

	_red = r;
	_green = g;
	_blue = b;
}

TextButton::~TextButton()
{
}

void TextButton::changeColor(float r, float g, float b)
{
	_red = r;
	_green = g;
	_blue = b;
}

const char* TextButton::getText()
{
	return _text.c_str();
}

void TextButton::setText(std::string text)
{
	_text = text;
	_w = (text.length() + 2) * 16;
}

void TextButton::getColors(float* r, float* g, float* b)
{
	*r = _red;
	*g = _green;
	*b = _blue;
}

void TextButton::handleMouseIntersection(int mx, int my)
{
	if (isMouseIntersecting(mx, my))
	{
		changeColor(0, 1, 0);
	}
	else
	{
		changeColor(1, 1, 1);
	}
}

ImageButton::ImageButton(int x, int y, int w, int h, unsigned int index) : Button(x, y, w, h)
{
	_defaultIndex = index;
	_selectedIndex = index;
	_currIndex = index;
}

ImageButton::ImageButton(int x, int y, int w, int h, bool clickable, unsigned int index) : Button(x, y, w, h, clickable)
{
	_defaultIndex = index;
	_selectedIndex = index;
	_currIndex = index;
}

ImageButton::ImageButton(int x, int y, int w, int h, unsigned int index, unsigned int selectedIndex) : Button(x, y, w, h)
{
	_defaultIndex = index;
	_selectedIndex = selectedIndex;
	_currIndex = index;
}

ImageButton::~ImageButton()
{

}

unsigned int ImageButton::getSpriteIndex()
{
	return _currIndex;
}

void ImageButton::handleMouseIntersection(int mx, int my)
{
	if (isMouseIntersecting(mx, my))
	{
		_currIndex = _selectedIndex;
	}
	else
	{
		_currIndex = _defaultIndex;
	}
}

void ImageButton::changeSprites(unsigned int index, unsigned int selectedIndex)
{
	_currIndex = _defaultIndex = index;
	_selectedIndex = selectedIndex;
}