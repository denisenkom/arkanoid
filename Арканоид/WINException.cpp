// WINException.cpp: implementation of the WINException class.
//
//////////////////////////////////////////////////////////////////////

#include <exception>
#include <string>
#include <windows.h>
#include "WINException.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

WINException::WINException(const char * msg)
	: message(msg), errid(::GetLastError())
{
}

WINException::~WINException()
{

}

const char *WINException::what() const
{
	char a[256];
	FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, 0, errid, 0, a, 256, 0);
	buffer = message;
	buffer += a;
	return buffer.c_str();
}