// Just the stuff we need...
//
#ifndef __NTDLL_H__
#define __NTDLL_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _NTDDK_
#error This header cannot be compiled together with NTDDK
#endif


#ifndef _NTDLL_SELF_                            // Auto-insert the library
#pragma comment(lib, "Ntdll.lib")
#endif
		
	#define NtCurrentProcess() ( (HANDLE)(LONG_PTR) -1 )  

	__declspec(dllimport)
		unsigned long
		_cdecl
		DbgPrint(
			PCH Format,
			...
			);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __NTDLL_H__