#pragma once
#include <filesystem>
#include "util/timer.h"
#include "../mesh.h"

namespace emerald {
	struct Vertex {
		glm::vec3 m_position;
		glm::vec3 m_normal;
		//glm::vec2 m_uv;
	//	glm::vec3 m_tangent;
	//	glm::vec3 m_biTangent;

		Vertex() : m_position(glm::vec3()), m_normal(glm::vec3()) {}//, m_uv(glm::vec2()) {}//, m_tangent(glm::vec3()), m_biTangent(glm::vec3()) {}
	};

	class ModelLoader {

	public:
		ModelLoader(const std::string& file, bool loadMaterials = false) : m_loadMaterials(loadMaterials), m_file(file) {}

		std::vector<Ref<Mesh>> load();

	private:
		std::string m_file;
		std::string m_dir;
		bool m_loadMaterials = false;
		int m_width = 0;
		int m_height = 0;
	};
}