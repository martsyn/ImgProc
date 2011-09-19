#include "stdafx.h"
#include "Exception.h"

using namespace std;

BaseException::~BaseException()
{
}

wstring WinException::GetDescription() const
{
	wostringstream result;
	result << operation << " failed: ";

	wchar_t* msg;
	if (FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		0, code, 0, reinterpret_cast<wchar_t*>(&msg), 0, NULL))
	{
		result << msg;
		LocalFree(msg);
	}
	else
		result << "Unknown error 0x" << hex << code;

	return result.str();
}


wstring SimpleException::GetDescription() const
{
	return description;
}