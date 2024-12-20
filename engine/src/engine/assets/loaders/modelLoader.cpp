#include "eepch.h"
#include "modelLoader.h"
#define ASSIMP_BUILD_DLL
#include "assimp/postprocess.h"
#include "assimp/mesh.h"
#include "assimp/scene.h"
#include "assimp/Importer.hpp"
#include "graphics/buffers/vertexBuffer.h"
#include "graphics/buffers/vertexArray.h"
#include "graphics/buffers/indexBuffer.h"
#include "utils/system/timer.h"

namespace emerald {
	static const uint32_t ImportFlags =
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_SortByPType |
		aiProcess_GenNormals |
		aiProcess_OptimizeGraph |
		//aiProcess_OptimizeMeshes |
		//aiProcess_Debone |
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

	PreloadedMesh::PreloadedMesh(const std::string& name, aiMesh* mesh, const aiScene* scene, uint32_t subMeshIndex) : m_name(name), m_subMeshIndex(subMeshIndex) {
		m_vertices.reserve(mesh->mNumVertices);
		m_indices.reserve((size_t)(mesh->mNumFaces) * 3);

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
	}

	Ref<Mesh> processMesh(const std::string& meshName, aiMesh* mesh, const aiScene* scene, uint32_t& index) {
		std::vector<Vertex> vertices;
		std::vector<GLuint> indices;
		vertices.reserve(mesh->mNumVertices);
		indices.reserve((size_t)(mesh->mNumFaces) * 3);

		for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
			Vertex vertex;
			vertex.m_position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
			vertex.m_normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
			//vertex.m_uv = mesh->mTextureCoords[0] ? glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y) : glm::vec2(0.0f, 0.0f);
			//vertex.m_tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
			//vertex.m_biTangent = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
			vertices.push_back(vertex);
		}

		for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			if (face.mNumIndices == 3) {
				indices.push_back(face.mIndices[0]);
				indices.push_back(face.mIndices[1]);
				indices.push_back(face.mIndices[2]);
			} else {
				Log::error("[Model] Unexpected number of indices ({}) in a face", face.mNumIndices);
			}
		}

		// Create the vertex array and buffers
		Ref<IndexBuffer> ibo = Ref<IndexBuffer>::create((byte*)indices.data(), (uint32_t)(indices.size() * sizeof(uint32_t)));
		Ref<VertexBuffer> vbo = Ref<VertexBuffer>::create((byte*)vertices.data(), (uint32_t)(vertices.size() * sizeof(Vertex)));
		Ref<VertexArray> vao = Ref<VertexArray>::create(s_layout);
		vao->addBuffer(vbo);
		vao->validate();
		return Ref<Mesh>::create(std::format("{}_{}", meshName, index), vao, ibo);
	}

	void processNode(std::vector<PreloadedMesh>& meshes, const std::string& meshName, aiNode* node, const aiScene* scene, uint32_t& index) {
		for (GLuint i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(PreloadedMesh(meshName, mesh, scene, index));
			index++;
		}

		for (uint32_t i = 0; i < node->mNumChildren; i++) {
			processNode(meshes, meshName, node->mChildren[i], scene, index);
		}
	}

	void ModelLoader::asyncLoad() {
		if (!std::filesystem::exists(m_path.string())) {
			Log::error("[Model] File at {} does not exist!", m_path.string().c_str());
			return;
		}

		Timer timer;
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(m_path.string(), ImportFlags);

		if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			Log::error("[Model] Failed to load model from {}", m_path.string().c_str());
			return;
		}

		m_modelName = m_path.stem().string();

		uint32_t index = 0;
		processNode(m_preloadedMeshes, m_modelName, scene->mRootNode, scene, index);


		asyncLoadTime = timer.get();
	}

	Ref<Asset> ModelLoader::syncLoad() {
		if (m_preloadedMeshes.empty()) return Ref<Model>();
		Timer timer;

		Ref<Model> model = Ref<Model>::create(m_modelName);
		for (auto& mesh : m_preloadedMeshes) {
			Ref<IndexBuffer> ibo = Ref<IndexBuffer>::create((byte*)mesh.m_indices.data(), (uint32_t)(mesh.m_indices.size() * sizeof(uint32_t)));
			Ref<VertexBuffer> vbo = Ref<VertexBuffer>::create((byte*)mesh.m_vertices.data(), (uint32_t)(mesh.m_vertices.size() * sizeof(Vertex)));
			Ref<VertexArray> vao = Ref<VertexArray>::create(s_layout);
			vao->addBuffer(vbo);
			vao->validate();
			model->addSubmesh(Ref<Mesh>::create(std::format("{}_{}", mesh.m_name, mesh.m_subMeshIndex), vao, ibo));
		}

		Log::info("[Model] Loaded {} in {:.2f} ms cpu and {:.2f} ms gpu", m_path.string().c_str(), asyncLoadTime, timer.get());
		return model;
	}
}