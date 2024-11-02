#include "eepch.h"
#include "modelLoader.h"
#include "assimp/postprocess.h"
#include "assimp/mesh.h"
#include "assimp/scene.h"
#include "assimp/Importer.hpp"
#include "graphics/buffers/vertexBuffer.h"
#include "graphics/buffers/vertexArray.h"
#include "graphics/buffers/indexBuffer.h"

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
		//{VertexAttributeType::FLOAT2, "vsUv", 0}
		//{VertexAttributeType::FLOAT3, "vsBitangents", 0},
		//{VertexAttributeType::FLOAT3, "vsTangents", 0}
	};


	Ref<Mesh> processMesh(const std::string& meshName, aiMesh* mesh, const aiScene* scene, uint32_t& index) {
		std::vector<Vertex> vertices;
		std::vector<GLuint> indices;

		vertices.reserve(mesh->mNumVertices);
		size_t capacity = (size_t)(mesh->mNumFaces) * 3;
		indices.reserve(capacity);

		for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
			Vertex vertex;
			vertex.m_position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
			vertex.m_normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
			//vertex.m_uv = mesh->mTextureCoords[0] ? glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y) : glm::vec2(0.0f, 0.0f);
		//	vertex.m_tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
		//	vertex.m_biTangent = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };

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

	std::vector<Ref<Mesh>> processNode(const std::string& meshName, aiNode* node, const aiScene* scene, uint32_t& index) {
		std::vector<Ref<Mesh>> meshes;
		for (GLuint i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(meshName, mesh, scene, index));
			index++;
		}

		for (GLuint i = 0; i < node->mNumChildren; i++) {
			auto childMeshes = processNode(meshName, node->mChildren[i], scene, index);
			meshes.insert(meshes.end(), childMeshes.begin(), childMeshes.end());
		}
		return meshes;
	}

	std::vector<Ref<Mesh>> ModelLoader::load() {
		if (!std::filesystem::exists(m_file.string())) {
			Log::error("[Model] File at {} does not exist!", m_file.string().c_str());
			return {};
		}

		Timer timer;
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(m_file.string(), ImportFlags);

		if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			Log::error("[Model] Failed to load model from {}", m_file.string().c_str());
			return {};
		}

		uint32_t index = 0;
		std::vector<Ref<Mesh>> meshes = processNode(m_file.stem().string(), scene->mRootNode, scene, index);

		Log::info("[Model] Loaded {} in {:.2f} ms", m_file.string().c_str(), timer.get());
		return meshes;
	}
}