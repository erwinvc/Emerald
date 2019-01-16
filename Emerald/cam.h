class Cam {
private:
    Vector3 viewDir;
    Vector3 right;
    Vector3 up;
    Vector3 position;

    double prevMouseX = 0.0;
    double prevMouseY = 0.0;

    double curMouseX = 0.0;
    double curMouseY = 0.0;

public:
    Matrix4 GetViewMatrix() {
        return Matrix4::LookAt(position, position + viewDir, up);

    }

    Cam(Vector3& position_, Vector3& viewDir_) : position(position_), viewDir(viewDir_) {
        viewDir = viewDir.Normalize();
        right = viewDir.Cross(Vector3(0.0f, 1.0f, 0.0f)).Normalize();
        up = Vector3(0, 1.0f, 0);
    }

    void Update(const float delta, GLFWwindow* window) {
        // we use mouse movement to change the camera viewing angle.
        prevMouseX = curMouseX;
        prevMouseY = curMouseY;
        glfwGetCursorPos(window, &curMouseX, &curMouseY);
        float mouseDeltaX = (float)(curMouseX - prevMouseX);
        float mouseDeltaY = (float)(curMouseY - prevMouseY);

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            viewDir = viewDir.Rotate(mouseDeltaX*-0.01f, up);
            viewDir = viewDir.Rotate(mouseDeltaY*-0.01f, right);
            right = viewDir.Cross(Vector3(0.0f, 1.0f, 0.0f));
            up = Vector3(0.0f, 1.0f, 0.0f);
        }

        static float cameraSpeed;

        if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
            cameraSpeed = 1000.0f;
        else
            cameraSpeed = 100.0f;

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            position += viewDir * delta * cameraSpeed;
        else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            position += viewDir  * -delta * cameraSpeed;

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            position += right * -delta * cameraSpeed;
        else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            position += right * +delta * cameraSpeed;

        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
            position += up * +delta * cameraSpeed;
        else if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
            position += up * -delta * cameraSpeed;

        if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS) {
            printf("vec3(%f,%f,%f), vec3(%f,%f,%f), vec3(%f,%f,%f), vec3(%f,%f,%f),\n\n",
                viewDir.x, viewDir.y, viewDir.z,
                right.x, right.y, right.z,
                up.x, up.y, up.z,
                position.x, position.y, position.z
            );
        }

    }

    Vector3 GetPosition() const {
        return position;
    }
};
