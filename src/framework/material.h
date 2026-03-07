#pragma once

#include "shader.h"
#include "texture.h"
#include "framework.h"

// Light struct
struct sLight {
    Vector3 position;
    Vector3 color;
};

// Uniform data struct to pass to shaders
struct sUniformData {
    Matrix44 model;
    Matrix44 viewprojection;
    Vector3 ambient_light;
    Vector3 eye;
    sLight lights[10]; // Support up to 10 lights
    int num_lights;
    bool use_color_texture;
    bool use_specular_texture;
    bool use_normal_texture;
};

class Material {
public:
    Shader* shader = nullptr;
    Texture* color_texture = nullptr;
    Texture* specular_texture = nullptr;
    Texture* normal_texture = nullptr;

    Vector3 ka; // Ambient coefficient
    Vector3 kd; // Diffuse coefficient
    Vector3 ks; // Specular coefficient
    float ns;   // Specular exponent

    Material();
    ~Material();

    void Enable(const sUniformData& uniformData);
    void Disable();
};