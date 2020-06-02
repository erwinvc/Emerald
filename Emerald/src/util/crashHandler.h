#pragma once

class CrashHandler {
private:
	static WString_t s_crashMessages[15];
	static String GetTimeAndDateAsString();
	static void WriteFullDump(EXCEPTION_POINTERS* exceptionPtrs, const String& name);
	static LONG WINAPI ExceptionFilter(struct _EXCEPTION_POINTERS* exceptionInfo);
	
public:
	static void Initialize();
	static String GetStackTrace();
};