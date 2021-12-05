#pragma once

#include "Global.h"
#include "FS.h"
#include "Encoding.h"

#define _USE_MATH_DEFINES 
#include <math.h>

class Chart
{
public:
	Chart() = default;

	void load(const WString& fileName);
	void draw(HDC hdc, const RECT& clientRect);


	bool empty()const;
private: 
	size_t calcMax(size_t value);

protected:
	Chart(const Chart&) = delete;
	Chart& operator=(const Chart&) = delete;

private:
	Vector<size_t> frequencies_;

	const WString xLabel = L"Ранг";
	const WString yLabel = L"Частота";
	
	RECT xLabelRect;
	RECT yLabelRect;

	RECT xMaxLabelRect;
	RECT yMaxLabelRect;

	const size_t pikeWidth_ = 50;
	const size_t simpleWidth_ = 10;

	const int tickCount_ = 10;
};

