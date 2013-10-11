// WINException.h: interface for the WINException class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINEXCEPTION_H__C0DA31AF_A975_4781_81BE_523F31311E02__INCLUDED_)
#define AFX_WINEXCEPTION_H__C0DA31AF_A975_4781_81BE_523F31311E02__INCLUDED_

class WINException : public std::exception  
{
	mutable std::string buffer;
	std::string message;
public:
	DWORD errid;
	WINException(const char* msg) throw();
	virtual const char *what() const throw();
	virtual ~WINException() throw();
};

#endif // !defined(AFX_WINEXCEPTION_H__C0DA31AF_A975_4781_81BE_523F31311E02__INCLUDED_)
