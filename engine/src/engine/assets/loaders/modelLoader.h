#pragma once
#include <filesystem>
#include "engine/assets/model/mesh.h"
#include "engine/assets/model/model.h"
#include "assetLoader.h"
#include "../metadata/assetMetadata.h"
#include "textureLoader.h"

struct aiMesh;
struct aiScene;
namespace emerald {
	struct Vertex {
		glm::vec3 m_position;
		glm::vec3 m_normal;
		glm::vec2 m_uv;
		glm::vec3 m_tangent;
		glm::vec3 m_biTangent;

		Vertex() : m_position(glm::vec3()), m_normal(glm::vec3()), m_uv(glm::vec2()), m_tangent(glm::vec3()), m_biTangent(glm::vec3()) {}
	};

	struct PreloadedMesh {
		std::string m_name;
		std::vector<Vertex> m_vertices;
		std::vector<GLuint> m_indices;
		uint32_t m_subMeshIndex;
		uint32_t m_materialIndex;
		bool m_valid = false;

		PreloadedMesh(const std::string& name, aiMesh* mesh, const aiScene* scene, uint32_t subMeshIndex);
	};

	class ModelLoader : public AssetLoader {
	public:
		ModelLoader(const std::filesystem::path& path, bool loadMaterials = false) : m_loadMaterials(loadMaterials), m_path(path) {}

		bool onBeginLoad() override;
		Ref<Asset> onFinishLoad() override;

	private:
		std::vector<PreloadedMesh> m_preloadedMeshes;
		std::vector<Ref<Material>> m_materials;
		std::filesystem::path m_path;
		std::string m_modelName;
		bool m_loadMaterials = false;
		int m_width = 0;
		int m_height = 0;
		float asyncLoadTime = 0.0f;
	};
}