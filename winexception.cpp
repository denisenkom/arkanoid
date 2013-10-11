#include "stdafx.h"
#pragma hdrstop

#include "winexception.h"

WINException::WINException(const char * msg) throw()
	: message(msg), errid(::GetLastError())
{
}

WINException::~WINException() throw()
{

}

const char *WINException::what() const throw()
{
	char a[256];
	FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, 0, errid, 0, a, 256, 0);
	buffer = message;
	buffer += a;
	return buffer.c_str();
}
