#pragma once

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "Material.h"
#include "Mesh.h"
#include "LightGroup.h"

#include <vector>
#include <string>
#include <memory>
#include <d3d12.h>
#include <wrl.h>

class Camera;
class WorldTransform;
class ObjectColor;

class Model
{
public:
    void Initialize();

    void Draw(const WorldTransform& _transform, const Camera* _camera, uint32_t _textureHandle, ObjectColor* _color);
    void Draw(const WorldTransform& _transform, const Camera* _camera, ObjectColor* _color);

    void ShowImGui(const std::string& _name);

    static Model* CreateFromObj(const std::string& _filePath);

    Mesh* GetMeshPtr() { return mesh_[0].get(); }
    Material* GetMaterialPtr() { return material_[0].get(); }


    void SetLightGroup(LightGroup* _lightGroup) { lightGroup_ = _lightGroup; }

    ~Model() { delete lightGroup_; }

    static const std::string defaultDirpath_;
private:

    std::string name_ = {};

    std::vector<std::unique_ptr<Mesh>> mesh_ = {};
    std::vector<std::unique_ptr<Material>> material_ = {};

    LightGroup* lightGroup_ = nullptr;

    void LoadFile(const std::string& _filepath);
    void LoadMesh(const std::string& _filepath);
    void LoadMaterial(const std::string& _filepath);

    void TransferData();

};