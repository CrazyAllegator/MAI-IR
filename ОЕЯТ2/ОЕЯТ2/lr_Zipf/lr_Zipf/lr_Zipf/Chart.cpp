#include "Chart.h"
//-------------------------------------------------------------------------------------------------------------
void deserialize(size_t& value, File &file)
{
	file.read(&value, sizeof(value));
}

void deserialize(WString& wstr, File &file)
{
	size_t size = 0;
	deserialize(size, file);
	wstr.resize(size);

	file.read(const_cast<wchar_t*>(wstr.data()), size * sizeof(WString::value_type));
}

void skipWString(File &file)
{
	size_t size = 0;
	deserialize(size, file);
	file.seek(size * sizeof(WString::value_type));
}

void skipWStrings(File &file)
{
	size_t size = 0;
	deserialize(size, file);

	for (size_t i = 0; i < size; ++i)
		skipWString(file);
}

void skipLocations(size_t locationSize, File &file)
{
	file.seek(locationSize * sizeof(size_t) * 3);  //size_t fileIndex; size_t tokenNumber;  size_t pos;
}
//-------------------------------------------------------------------------------------------------------------
void drawLine(HDC hdc, int x1, int y1, int x2, int y2)
{
	MoveToEx(hdc, x1, y1, NULL);
	LineTo(hdc, x2, y2);
}
//-------------------------------------------------------------------------------------------------------------
void drawPike(HDC hdc, int x1, int y1, int x2, int y2)
{
	double x = x2 - x1;
	double y = y2 - y1;
	double fi = atan2(y, x);

	double alpha1 = fi + M_PI / 12.;
	double alpha2 = fi - M_PI / 12.;

	const double l = 15;

	double lx1 = l * cos(alpha1);
	double ly1 = l * sin(alpha1);
	double lx2 = l * cos(alpha2);
	double ly2 = l * sin(alpha2);

	MoveToEx(hdc,x2 - lx1, y2 - ly1,NULL);
	LineTo(hdc, x2, y2);

	MoveToEx(hdc, x2 - lx2, y2 - ly2, NULL);
	LineTo(hdc, x2, y2);
}
//-------------------------------------------------------------------------------------------------------------
int width(const RECT& rect)
{
	return rect.right - rect.left;
}
//-------------------------------------------------------------------------------------------------------------
int height(const RECT& rect)
{
	return rect.bottom - rect.top;
}
//-------------------------------------------------------------------------------------------------------------
void Chart::load(const WString & fileName)
{
	frequencies_.clear();

	File file;
	file.open(fileName.c_str(), GENERIC_READ, OPEN_EXISTING);

	skipWStrings(file); // fileNames
	skipWStrings(file); // snippets

	size_t tokenCount = 0;
	deserialize(tokenCount, file); // tokenCount
	frequencies_.reserve(tokenCount);

	for (size_t i = 0; i < tokenCount; ++i)
	{
		skipWString(file); // token

		size_t size = 0;
		deserialize(size, file);   // locations size
		skipLocations(size, file); // locations

		// ”пор€дочиваем поубыванию "частотности" токена
		auto it = user::lower_bound(frequencies_.begin(), frequencies_.end(), size,
			[](size_t a, size_t b) { return a > b; });
		frequencies_.insert(it, size);
	}
}

void Chart::draw(HDC hdc, const RECT & clientRect)
{
	if(frequencies_.empty())
		return;

    DrawText(hdc, yLabel.c_str(), yLabel.size(), &yLabelRect, DT_CALCRECT);
	DrawText(hdc, xLabel.c_str(), xLabel.size(), &xLabelRect, DT_CALCRECT);

	SetRect(&yLabelRect, clientRect.left, clientRect.top, 
		                 clientRect.left+width(yLabelRect), clientRect.top+height(yLabelRect));

	SetRect(&xLabelRect, clientRect.right-width(xLabelRect), clientRect.bottom - height(xLabelRect), 
		                 clientRect.right, clientRect.bottom);

	DrawText(hdc, yLabel.c_str(), yLabel.size(), &yLabelRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	DrawText(hdc, xLabel.c_str(), xLabel.size(), &xLabelRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

	RECT rect = clientRect;
	rect.left   += width(yLabelRect);
	rect.bottom -= width(xLabelRect);

	SetDCPenColor(hdc, RGB(0, 0, 0));

	size_t xMax = calcMax(frequencies_.size()); // max rank
	size_t yMax = calcMax(frequencies_[0]);     // max freq

	const String xMaxLabel = uIntToStr(xMax);
	const String yMaxLabel = uIntToStr(yMax);
	DrawTextA(hdc, xMaxLabel.c_str(), xMaxLabel.size(), &xMaxLabelRect, DT_CALCRECT);
	DrawTextA(hdc, yMaxLabel.c_str(), yMaxLabel.size(), &yMaxLabelRect, DT_CALCRECT);

	const POINT center{ rect.left +   width(yMaxLabelRect) + simpleWidth_,
				        rect.bottom - height(xMaxLabelRect) - simpleWidth_ };

	drawLine(hdc, center.x, center.y, rect.right, center.y);
	drawLine(hdc, center.x, center.y, center.x, rect.top);

	drawPike(hdc, center.x, center.y, rect.right, center.y);
	drawPike(hdc, center.x, center.y, center.x, rect.top);
	
	const float kx = float(rect.right - center.x - pikeWidth_) / xMax;
	const size_t valueXStep = xMax / tickCount_;
	for (size_t value=0; value <= xMax; value+= valueXStep)
	{
		POINT  p{ center.x+ kx*value, center.y};
		drawLine(hdc, p.x, p.y - simpleWidth_ / 2, p.x, p.y + simpleWidth_ / 2);

		if (value!=0)
		{
			HGDIOBJ hOldPen = SelectObject(hdc, GetStockObject(DC_PEN));
			SetDCPenColor(hdc, RGB(0, 254, 246));
			drawLine(hdc, p.x, p.y - simpleWidth_ / 2, p.x, rect.top);
			SelectObject(hdc, hOldPen);
		}

		const auto label = uIntToStr(value); 
		RECT labelRect = { p.x - width(xMaxLabelRect) / 2 , p.y + simpleWidth_,
						   p.x + width(xMaxLabelRect) / 2 , p.y + rect.bottom };
		DrawTextA(hdc, label.c_str(), label.size(), &labelRect, DT_CENTER | DT_TOP);
	}

	
	const float ky = float(center.y- rect.top - pikeWidth_) / yMax;
	const size_t valueYStep = yMax / tickCount_;
	for (size_t value = 0; value <= yMax; value += valueYStep)
	{
		POINT  p{  center.x, center.y - ky * value };
		drawLine(hdc, p.x - simpleWidth_ / 2, p.y, p.x + simpleWidth_ / 2, p.y);

		if(value!=0)
		{
		  HGDIOBJ hOldPen = SelectObject(hdc, GetStockObject(DC_PEN));
		  SetDCPenColor(hdc, RGB(0, 254, 246));
		  drawLine(hdc, p.x + simpleWidth_ / 2, p.y, rect.right, p.y);
		  SelectObject(hdc, hOldPen);
		}

		const auto label = uIntToStr(value);
		RECT labelRect = { rect.left,        p.y - height(yMaxLabelRect)/2,
						   p.x - simpleWidth_ ,  p.y + height(yMaxLabelRect)/2 };
		DrawTextA(hdc, label.c_str(), label.size(), &labelRect, DT_RIGHT | DT_VCENTER);
	}

	HGDIOBJ hOldPen = SelectObject(hdc, GetStockObject(DC_PEN));
	SetDCPenColor(hdc, RGB(255, 0, 0));
	for (size_t x = 0; x < frequencies_.size(); ++x)
	{
		POINT p{ center.x + kx * x, center.y - ky * frequencies_[x] };
		if (x == 0)
			MoveToEx(hdc, p.x, p.y, NULL);
		else
			LineTo(hdc, p.x, p.y);
	}
	SelectObject(hdc, hOldPen);
}


bool Chart::empty() const
{
	return frequencies_.empty();
}


size_t Chart::calcMax(size_t value)
{
	for (size_t i = 10; ; i *= 10)
	{
		if (value / i == 0)
		{
			i /= 10;
			double c;
			double o = modf(double(value) /i, &c);
			c = (o < 0.5) ? c+0.5: c+1;
			return c * i;
		}
	}
	return 0;
}
