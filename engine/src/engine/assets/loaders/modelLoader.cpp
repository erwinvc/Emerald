#include "eepch.h"
#include "modelLoader.h"

#define ASSIMP_BUILD_NO_EXPORT
#define ASSIMP_BUILD_NO_IMPORT
#include "assimp/postprocess.h"
#include "assimp/mesh.h"
#include "assimp/scene.h"
#include "assimp/Importer.hpp"
#include "graphics/buffers/vertexBuffer.h"
#include "graphics/buffers/vertexArray.h"
#include "graphics/buffers/indexBuffer.h"
#include "utils/system/timer.h"
#include "graphics/textures/texture.h"
#include "textureLoader.h"
#include "graphics/render/renderPipeline.h"
#include "../core/assetRegistry.h"
#include "graphics/textures/fallbackTextures.h"
#include "../texture/textureAsset.h"
#include "../../events/eventSystem.h"
#include "core/application/application.h"
#include "assimp/cimport.h"
#include "utils/math/color.h"
#include "utils/system/fileSystem.h"

namespace emerald {
	static const uint32_t ImportFlags =
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_SortByPType |
		aiProcess_GenNormals |
		aiProcess_OptimizeGraph |
		aiProcess_OptimizeMeshes |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ImproveCacheLocality |
		aiProcess_RemoveRedundantMaterials |
		aiProcess_FindInvalidData |
		aiProcess_ValidateDataStructure;

	static const VertexBufferLayout s_layout = {
		{VertexAttributeType::FLOAT3, "vsPos", 0},
		{VertexAttributeType::FLOAT3, "vsNormal", 0},
		{VertexAttributeType::FLOAT2, "vsUv", 0},
		{VertexAttributeType::FLOAT3, "vsBitangents", 0},
		{VertexAttributeType::FLOAT3, "vsTangents", 0}
	};

	PreloadedMesh::PreloadedMesh(const std::string& name, aiMesh* mesh, const aiScene* scene, uint32_t subMeshIndex) : m_name(name), m_subMeshIndex(subMeshIndex), m_materialIndex(mesh->mMaterialIndex) {
		try {
			m_vertices.reserve(mesh->mNumVertices);
			m_indices.reserve((size_t)(mesh->mNumFaces) * 3);

			if (!mesh->mVertices) return;
			if (!mesh->mNormals) return;
			if (!mesh->mTextureCoords) return;
			if (!mesh->mTangents) return;
			if (!mesh->mBitangents) return;

			for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
				Vertex vertex;
				vertex.m_position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
				vertex.m_normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
				vertex.m_uv = mesh->mTextureCoords[0] ? glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y) : glm::vec2(0.0f, 0.0f);
				vertex.m_tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
				vertex.m_biTangent = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
				m_vertices.push_back(vertex);
			}

			for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
				aiFace face = mesh->mFaces[i];
				if (face.mNumIndices == 3) {
					m_indices.push_back(face.mIndices[0]);
					m_indices.push_back(face.mIndices[1]);
					m_indices.push_back(face.mIndices[2]);
				} else {
					Log::error("[Model] Unexpected number of indices ({}) in a face", face.mNumIndices);
				}
			}

			m_valid = true;
		} catch (std::exception) {}
	}

	static void processNode(std::vector<PreloadedMesh>& meshes, const std::string& meshName, aiNode* node, const aiScene* scene, uint32_t& index) {
		for (GLuint i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			PreloadedMesh preloadedMesh = PreloadedMesh(meshName, mesh, scene, index);
			if (preloadedMesh.m_valid) {
				meshes.push_back(std::move(preloadedMesh));
			}
			index++;
		}

		for (uint32_t i = 0; i < node->mNumChildren; i++) {
			processNode(meshes, meshName, node->mChildren[i], scene, index);
		}
	}

	static bool loadMaterialTexture(const std::string& name, Ref<Material> material, aiMaterial* mat, aiTextureType type, const std::filesystem::path& basePath, const Ref<Texture>& fallback) {
		material->setTexture(name, fallback);

		if (mat->GetTextureCount(type) > 0) {
			aiString str;
			mat->GetTexture(type, 0, &str);
			std::filesystem::path texPath = basePath.parent_path() / str.C_Str();

			AssetMetadata* metadata = AssetRegistry::getAssetMetadata(texPath);
			if (metadata) {
				AssetRegistry::executeWhenAssetStreamed(metadata, [material, name](const Ref<Asset>& asset) {
					material->setTexture(name, asset);
				});
				return true;
			}
		}
		return false;
	}

	static float getMaterialFloat(aiMaterial* mat, const char* key, unsigned int type, unsigned int idx, float defaultValue = 1.0f) {
		float value;
		if (mat->Get(key, type, idx, value) == AI_SUCCESS)
			return value;
		return defaultValue;
	}

	static bool getMaterialColor(aiMaterial* mat, const char* key, unsigned int type, unsigned int idx, glm::vec4& out) {
		aiColor4D color;
		if (mat->Get(key, type, idx, color) != AI_SUCCESS) return false;
		out = glm::vec4(color.r, color.g, color.b, color.a);
		return true;
	}

	Expected<Empty> ModelLoader::onBeginLoad() {
		if (!std::filesystem::exists(m_path)) {
			return Unexpected("[Model] File at {} does not exist!", m_path.u8string());
		}

		Timer timer;
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(FileSystem::pathToString(m_path), ImportFlags);

		if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			return Unexpected("[Model] Failed to load model from {}", m_path.u8string());
		}

		m_modelName = FileSystem::pathToString(m_path.stem());

		uint32_t index = 0;

		m_preloadedMeshes.reserve(scene->mNumMeshes);
		m_materials.reserve(scene->mNumMaterials);

		processNode(m_preloadedMeshes, m_modelName, scene->mRootNode, scene, index);

		for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
			aiMaterial* aiMat = scene->mMaterials[i];

			aiString name;
			aiMat->Get(AI_MATKEY_NAME, name);

			Ref<Material> material = Ref<Material>::create(name.C_Str(), RenderPipeline::shader);

			m_materials.push_back(material);

			Log::info("[Model] Loading material {}", material->getName());

			glm::vec4 baseColor = Color::white();
			//if (!getMaterialColor(aiMat, AI_MATKEY_BASE_COLOR, baseColor)) {
			//	getMaterialColor(aiMat, AI_MATKEY_COLOR_DIFFUSE, baseColor);
			//}
			material->set("_BaseColor", baseColor);
			material->set("_Metallic", getMaterialFloat(aiMat, AI_MATKEY_METALLIC_FACTOR, 1.0f));
			material->set("_Roughness", getMaterialFloat(aiMat, AI_MATKEY_ROUGHNESS_FACTOR, 1.0f));

			loadMaterialTexture("_Albedo", material, aiMat, aiTextureType_DIFFUSE, m_path, FallbackTextures::null());
			if (!loadMaterialTexture("_Normal", material, aiMat, aiTextureType_HEIGHT, m_path, FallbackTextures::normal())) {
				loadMaterialTexture("_Normal", material, aiMat, aiTextureType_NORMALS, m_path, FallbackTextures::normal());
			}
			if (!loadMaterialTexture("_RoughnessMetalic", material, aiMat, aiTextureType_METALNESS, m_path, FallbackTextures::white())) {
				loadMaterialTexture("_RoughnessMetalic", material, aiMat, aiTextureType_DIFFUSE_ROUGHNESS, m_path, FallbackTextures::white());
			}
			loadMaterialTexture("_Emission", material, aiMat, aiTextureType_EMISSIVE, m_path, FallbackTextures::black());
		}

		asyncLoadTime = timer.get();

		return Expected<Empty>();
	}

	Expected<Ref<Asset>> ModelLoader::onFinishLoad() {
		if (m_preloadedMeshes.empty()) {
			return Unexpected("No meshes found in model " + FileSystem::pathToString(m_path));
		}
		Timer timer;

		Ref<Model> model = Ref<Model>::create(m_modelName);
		for (auto& mesh : m_preloadedMeshes) {
			Ref<IndexBuffer> ibo = Ref<IndexBuffer>::create((byte*)mesh.m_indices.data(), (uint32_t)(mesh.m_indices.size() * sizeof(uint32_t)));
			Ref<VertexBuffer> vbo = Ref<VertexBuffer>::create((byte*)mesh.m_vertices.data(), (uint32_t)(mesh.m_vertices.size() * sizeof(Vertex)));
			Ref<VertexArray> vao = Ref<VertexArray>::create(s_layout);
			vao->addBuffer(vbo);
			vao->validate();

			Ref<Mesh> subMesh = Ref<Mesh>::create(std::format("{}_{}", mesh.m_name, mesh.m_subMeshIndex), vao, ibo);

			if (mesh.m_materialIndex < m_materials.size()) {
				subMesh->setMaterial(m_materials[mesh.m_materialIndex]);
			}

			model->addSubmesh(subMesh);
		}

		Log::info("[Model] Loaded {} in {:.2f} ms", m_path.u8string(), asyncLoadTime + timer.get());
		return model.as<Asset>();
	}
}