#ifndef WINERRORS_H
#define WINERRORS_H

// Returns error description for Windows errors
// Parameters:
//	err [in] - error number
inline std::string winerror(DWORD err = GetLastError())
{
	char buf[256];
	::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, 0, buf, sizeof(buf), NULL);
	return std::string(buf);
}

#ifdef NDEBUG

#define assert_winerror(exp) (void)(exp)

#else

// Pops up assertion failed window if exp false
// message contains exp string, windows error description, source file name, source line number
#define assert_winerror(exp) (void)((exp) || (_assert((void*)((std::string(#exp) += winerror()).c_str()), __FILE__, __LINE__),0))

#endif

#endif	// WINERRORS_H