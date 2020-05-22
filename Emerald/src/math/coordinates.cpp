#include "stdafx.h"

namespace Coordinates {
	bool WorldToScreen(const glm::vec3& pos, glm::vec2& out) {
		glm::vec4 clipSpacePos = Camera::active->GetProjectionMatrix() * (Camera::active->GetViewMatrix() * glm::vec4(pos, 1.0f));
		glm::vec3 ndcSpacePos = glm::vec3(clipSpacePos) / clipSpacePos.w;
		bool visible = clipSpacePos.z > 0.0f;
		out = visible ? (((glm::vec2(ndcSpacePos) + glm::vec2(1.0f, 1.0f)) / glm::vec2(2.0f, 2.0f)) * glm::vec2(GetApp()->GetWidth(), GetApp()->GetHeight())) : glm::vec2(-9999.0f, -9999.0f);
		return visible;
	}
}