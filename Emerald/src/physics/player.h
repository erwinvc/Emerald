#pragma once

class Player {
public:
	RigidBody body;
	vector<Tile*> m_surroundingTiles;

	void Move(const TimeStep& time) {

		float speed = 0.5f;
		glm::vec3 vel(0, 0, 0);

		if (KeyDown('W')) vel.x += speed * time.GetSeconds();
		if (KeyDown('S')) vel.x -= speed * time.GetSeconds();
		if (KeyDown('D')) vel.z += speed * time.GetSeconds();
		if (KeyDown('A')) vel.z -= speed * time.GetSeconds();
		if (KeyDown(' ') && body.m_position.y == 0) body.m_velocity.y += 0.2f;
		if (KeyDown(LSHIFT)) {
			vel *= 2;
			if (vel.x != 0 || vel.y != 0)
				GetPipeline()->m_chromatic = Math::Ease(GetPipeline()->m_chromatic, 0.015f, 6);
		} else GetPipeline()->m_chromatic = Math::Ease(GetPipeline()->m_chromatic, 0, 6);

		float _sin = Math::Sin(GetCamera()->transform.m_rotation.y - Math::HALF_PI);
		float _cos = Math::Cos(GetCamera()->transform.m_rotation.y - Math::HALF_PI);

		float x = vel.x * _cos - vel.z * _sin;
		float z = vel.x * _sin + vel.z * _cos;

		body.m_velocity.x += x;
		body.m_velocity.z += z;

		body.Update(time, m_surroundingTiles);

		GetCamera()->transform.m_position = glm::vec3(body.m_position.x, body.m_position.y + 0.9f, body.m_position.z);

		if (GetApp()->GetWindow()->GetFocussed()) {
			glfwSetInputMode(GetApp()->GetWindow()->GetHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		} else {
			glfwSetInputMode(GetApp()->GetWindow()->GetHandle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}

		m_surroundingTiles.clear();
		for (int i = 0; i < 8; i++) {
			//glm::ivec2 dir = WangTile::m_positionOffsetsComplete[i];
			//Tile* tile = GetWorld()->GetTile(Math::Round(body.m_position.x) + dir.x, Math::Round(body.m_position.z) + dir.y);
			//if (tile) m_surroundingTiles.push_back(tile);
		}
	}

	vector<Tile*>& GetTiles() { return m_surroundingTiles; }
	void DebugDraw() {
		body.DebugDraw();
	}
};