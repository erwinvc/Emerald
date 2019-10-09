#pragma once

class RigidBody {
public:
	Vector3 m_velocity;
	Vector3 m_position;
	Vector3 m_size;

	vector<Rect> m_rects;
	const float PRECISION = 0.1f;
	const float gravity = 0.008f;
	RigidBody() : m_velocity(Vector3(0, 0, 0)), m_position(Vector3(14, 0, -2)), m_size(Vector3(0.4f, 0.4f, 0.4f)) {}

	void Update(const TimeStep& time, vector<Tile*> surroundingTiles) {
		m_rects.clear();
		m_velocity.y -= gravity;
		m_velocity *= 0.8f;

		//for (Tile* tile : surroundingTiles) {
		//	if (tile->IsSolid()) {
		//		m_rects.push_back(Rect((float)tile->m_position.x + 0.5f, (float)tile->m_position.y + 0.5f, 1.0f, 1.0f));
		//	}
		//}
		Move(m_rects, m_velocity);
	}

	/*Check for collision with a list of entities*/
	bool checkCollision(vector<Rect>& entityList, float xa, float ya, float za) {
		for (Rect& b : entityList) {
			bool withinX = Math::Within<float>(m_position.x - m_size.x / 2 + xa, m_position.x + m_size.x / 2 + xa, b.m_position.x - b.m_size.x / 2, b.m_position.x + b.m_size.x / 2);
			bool withinZ = Math::Within<float>(m_position.z - m_size.z / 2 + za, m_position.z + m_size.z / 2 + za, b.m_position.y - b.m_size.y / 2, b.m_position.y + b.m_size.y / 2);

			if (withinX && withinZ) {
				return true;
			}
		}
		return false;
	}

	void DebugDraw() {
		for (Rect& rect : m_rects) {
			GetLineRenderer()->DrawRect(rect, Color::Cyan());
		}
		GetLineRenderer()->DrawRect(Rect(m_position.x, m_position.z, 0.2f, 0.2f), Color::Yellow());
	}

	void Move(vector<Rect>& entityList, const Vector3& vel) {
		bool contactX = MoveDirection(entityList, m_position.x, vel.x, Vector3(1, 0, 0));
		bool contactZ = MoveDirection(entityList, m_position.z, vel.z, Vector3(0, 0, 1));
		m_position.y += m_velocity.y;

		if (m_position.y < 0) {
			m_position.y = 0;
			m_velocity.y = 0;
		}

		//if (!contactX) {
		//	float dist = (contactX.pos.z + contactX.size.z / 2) - (pos.z - size.z / 2);
		//	//if (dist < 1) pos.y = (contactX.pos.y + contactX.size.y / 2) + size.y/2;
		//	if (dist < 1) {
		//		pos.z += dist;
		//	}
		//	vel.x = 0;
		//}
		//if (!contactY != null) {
		//	float dist = (contactY.pos.z + contactY.size.z / 2) - (pos.z - size.z / 2);
		//	//if (dist < 1) pos.y = (contactZ.pos.y + contactZ.size.y / 2) + size.y/2;
		//	if (dist < 1) {
		//		pos.z += dist;
		//	}
		//	vel.y = 0;
		//}

		//if (contactZ != null) {
		//	jumping = false;
		//	vel.z = 0;
		//}
		checkCollision(entityList, 0, 0, 0);
	}

	float ToPrecision(float value) {
		return value < 0 ? -PRECISION : PRECISION;
	}


	bool MoveDirection(vector<Rect>& entityList, float& value, float d, const Vector3& axis) {
		bool contact = false;
		while (d != 0) {
			float precision = ToPrecision(d);
			if (Math::Abs(d) > PRECISION) {
				contact = checkCollision(entityList, precision * axis.x, precision * axis.y, precision * axis.z);
				if (!contact) {
					value += precision;
				} else return contact;
				d -= precision;
			} else {
				contact = checkCollision(entityList, d * axis.x, d * axis.y, d * axis.z);
				if (!contact) {
					value += d;
				} else return contact;
				d = 0;
			}
		}
		return false;
	}
};