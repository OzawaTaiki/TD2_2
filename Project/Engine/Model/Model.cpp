#include "Model.h"
#include "ModelManager.h"
#include "DXCommon.h"
#include "TextureManager.h"
#include "Camera.h"
#include "WorldTransform.h"
#include "ObjectColor.h"

#include <cassert>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <Debug.h>

const std::string Model::defaultDirpath_ = "Resources/models/";

void Model::Initialize()
{

}

void Model::Draw(const WorldTransform& _transform, const Camera* _camera, uint32_t _textureHandle, ObjectColor* _color)
{
    if (!lightGroup_)
    {
        lightGroup_ = new LightGroup;
        lightGroup_->Initialize();
    }

    ID3D12GraphicsCommandList* commandList = DXCommon::GetInstance()->GetCommandList();

    for (auto& mesh : mesh_)
    {
        mesh->QueueCommand(commandList);

        // カメラ（ｖｐ
        commandList->SetGraphicsRootConstantBufferView(0, _camera->GetResource()->GetGPUVirtualAddress());
        // トランスフォーム
        commandList->SetGraphicsRootConstantBufferView(1, _transform.GetResource()->GetGPUVirtualAddress());
        // マテリアル
        material_[mesh->GetUseMaterialIndex()]->MateriallQueueCommand(commandList, 2);
        // カラー
        _color->QueueCommand(commandList, 3);
        //commandList->SetGraphicsRootConstantBufferView(3, _color->GetResource()->GetGPUVirtualAddress());
        // テクスチャ
        material_[mesh->GetUseMaterialIndex()]->TextureQueueCommand(commandList, 4, _textureHandle);
        // ライトたち
        lightGroup_->TransferData();

        commandList->DrawIndexedInstanced(mesh->GetIndexNum(), 1, 0, 0, 0);
    }
}

void Model::Draw(const WorldTransform& _transform, const Camera* _camera, ObjectColor* _color)
{
    if (!lightGroup_)
    {
        lightGroup_ = new LightGroup;
        lightGroup_->Initialize();
    }

    ID3D12GraphicsCommandList* commandList = DXCommon::GetInstance()->GetCommandList();


    for (auto& mesh : mesh_)
    {
        mesh->QueueCommand(commandList);

        // カメラ（ｖｐ
        commandList->SetGraphicsRootConstantBufferView(0, _camera->GetResource()->GetGPUVirtualAddress());
        // トランスフォーム
        commandList->SetGraphicsRootConstantBufferView(1, _transform.GetResource()->GetGPUVirtualAddress());
        // マテリアル
        material_[mesh->GetUseMaterialIndex()]->MateriallQueueCommand(commandList, 2);
        // カラー
        _color->QueueCommand(commandList, 3);
        //commandList->SetGraphicsRootConstantBufferView(3, _color->GetResource()->GetGPUVirtualAddress());
        // テクスチャ
        material_[mesh->GetUseMaterialIndex()]->TextureQueueCommand(commandList, 4);
        // ライトたち
        lightGroup_->TransferData();

        commandList->DrawIndexedInstanced(mesh->GetIndexNum(), 1, 0, 0, 0);
    }
}

void Model::ShowImGui(const std::string& _name)
{


}

Model* Model::CreateFromObj(const std::string& _filePath)
{
    Model* model = ModelManager::GetInstance()->FindSameModel(_filePath);

    if (model->mesh_.size() == 0|| model->material_.size() == 0)
    {
       model-> LoadFile(_filePath);
        //model->LoadMesh(_filePath);
        //model->LoadMaterial(_filePath);
    }

    return model;
}

void Model::LoadFile(const std::string& _filepath)
{
    auto start = std::chrono::high_resolution_clock::now();
    Utils::Log("load start\nfilepath:" + defaultDirpath_ + _filepath + "\n");
    name_ = _filepath;

    Assimp::Importer importer;
    std::string filepath = defaultDirpath_ + _filepath;
    const aiScene* scene = importer.ReadFile(filepath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs); // 三角形の並びを逆に，UVのy軸反転
    assert(scene->HasMeshes());// メッシュがないのは対応しない


    // メッシュの読み込み
    for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
        aiMesh* mesh = scene->mMeshes[meshIndex];
        assert(mesh->HasNormals());						    // 法線がないMeshは今回は非対応
        assert(mesh->HasTextureCoords(0));				    // TexcoordがないMeshは今回は非対応
        std::unique_ptr<Mesh> pMesh = std::make_unique<Mesh>();
        pMesh->Initialize();

        for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex)
        {
            Mesh::VertexData vertex = {};
            vertex.position = { mesh->mVertices[vertexIndex].x, mesh->mVertices[vertexIndex].y, mesh->mVertices[vertexIndex].z, 1.0f };
            vertex.normal = { mesh->mNormals[vertexIndex].x, mesh->mNormals[vertexIndex].y, mesh->mNormals[vertexIndex].z };
            vertex.texcoord = { mesh->mTextureCoords[0][vertexIndex].x, mesh->mTextureCoords[0][vertexIndex].y };

            vertex.position.z *= -1.0f;  // Z反転
            vertex.normal.z *= -1.0f;    // Z反転

            pMesh->vertices_.push_back(vertex);
        }

        for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
            aiFace& face = mesh->mFaces[faceIndex];
            assert(face.mNumIndices == 3); // 三角形のみサポート
            for (uint32_t index = 0; index < face.mNumIndices; ++index)
            {
                pMesh->indices_.push_back(face.mIndices[index]);
            }
        }
        pMesh->SetUseMaterialIndex(mesh->mMaterialIndex);
        pMesh->TransferData();
        mesh_.push_back(std::move(pMesh));
    }

    material_.resize(scene->mNumMaterials);
    for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
        aiMaterial* material = scene->mMaterials[materialIndex];

        std::string path = "";
        material_[materialIndex] = std::make_unique<Material>();

        if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
            aiString textureFilePath;
            material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);

            // /の位置を探す
            size_t slashPos = _filepath.find('/');

            if (slashPos != std::string::npos)
            {// 見つかったら
                std::string dirPath = _filepath.substr(0, slashPos);
                textureFilePath = dirPath + '/' + textureFilePath.C_Str();
            }

            path = defaultDirpath_ + "/" + textureFilePath.C_Str();
        }
        else if (path == "")
        {
            path = "Resources/images/uvChecker.png";
        }
        material_[materialIndex]->Initialize(path);
    }

    TransferData();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::string str = std::to_string(duration);
    Utils::Log("Load finish \ntime :" + str + "ms\n");
    //Utils::Log("data\nvertex :" + std::to_string(vertices_.size()) + "\nindex :" + std::to_string(indices_.size()) + "\n");

}

void Model::LoadMesh(const std::string& _filePath)
{/*
    name_ = _filePath;
    mesh_ = std::make_unique<Mesh>();
    mesh_->Initialize();
    mesh_->LoadFile(_filePath);*/
}

void Model::LoadMaterial(const std::string& _filePath)
{/*
    material_ = std::make_unique<Material>();
    material_->Initialize(mesh_->GetTexturePath());
    material_->LoadTexture();*/
}

void Model::TransferData()
{
}
