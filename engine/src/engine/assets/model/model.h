#pragma once
#include "engine/assets/core/asset.h"
#include "mesh.h"

namespace emerald {
	class Model : public Asset {
	public:
		Model() = default;
		Model(const std::string& name) : m_name(name) {}

		~Model() = default;

		void addSubmesh(const Ref<Mesh>& mesh) {
			m_subMeshes.push_back(mesh);
		}

		void addMaterial(const Ref<Material>& material) {
			m_materials.push_back(material);
		}

		const std::vector<Ref<Mesh>>& getSubMeshes() const {
			return m_subMeshes;
		}

		const std::string& getName() const { return m_name; }

	private:
		std::string m_name;
		std::vector<Ref<Mesh>> m_subMeshes;
		std::vector<Ref<Material>> m_materials;
	};
}