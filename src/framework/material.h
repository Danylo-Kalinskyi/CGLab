#pragma once
#include "shader.h"
#include "texture.h"
#include "framework.h"

struct sLight {
    Vector3 position;
    Vector3 color; // intensity
};

struct sUniformData {
    Matrix44 model;
    Matrix44 viewprojection;
    Vector3 ambient_light;
    // Material properties will be in Material class
    std::vector<sLight> lights;
    int num_lights = 1;
    // Toggles
    bool use_color_texture;
    bool use_specular_texture;
    bool use_normal_texture;
    // Add more as needed
};

class Material {
public:
    Shader* shader = nullptr;
    Texture* color_texture = nullptr;
    Texture* normal_texture = nullptr;
    // Material properties
    Vector3 Ka; // ambient
    Vector3 Kd; // diffuse
    Vector3 Ks; // specular
    float shininess = 1.0f;

    Material();
    ~Material();

    void Enable(const sUniformData& uniformData);
    void Disable();
};