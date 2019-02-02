#pragma once

class Camera;

#define ASSERT(x, ...) \
		if (!(x)) {\
			LOG_ERROR("*************************"); \
			LOG_ERROR("    ASSERTION FAILED!    "); \
			LOG_ERROR("*************************"); \
			LOG_ERROR(__FILE__, ": ", __LINE__); \
			LOG_ERROR("Condition: ", #x); \
			LOG_ERROR(__VA_ARGS__); \
			__debugbreak(); \
}

String va(String_t fmt, ...);

namespace Utils {
    //IO
    static String ReadFile(String path) {
        ifstream stream(path);
        string str((istreambuf_iterator<char>(stream)), istreambuf_iterator<char>());
        stream.close();
        return str;
    }

    //String
    static vector<String> Split(String& s, String splitter) {
        size_t pos;
        vector<String> out;
        while ((pos = s.find(splitter)) != String::npos) {
            String token = s.substr(0, pos);
            out.push_back(token);
            s.erase(0, pos + splitter.length());
        }
        out.push_back(s);
        return out;
    }


    static string ReplaceString(string subject, const string& search,
        const string& replace) {
        size_t pos = 0;
        while ((pos = subject.find(search, pos)) != string::npos) {
            subject.replace(pos, search.length(), replace);
            pos += replace.length();
        }
        return subject;
    }

    static void DoTimedFunction(int* timer, int timeMS, function<void()> function) {
        if (*timer < GetTickCount()) {
            *timer = GetTickCount() + timeMS;
            function();
        }
    }

    //Math?
    void setPositionInFrontOfCam(Vector3& dest, const Camera& cam, float distance);
}