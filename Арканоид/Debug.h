#if !defined DEBUG_H_INCLUDED && defined(_DEBUG)
#define DEBUG_H_INCLUDED

#include <stdexcept>

/*#define ASSERT(x, msg) if (!(x)) throw DebugException((msg), __FILE__, __LINE__)

class DebugException : public std::exception {
public:
	DebugException(const char* mes, const char* src_file, int line);
};*/

#endif
