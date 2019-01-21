#pragma once

#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/material.h"
#include <assimp/Importer.hpp>
#include "assets/textureManager.h"

class Model {
private:
    vector<Mesh*> m_meshes;
    vector<Material*> m_materials;
    String m_dir;

    void ProcessNode(aiNode* node, const aiScene* scene) {
        for (GLuint i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            m_meshes.push_back(ProcessMesh(mesh, scene));
        }

        for (GLuint i = 0; i < node->mNumChildren; i++) {
            ProcessNode(node->mChildren[i], scene);
        }
    }

    Mesh* ProcessMesh(aiMesh *mesh, const aiScene *scene) {
        vector<GLfloat> texcoords;
        for (uint i = 0; i < mesh[0].mNumVertices; i++) {
            texcoords.push_back(mesh[0].mTextureCoords[0][i].x);
            texcoords.push_back(mesh[0].mTextureCoords[0][i].y);
        }

        //VAO
        VertexArray* vaoModel = new VertexArray();
        vaoModel->AddBuffer(new Buffer((float*)mesh->mVertices, mesh->mNumVertices * 3, 3), 0, false);
        vaoModel->AddBuffer(new Buffer((float*)mesh->mNormals, mesh->mNumVertices * 3, 3), 1, false);
        vaoModel->AddBuffer(new Buffer((float*)texcoords.data(), texcoords.size(), 2), 2, false);
        vaoModel->AddBuffer(new Buffer((float*)mesh->mTangents, mesh->mNumVertices * 3, 3), 3, false);

        //IBO
        vector<GLuint> indices;
        for (uint i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (uint j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
        aiString path;
        mat->GetTexture(aiTextureType_DIFFUSE, 0, &path);
        String fullPath = m_dir + "\\" + path.C_Str();

        return new Mesh(vaoModel, new IndexBuffer(indices.data(), indices.size()), m_materials[mesh->mMaterialIndex]);
    }

    void LoadMaterials(const aiScene *scene) {
        for (int i = 0; i < scene->mNumMaterials; i++) {
            aiMaterial* mat = scene->mMaterials[i];
            if (mat->GetTextureCount(aiTextureType_AMBIENT) > 0) {
                aiString path;
                mat->GetTexture(aiTextureType_AMBIENT, 0, &path);
                String fullPath = m_dir + "\\" + path.C_Str();
                m_materials[i]->SetAlbedo(new Texture(fullPath));
            }
            if (mat->GetTextureCount(aiTextureType_HEIGHT) > 0) {
                aiString path;
                mat->GetTexture(aiTextureType_HEIGHT, 0, &path);
                String fullPath = m_dir + "\\" + path.C_Str();
                m_materials[i]->SetNormal(new Texture(fullPath));
            }
        }
    }
public:
    ~Model() {
        for (auto& mesh : m_meshes) delete mesh;
        for (auto& mat : m_materials) delete mat;
        //map<String, Material*>::iterator it;
        //for (it = m_materials.begin(); it != m_materials.end(); it++) {
        //    delete it->second;
        //    m_materials.erase(it);
        //}
    }

    void LoadModel(const String& path);

    void Draw(Shader* shader) {
        for (auto& mesh : m_meshes) {
            mesh->GetMaterial()->Bind(shader);
            mesh->Draw();
        }
    }
};