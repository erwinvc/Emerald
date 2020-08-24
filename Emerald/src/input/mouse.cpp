#include "stdafx.h"

void Mouse::Initialize(Window* window) {
	for (int i = 0; i < BUTTONSSIZE; i++) {
		m_buttonStates[i] = Button();
	}

	GetGLCallbackManager()->AddOnMouseButtonCallback(this, &Mouse::OnMouseButton);
	GetGLCallbackManager()->AddOnMousePosCallback(this, &Mouse::OnMousePos);
	GetGLCallbackManager()->AddOnScrollCallback(this, &Mouse::OnScroll);

	m_window = window;
}

bool Mouse::CheckImGuiControl() {
	if (m_overrideImGuiThisFrame) return false;
	return ImGui::GetCurrentContext()->NavWindow || ImGui::GetIO().WantCaptureMouse;
}

void Mouse::OnMouseButton(int button, int action, int mods) {
	if (action == GLFW_RELEASE) {
		m_buttonStates[button].m_glIsUpNow = true;
	} else if (action == GLFW_PRESS) {
		GetMouse()->m_lastButton = button;
		m_buttonStates[button].m_glIsUpNow = false;
	}
}

void Mouse::OnMousePos(double xpos, double ypos) {
	GetMouse()->m_position.x = (float)xpos;
	GetMouse()->m_position.y = (float)ypos;
}


void Mouse::OnScroll(double xoffset, double yoffset) {
	GetMouse()->m_scroll.x = (float)xoffset;
	GetMouse()->m_scroll.y = (float)yoffset;
}

void Mouse::Update() {
	m_overrideImGuiThisFrame = m_overrideImGuiNextFrame;
	m_overrideImGuiNextFrame = false;


	m_prevPosition = m_usePosition;
	m_usePosition = m_position;

	m_useScroll = m_scroll;
	m_scroll.x = 0;
	m_scroll.y = 0;
	
	float sensitivity = 0.6f * m_sensitivity + 0.2f;
	sensitivity = Math::Pow(sensitivity, 3) * 8.0f;
	
	m_rawDelta = m_prevPosition - m_usePosition;
	m_delta = m_rawDelta * sensitivity;

	m_imGuiControlThisFrame = CheckImGuiControl();
}

void Mouse::UpdateTick()
{
	for (int i = 0; i < 5; i++) {
		m_buttonStates[i].m_wasUp = m_buttonStates[i].m_isUpNow;
		m_buttonStates[i].m_isUpNow = m_buttonStates[i].m_glIsUpNow;
		m_buttonStates[i].m_justDown = m_buttonStates[i].m_wasUp && !m_buttonStates[i].m_isUpNow;
		m_buttonStates[i].m_justUp = !m_buttonStates[i].m_wasUp && m_buttonStates[i].m_isUpNow;
	}
}

bool Mouse::MouseWithin(float x, float y, float width, float height) {
	return Math::Within(m_usePosition.x, x, x + width) && Math::Within(m_usePosition.y, y, y + height);
}

bool Mouse::MouseWithinCentered(float x, float y, float width, float height) {
	return Math::Within(m_usePosition.x, x - width / 2, x + width / 2) && Math::Within(m_usePosition.y, y - height / 2, y + height / 2);
}
void Mouse::SetPosition(glm::vec2& position) {
	glfwSetCursorPos(m_window->GetHandle(), position.x, position.y);
}