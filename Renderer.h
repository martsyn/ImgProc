struct Rgba;

class Renderer
{
	bool _initialized;
	ID2D1HwndRenderTarget* _target;			
	ID2D1Bitmap *_originalBitmap, *_finalBitmap;
	bool _hasError;
	std::wstring _errorDescription;

	void Init(HWND hWnd);
	void Filter(const Rgba * buf, Rgba * destBuf, UINT width, UINT height);
public:
	Renderer();
	~Renderer();
	void Render(HWND hWnd);
	void Resize( UINT width, UINT height );
};