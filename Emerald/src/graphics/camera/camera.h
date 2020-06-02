#pragma once

struct CornerRayPositions {
	union {
		struct {
			glm::vec3 TL;
			glm::vec3 TR;
			glm::vec3 BR;
			glm::vec3 BL;
		};
		glm::vec3 corners[4];
	};

	CornerRayPositions() {}
};

class Camera {
protected:
	glm::mat4 m_projectionMatrix;
	glm::mat4 m_viewMatrix;
	glm::mat4 m_inverseProjectionMatrix;
	glm::mat4 m_inverseViewMatrix;
	float m_FOV;
	float m_nearPlane;
	float m_farPlane;
	glm::vec4 m_viewPort;
	
	void UpdateViewMatrix(float partialTicks);
	virtual void UpdateProjectionMatrix();

public:
	static Camera* active;
	static Camera* uiActive;
	Transform lastUpdateTransform;
	Transform transform;

	Camera(glm::vec2 viewportSize, float fov, float nearPlane, float farPlane) : m_viewPort(glm::vec4(0, 0, viewportSize.x, viewportSize.y)) { SetProjectionMatrix(fov, nearPlane, farPlane); UpdateViewMatrix(0); }

	virtual ~Camera() {}
	virtual void Update(const TimeStep& time) {}
	virtual void DrawUpdate(float partialTicks);
	virtual void FixedUpdate() {}

	void SetProjectionMatrix(float fov, float nearPlane, float farPlane);
	void SetViewport(uint x, uint y, uint width, uint height);
	
	inline glm::mat4 GetProjectionMatrix() const { return m_projectionMatrix; }
	inline glm::mat4 GetInverseProjectionMatrix() const { return m_inverseProjectionMatrix; }
	inline glm::mat4 GetViewMatrix() const { return m_viewMatrix; }
	inline glm::mat4 GetInverseViewMatrix() const { return m_inverseViewMatrix; }
	inline glm::vec4 GetViewport() const { return m_viewPort; }
	inline float GetFOV() const { return m_FOV; }
	inline float GetNear() const { return m_nearPlane; }
	inline float GetFar() const { return m_farPlane; }

	virtual void OnImGui();

	static CornerRayPositions GetCornerRays(float offset);
};