#include "stdafx.h"

void to_json(nlohmann::json& j, const Transform& obj) {
	//j = nlohmann::json{ {"position", obj.m_position}, {"size", obj.m_size}, {"rotation", obj.m_rotation} };
}

void from_json(const nlohmann::json& j, Transform& obj) {
	//j.at("position").get_to(obj.m_position);
	//j.at("size").get_to(obj.m_size);
	//j.at("rotation").get_to(obj.m_rotation);
}