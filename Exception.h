#pragma once

class BaseException
{
public:
	virtual std::wstring GetDescription() const = 0;
	virtual ~BaseException();
};

class WinException : public BaseException
{
	const wchar_t* operation;
	HRESULT code;
public:
	WinException(HRESULT hr, const wchar_t* operation) : code(hr), operation(operation)
	{
	}

	WinException(const wchar_t* operation) : operation(operation)
	{
		code = static_cast<HRESULT>(GetLastError());
	}

	virtual std::wstring GetDescription() const;
};

template <class T> inline T WinCall(T result, const wchar_t* operation)
{
	if (!result)
		throw WinException(operation);
	return result;
}
#define WC(x) WinCall(x, L#x)

inline void HRCall(HRESULT hr, const wchar_t* operation)
{
	if (FAILED(hr))
		throw WinException(hr, operation);
}
#define HR(x) HRCall(x, L#x)

class SimpleException : public BaseException
{
	std::wstring description;
public:
	SimpleException(const wchar_t* description) : description(description) {}
	virtual std::wstring GetDescription() const;
};