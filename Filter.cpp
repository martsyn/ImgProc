#include "stdafx.h"

#include "Filter.h"
#include "Exception.h"

using namespace std;

vector<Rgba> Filter(const vector<Rgba>& srcBuf, UINT width, UINT height, bool includeAlpha) throw(...)
{
	if (srcBuf.size() != height*width)
		throw SimpleException(L"Filter: supplied source vector size doesn't match height*width");

	vector<Rgba> destBuf(height*width);

	const UINT size = 5;
	const UINT mid = size/2;

	BYTE filter[size][size]={
		{ 2, 4, 5, 4, 2, },
		{ 4, 9,12, 9, 4, },
		{ 5,12,15,12, 5, },
		{ 4, 9,12, 9, 4, },
		{ 2, 4, 5, 4, 2, },
	};

	for (UINT y = 0; y < height; ++y)
		for (UINT x = 0; x < width; ++x)
		{
			UINT r = 0, g = 0, b = 0, a = 0;
			UINT p = y*width + x;
			UINT sum = 0;

			for (UINT i = y > mid ? 0 : mid - y; i < (height - y > mid ? size : height - y + mid); ++i)
				for (UINT j = x > mid ? 0 : mid - x; j < (width - x > mid ? size : width - x + mid); ++j)
				{
					BYTE f = filter[i][j];
					sum += f;
					UINT q = p + (i - mid)*width + j - mid;
					r += srcBuf[q].r*f;
					g += srcBuf[q].g*f;
					b += srcBuf[q].b*f;
					if (includeAlpha)
						a += srcBuf[q].a*f;
				}

			destBuf[p] = Rgba(
				static_cast<BYTE>(r/sum), 
				static_cast<BYTE>(g/sum), 
				static_cast<BYTE>(b/sum), 
				includeAlpha ? static_cast<BYTE>(a/sum) : 0xff);
		}

	return destBuf;
}
