#include "stdafx.h"

#include "Renderer.h"
#include "Utils.h"
#include "Exception.h"

using namespace std;

Renderer::Renderer() 
	: _initialized(false), _hasError(false), _errorDescription(), 
	_target(NULL), _originalBitmap(NULL), _finalBitmap(NULL)
{
}

Renderer::~Renderer()
{
	SafeRelease(_finalBitmap);
	SafeRelease(_originalBitmap);
	SafeRelease(_target);
}

struct Rgba
{
	BYTE r, g, b, a;

	Rgba()
	{
	}

	Rgba(BYTE r, BYTE g, BYTE b, BYTE a) : r(r), g(g), b(b), a(a)
	{
	}
};

void Renderer::Init(HWND hWnd)
{
	_initialized = true;

	try
	{
		ID2D1Factory* d2dFactory;
		HR(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2dFactory));

		// Obtain the size of the drawing area.
		RECT rc;
		GetClientRect(hWnd, &rc);

		// Create a Direct2D render target			
		HR(d2dFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(hWnd, D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)),
			&_target));

		IWICImagingFactory *wicFactory;
		HR(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, reinterpret_cast<void **>(&wicFactory)));

		//load image using WIC
		IWICBitmapDecoder *decoder;
		HR(wicFactory->CreateDecoderFromFilename(
			L"cube.png",
			NULL,
			GENERIC_READ,
			WICDecodeMetadataCacheOnLoad,
			&decoder));

		//get the initial frame
		IWICBitmapFrameDecode *frame;
		HR(decoder->GetFrame(0, &frame));
		SafeRelease(decoder);
 
		//format convert to 32bppPBGRA -- which D2D expects
		IWICFormatConverter *converter;
		HR(wicFactory->CreateFormatConverter(&converter));
 
		//initialize the format converter
		HR(converter->Initialize(
			frame,
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			NULL,
			0.f,
			WICBitmapPaletteTypeMedianCut));

		_target->CreateBitmapFromWicBitmap(converter, &_originalBitmap);

		UINT width, height;
		HR(frame->GetSize(&width, &height));
		WICRect rect={0,0,width, height};
		
		vector<Rgba> srcBuf(height*width);
		HR(converter->CopyPixels(&rect, sizeof(Rgba)*width, sizeof(Rgba)*width*height, reinterpret_cast<BYTE*>(srcBuf.data())));

		vector<Rgba> destBuf = Filter(srcBuf, width, height);

		HR(_target->CreateBitmap(
			D2D1::Size(width, height), destBuf.data(), sizeof(Rgba)*width, 
			D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,D2D1_ALPHA_MODE_IGNORE)), 
			&_finalBitmap));

		SafeRelease(frame);
		SafeRelease(converter);
		SafeRelease(wicFactory);
		SafeRelease(d2dFactory);
	}
	catch (BaseException &x)
	{
		_hasError = true;
		_errorDescription = x.GetDescription();
	}
}

void Renderer::Render(HWND hWnd)
{
	if (!_initialized)
		Init(hWnd);
	
	if (_hasError)
	{
		HDC dc = GetDC(hWnd);
		TextOut(dc, 0, 0, _errorDescription.c_str(), _errorDescription.length());
		ReleaseDC(hWnd, dc);
		return;
	}

	_target->BeginDraw();

	auto size = _originalBitmap->GetSize();

	D2D1_RECT_F rect1 = {10,10,10+size.width,10+size.height};
	_target->DrawBitmap(_originalBitmap, rect1);

	D2D1_RECT_F rect2 = {20+size.width,10,20+size.width*2,10+size.height};
	_target->DrawBitmap(_finalBitmap, rect2);

	_target->EndDraw();
}

void Renderer::Resize( UINT width, UINT height )
{
	if (!_initialized)
		return;
	_target->Resize(D2D1::SizeU(width, height));
}

vector<Rgba> Renderer::Filter(const vector<Rgba>& srcBuf, UINT width, UINT height)
{
	if (srcBuf.size() < height*width)
		throw SimpleException(L"Filter: supplied source vector is smaller than height*width");

	vector<Rgba> destBuf(height*width);

	const UINT size = 5;
	const UINT mid = size/2;

	UINT filter[size][size]={
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
					r+=srcBuf[q].r*f;
					g+=srcBuf[q].g*f;
					b+=srcBuf[q].b*f;
					a+=srcBuf[q].a*f;
				}

			destBuf[p] = Rgba(r/sum, g/sum, b/sum, a/sum);
		}

	return destBuf;
}
