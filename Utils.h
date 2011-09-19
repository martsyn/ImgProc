template<class Interface> inline void SafeRelease(Interface *&ppInterfaceToRelease)
{
    if (ppInterfaceToRelease != NULL)
    {
        (ppInterfaceToRelease)->Release();
        (ppInterfaceToRelease) = NULL;
    }
}

#define IFT(x) {HRESULT hr = (x);if (FAILED(hr)){throw ComException()}}