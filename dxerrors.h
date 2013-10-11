// DirectX errors handling helper
#ifndef DXERRORS_H_INCLUDED
#define DXERRORS_H_INCLUDED

#include <assert.h>

// convert HRESULT to string
const char * hres2str(HRESULT hres);

#ifdef NDEBUG

#define assert_hres(exp) (void)(exp)

#else

// HRESULT assertion, if failed then pops message including additional HRESULT information
#define assert_hres(exp) {HRESULT _hres = exp; (void)(SUCCEEDED(_hres) || (_assert((void*)((std::string(#exp) += hres2str(_hres)).c_str()), __FILE__, __LINE__), 0));}

#endif

#endif	// DXERRORS_H_INCLUDED