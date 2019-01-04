#include "stdafx.h"

String va(String_t fmt, ...) {
    char vaBuffer[0x200];
    va_list ap;
    va_start(ap, fmt);
    vsprintf_s(vaBuffer, fmt, ap);
    va_end(ap);
    return String(vaBuffer);
}

namespace Utils {
    void setPositionInFrontOfCam(Vector3& dest, const Camera& cam, float distance) {
        const float x = cam.m_position.x;
        const float y = cam.m_position.y;
        const float z = cam.m_position.z;

        dest.x = x + sin(cam.m_rotation.yaw) * distance * abs(cos(cam.m_rotation.pitch));
        dest.y = y - sin(cam.m_rotation.pitch) * distance;
        dest.z = z - cos(cam.m_rotation.yaw) * distance * abs(cos(cam.m_rotation.pitch));
    }
}
