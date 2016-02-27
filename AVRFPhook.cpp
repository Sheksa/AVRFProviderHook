#include <Windows.h>
#include "ntdll.h"

#define DLL_PROCESS_VERIFIER 4

typedef VOID(NTAPI * RTL_VERIFIER_DLL_LOAD_CALLBACK) (PWSTR DllName, PVOID DllBase, SIZE_T DllSize, PVOID Reserved);
typedef VOID(NTAPI * RTL_VERIFIER_DLL_UNLOAD_CALLBACK)  (PWSTR DllName, PVOID DllBase, SIZE_T DllSize, PVOID Reserved);
typedef VOID(NTAPI *RTL_VERIFIER_NTDLLHEAPFREE_CALLBACK)(PVOID AllocationBase, SIZE_T AllocationSize);

typedef int(__stdcall *OldMessageBoxW)(HWND, LPWSTR, LPWSTR, UINT);

PVOID realMessageBoxW = NULL;

typedef struct _RTL_VERIFIER_THUNK_DESCRIPTOR {
	PCHAR ThunkName;
	PVOID ThunkOldAddress;
	PVOID ThunkNewAddress;
} RTL_VERIFIER_THUNK_DESCRIPTOR, *PRTL_VERIFIER_THUNK_DESCRIPTOR;

typedef struct _RTL_VERIFIER_DLL_DESCRIPTOR {
	PWCHAR DllName;
	DWORD DllFlags;
	PVOID DllAddress;
	PRTL_VERIFIER_THUNK_DESCRIPTOR DllThunks;
} RTL_VERIFIER_DLL_DESCRIPTOR, *PRTL_VERIFIER_DLL_DESCRIPTOR;

typedef struct _RTL_VERIFIER_PROVIDER_DESCRIPTOR {
	DWORD Length;
	PRTL_VERIFIER_DLL_DESCRIPTOR ProviderDlls;
	RTL_VERIFIER_DLL_LOAD_CALLBACK ProviderDllLoadCallback;
	RTL_VERIFIER_DLL_UNLOAD_CALLBACK ProviderDllUnloadCallback;
	PWSTR VerifierImage;
	DWORD VerifierFlags;
	DWORD VerifierDebug;
	PVOID RtlpGetStackTraceAddress;
	PVOID RtlpDebugPageHeapCreate;
	PVOID RtlpDebugPageHeapDestroy;
	RTL_VERIFIER_NTDLLHEAPFREE_CALLBACK ProviderNtdllHeapFreeCallback;
} RTL_VERIFIER_PROVIDER_DESCRIPTOR, *PRTL_VERIFIER_PROVIDER_DESCRIPTOR;

int WINAPI MessageBoxWHook(HWND hWnd, LPWSTR lpText, LPWSTR lpCaption, UINT uType);

static RTL_VERIFIER_THUNK_DESCRIPTOR g_Thunks[] =
{ { "MessageBoxW", NULL, (PVOID)(ULONG_PTR)MessageBoxWHook } };
static RTL_VERIFIER_DLL_DESCRIPTOR g_HookedDlls[] =
{ { L"user32.dll", 0, NULL, g_Thunks } };
static RTL_VERIFIER_PROVIDER_DESCRIPTOR avrfDescriptor =
{ sizeof(RTL_VERIFIER_PROVIDER_DESCRIPTOR), g_HookedDlls };

BOOL WINAPI DllMain(
	_In_ HINSTANCE hinstDLL,
	_In_ DWORD fdwReason,
	_In_ LPVOID lpvReserved
	)
{
	PRTL_VERIFIER_PROVIDER_DESCRIPTOR* pVPD = (PRTL_VERIFIER_PROVIDER_DESCRIPTOR *)lpvReserved;

	UNREFERENCED_PARAMETER(hinstDLL);

	switch (fdwReason) {

	case DLL_PROCESS_VERIFIER:
		*pVPD = &avrfDescriptor;
		break;
	}
	return TRUE;
}

int WINAPI MessageBoxWHook(HWND hWnd, LPWSTR lpText, LPWSTR lpCaption, UINT uType)
{
	((OldMessageBoxW)g_Thunks[0].ThunkOldAddress)(hWnd, L"Lemon Waffle", lpText, uType);
	return 0;
}