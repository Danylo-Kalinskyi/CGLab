#include "material.h"

Material::Material() {
    // Initialize defaults
    Ka = Vector3(0.1f, 0.1f, 0.1f);
    Kd = Vector3(0.8f, 0.8f, 0.8f);
    Ks = Vector3(1.0f, 1.0f, 1.0f);
    shininess = 32.0f;
}

Material::~Material() {}

void Material::Enable(const sUniformData& uniformData) {
    if (!shader) return;

    shader->Enable();

    // Upload transform & camera-related uniforms
    shader->SetMatrix44("u_model", uniformData.model);
    shader->SetMatrix44("u_viewprojection", uniformData.viewprojection);
    shader->SetVector3("u_ambient_light", uniformData.ambient_light);

    // Material properties
    shader->SetVector3("u_Ka", Ka);
    shader->SetVector3("u_Kd", Kd);
    shader->SetVector3("u_Ks", Ks);
    shader->SetFloat("u_shininess", shininess);

    int max_lights = 2; // u_light_positions[2], u_light_colors[2] in shaders
    int available_lights = static_cast<int>(uniformData.lights.size());
    int num_lights_to_use = std::min(std::min(uniformData.num_lights, available_lights), max_lights);

    shader->SetInt("u_num_lights", num_lights_to_use);

    std::vector<float> light_positions;
    std::vector<float> light_colors;
    light_positions.reserve(num_lights_to_use * 3);
    light_colors.reserve(num_lights_to_use * 3);

    for (int i = 0; i < num_lights_to_use; ++i) {
        const auto& light = uniformData.lights[i];
        light_positions.push_back(light.position.x);
        light_positions.push_back(light.position.y);
        light_positions.push_back(light.position.z);
        light_colors.push_back(light.color.x);
        light_colors.push_back(light.color.y);
        light_colors.push_back(light.color.z);
    }
    if (num_lights_to_use > 0) {
        shader->SetUniform3Array("u_light_positions", light_positions.data(), num_lights_to_use);
        shader->SetUniform3Array("u_light_colors", light_colors.data(), num_lights_to_use);
    }

    // Toggles
    shader->SetInt("u_use_color_texture", uniformData.use_color_texture ? 1 : 0);
    shader->SetInt("u_use_specular_texture", uniformData.use_specular_texture ? 1 : 0);
    shader->SetInt("u_use_normal_texture", uniformData.use_normal_texture ? 1 : 0);

    if (color_texture && uniformData.use_color_texture) {
        shader->SetTexture("u_color_texture", color_texture);
    }

    if (normal_texture && uniformData.use_normal_texture) {
        shader->SetTexture("u_normal_texture", normal_texture);
    }
}

void Material::Disable() {
    if (shader) {
        shader->Disable();
    }
}