#pragma once

static bool GLLogCall(const char* function, const char* file, int line) {
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        String fileName = file;
        size_t position = fileName.find_last_of('\\');
        fileName = fileName.substr(position);
        LOG_ERROR("[GLError] %s caused error %s in %s at line %d", function, GLUtils::GLErrorToString(error).c_str(), fileName.c_str(), line);
        return false;
    }
    return true;
}

//if(!GetGLFiberManager()->IsSameThread(this_thread::get_id())) LOG_ERROR("AAAAAA");

#ifdef EE_DEBUG
#define GL(x) glGetError();\
		x; \
		if (!GLLogCall(#x, __FILE__, __LINE__)) __debugbreak();
#else
#define GL(x) x
#endif
