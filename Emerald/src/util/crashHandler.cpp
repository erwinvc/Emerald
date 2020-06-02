#include "stdafx.h"

WString_t CrashHandler::s_crashMessages[15] = {
	L"Rip, the game crashed.",
	L"The fuck just happened?",
	L"Oops, the game crashed!",
	L"That wasn't supposed to happen...",
	L"Sorry, crash...",
	L"Try again.",
	L"Congratulations, you fucked up.",
	L"The application failed succesfully.",
	L"...",
	L"HUH?!",
	L"Error: NO ERROR",
	L"Please contact Emerald technical support.",
	L"Unknown error",
	L"This is your fault.",
	L"Yeaaaaah... Idk what just happened.",
};

String CrashHandler::GetTimeAndDateAsString() {
	time_t rawTime = time(nullptr);
	struct tm timeStruct;
	localtime_s(&timeStruct, &rawTime);

	static char buffer[48];
	sprintf_s(buffer, "[%04d-%02d-%02d_%02d-%02d-%02d]", timeStruct.tm_year + 1900, timeStruct.tm_mon, timeStruct.tm_mday, timeStruct.tm_hour, timeStruct.tm_min, timeStruct.tm_sec);
	return buffer;
}

void CrashHandler::WriteFullDump(EXCEPTION_POINTERS* exceptionPtrs, const String& name) {
	const DWORD Flags = MiniDumpWithFullMemoryInfo | MiniDumpWithHandleData | MiniDumpWithUnloadedModules | MiniDumpWithThreadInfo | MiniDumpWithDataSegs /*| MiniDumpWithModuleHeaders*/;

	_mkdir("crashes");

	WString wName(name.length(), L' ');
	std::copy(name.begin(), name.end(), wName.begin());
	wName = L"crashes/" + wName + L".dmp";

	HANDLE fileHandle = CreateFile(wName.c_str(), GENERIC_ALL, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (fileHandle) {
		MINIDUMP_EXCEPTION_INFORMATION exceptionInfo;
		exceptionInfo.ThreadId = GetCurrentThreadId();
		exceptionInfo.ExceptionPointers = exceptionPtrs;
		exceptionInfo.ClientPointers = TRUE;

		BOOL Result = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), fileHandle, (MINIDUMP_TYPE)Flags, &exceptionInfo, nullptr, nullptr);
		CloseHandle(fileHandle);
	}

}

LONG WINAPI CrashHandler::ExceptionFilter(struct _EXCEPTION_POINTERS* exceptionInfo) {
	String name = GetTimeAndDateAsString();
	ULONG_PTR exceptionCode = exceptionInfo->ExceptionRecord->ExceptionCode;
	ULONG_PTR exceptionInfo0 = exceptionInfo->ExceptionRecord->ExceptionInformation[0];
	ULONG_PTR exceptionInfo1 = exceptionInfo->ExceptionRecord->ExceptionInformation[1];
	ULONG_PTR exceptionInfo2 = exceptionInfo->ExceptionRecord->ExceptionInformation[2];

	if (exceptionCode == EXCEPTION_BREAKPOINT) return EXCEPTION_CONTINUE_EXECUTION;

	uint64_t exceptionAddress = (uint64_t)exceptionInfo->ExceptionRecord->ExceptionAddress;

	LOG_WARN("[~RCrash~x] game crashed...");
	LOG_WARN("[~RCrash~x] game base address:    ~10x%p | 0x%p", GetModuleHandleA(nullptr), (exceptionAddress - (uint64_t)GetModuleHandleA(nullptr)));
	LOG_WARN("[~RCrash~x] exception at address: ~10x%p", exceptionInfo->ExceptionRecord->ExceptionAddress);

	switch (exceptionCode) {
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: LOG_WARN("[~RCrash~x] caused by: ~1EXCEPTION_ARRAY_BOUNDS_EXCEEDED"); break;
		case EXCEPTION_BREAKPOINT: LOG_WARN("[~RCrash~x] caused by: ~1EXCEPTION_BREAKPOINT"); break;
		case EXCEPTION_DATATYPE_MISALIGNMENT: LOG_WARN("[~RCrash~x] caused by: ~1EXCEPTION_DATATYPE_MISALIGNMENT"); break;
		case EXCEPTION_FLT_DENORMAL_OPERAND: LOG_WARN("[~RCrash~x] caused by: ~1EXCEPTION_FLT_DENORMAL_OPERAND"); break;
		case EXCEPTION_FLT_DIVIDE_BY_ZERO: LOG_WARN("[~RCrash~x] caused by: ~1EXCEPTION_FLT_DIVIDE_BY_ZERO"); break;
		case EXCEPTION_FLT_INEXACT_RESULT: LOG_WARN("[~RCrash~x] caused by: ~1EXCEPTION_FLT_INEXACT_RESULT"); break;
		case EXCEPTION_FLT_INVALID_OPERATION: LOG_WARN("[~RCrash~x] caused by: ~1EXCEPTION_FLT_INVALID_OPERATION"); break;
		case EXCEPTION_FLT_OVERFLOW: LOG_WARN("[~RCrash~x] caused by: ~1EXCEPTION_FLT_OVERFLOW"); break;
		case EXCEPTION_FLT_STACK_CHECK: LOG_WARN("[~RCrash~x] caused by: ~1EXCEPTION_FLT_STACK_CHECK"); break;
		case EXCEPTION_FLT_UNDERFLOW: LOG_WARN("[~RCrash~x] caused by: ~1EXCEPTION_FLT_UNDERFLOW"); break;
		case EXCEPTION_ILLEGAL_INSTRUCTION: LOG_WARN("[~RCrash~x] caused by: ~1EXCEPTION_ILLEGAL_INSTRUCTION"); break;
		case EXCEPTION_INT_DIVIDE_BY_ZERO: LOG_WARN("[~RCrash~x] caused by: ~1EXCEPTION_INT_DIVIDE_BY_ZERO"); break;
		case EXCEPTION_INT_OVERFLOW: LOG_WARN("[~RCrash~x] caused by: ~1EXCEPTION_INT_OVERFLOW"); break;
		case EXCEPTION_INVALID_DISPOSITION: LOG_WARN("[~RCrash~x] caused by: ~1EXCEPTION_INVALID_DISPOSITION"); break;
		case EXCEPTION_NONCONTINUABLE_EXCEPTION: LOG_WARN("[~RCrash~x] caused by: ~1EXCEPTION_NONCONTINUABLE_EXCEPTION"); break;
		case EXCEPTION_PRIV_INSTRUCTION: LOG_WARN("[~RCrash~x] caused by: ~1EXCEPTION_PRIV_INSTRUCTION"); break;
		case EXCEPTION_SINGLE_STEP: LOG_WARN("[~RCrash~x] caused by: ~1EXCEPTION_SINGLE_STEP"); break;
		case EXCEPTION_STACK_OVERFLOW: LOG_WARN("[~RCrash~x] caused by: ~1EXCEPTION_STACK_OVERFLOW"); break;
		case DBG_CONTROL_C: LOG_WARN("[~RCrash~x] caused by: ~1DBG_CONTROL_C"); break;
		case EXCEPTION_ACCESS_VIOLATION:
			LOG_WARN("[~RCrash~x] caused by: ~1EXCEPTION_ACCESS_VIOLATION");
			if (exceptionInfo0 == 0) {
				LOG_WARN("[~RCrash~x] attempted to read from: ~10x%p", exceptionInfo1);
			} else if (exceptionInfo0 == 1) {
				LOG_WARN("[~RCrash~x] attempted to write to: ~10x%p", exceptionInfo1);
			} else if (exceptionInfo0 == 8) {
				LOG_WARN("[~RCrash~x] Data Execution Prevention (DEP) at: ~10x%p", exceptionInfo1);
			} else {
				LOG_WARN("[~RCrash~x] unknown access violation at: ~10x%p", exceptionInfo1);
			}
			break;
		case EXCEPTION_IN_PAGE_ERROR:
			LOG_WARN("[~RCrash~x] caused by: ~1EXCEPTION_IN_PAGE_ERROR");
			if (exceptionInfo0 == 0) {
				LOG_WARN("[~RCrash~x] attempted to read from: ~10x%08x", exceptionInfo1);
			} else if (exceptionInfo0 == 1) {
				LOG_WARN("[~RCrash~x] attempted to write to: ~10x%08x", exceptionInfo1);
			} else if (exceptionInfo0 == 8) {
				LOG_WARN("[~RCrash~x] Data Execution Prevention (DEP) at: ~10x%08x", exceptionInfo1);
			} else {
				LOG_WARN("[~RCrash~x] unknown access violation at: ~10x%08x", exceptionInfo1);
			}
			LOG_WARN("[~RCrash~x] NTSTATUS: ~10x%08x", exceptionInfo2);
			break;
		default: LOG_WARN("[~RCrash~x] caused by: ~10x%08x", exceptionCode);
	}

	LOG_WARN("[~RCrash~x] ASM registers:");
	LOG_WARN("[~RCrash~x] RAX: ~10x%llx~x || RSI: ~10x%llx", exceptionInfo->ContextRecord->Rax, exceptionInfo->ContextRecord->Rsi);
	LOG_WARN("[~RCrash~x] RBX: ~10x%llx~x || RDI: ~10x%llx", exceptionInfo->ContextRecord->Rbx, exceptionInfo->ContextRecord->Rdi);
	LOG_WARN("[~RCrash~x] RCX: ~10x%llx~x || RBP: ~10x%llx", exceptionInfo->ContextRecord->Rcx, exceptionInfo->ContextRecord->Rbp);
	LOG_WARN("[~RCrash~x] RDX: ~10x%llx~x || RSP: ~10x%llx", exceptionInfo->ContextRecord->Rdx, exceptionInfo->ContextRecord->Rsp);

	LOG_WARN("[~RCrash~x] STACK:\n ~1%s", GetStackTrace().c_str());

	Logger::ForceEmptyQueue();
	WString crashMessage = s_crashMessages[Random::Int(0, 14)];
	crashMessage += L"\n\nSave crash report?";

	GetApp()->Cleanup();
	if (MessageBox(NULL, crashMessage.c_str(), L"Game Crash!", MB_YESNO) == IDYES) {
		WriteFullDump(exceptionInfo, name);
		std::ifstream src("log.log", std::ios::binary);
		std::ofstream dst(Format_t("crashes/%s.log", name.c_str()), std::ios::binary);
		dst << src.rdbuf();
	}
	exit(EXIT_FAILURE);
	return EXCEPTION_CONTINUE_SEARCH;
}

void CrashHandler::Initialize() {
	SetUnhandledExceptionFilter(ExceptionFilter);
}

String CrashHandler::GetStackTrace() {
	string s;
	typedef USHORT(WINAPI* CaptureStackBackTraceType)(__in ULONG, __in ULONG, __out PVOID*, __out_opt PULONG);
	CaptureStackBackTraceType func = (CaptureStackBackTraceType)(GetProcAddress(LoadLibraryA("kernel32.dll"), "RtlCaptureStackBackTrace"));
	if (func != NULL) {
		HMODULE hMods[1024];
		DWORD cbNeeded;
		EnumProcessModules((HANDLE)-1, hMods, sizeof(hMods), &cbNeeded);
		const int kMaxCallers = 62;
		void* callers[kMaxCallers];
		int count = (func)(0, kMaxCallers, callers, NULL);
		for (int i = 1; i < count; i++) {
			DWORD64 callerAddr = (DWORD64)callers[i];
			HMODULE currentCallerModule = NULL;
			for (unsigned int i2 = 0; i2 < (cbNeeded / sizeof(HMODULE)); i2++) {
				MODULEINFO information = { 0 };
				GetModuleInformation((HANDLE)-1, hMods[i2], &information, sizeof(information));
				if (callerAddr >= (DWORD64)information.lpBaseOfDll && callerAddr <= (DWORD64)information.lpBaseOfDll + information.SizeOfImage) {
					currentCallerModule = hMods[i2];
					break;
				}
			}
			if (currentCallerModule != NULL) {
				char moduleName[MAX_PATH];
				GetModuleBaseNameA((HANDLE)-1, currentCallerModule, moduleName, MAX_PATH);
				char buf[512];
				sprintf_s(buf, "0x%08llx [%s]%s", callerAddr - (DWORD64)currentCallerModule, moduleName, i != count - 1 ? "\n " : "");
				s += buf;
			} else {
				char buf[512];
				sprintf_s(buf, "0x%016llx %s", callerAddr, i != count - 1 ? ", " : "");
				s += buf;
			}
		}
		return s.c_str();
	}
	return "[failed]";
}