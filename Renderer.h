#pragma once

#include "Rgba.h"

class Renderer
{
	ID2D1HwndRenderTarget* _target;			
	ID2D1Bitmap *_originalBitmap, *_finalBitmap;
	std::wstring _errorDescription;
	bool _initialized;
	bool _hasError;

	void Init(HWND hWnd);
public:
	Renderer();
	~Renderer();
	void Render(HWND hWnd);
	void Resize( UINT width, UINT height );
};