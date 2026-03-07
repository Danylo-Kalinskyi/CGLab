#include "material.h"
#include "GL/glew.h"

Material::Material() {
    ka = Vector3(0.1f, 0.1f, 0.1f); // Default ambient
    kd = Vector3(0.8f, 0.8f, 0.8f); // Default diffuse
    ks = Vector3(0.5f, 0.5f, 0.5f); // Default specular
    ns = 32.0f; // Default shininess
}

Material::~Material() {
    // Textures are managed by the Texture class
}

void Material::Enable(const sUniformData& uniformData) {
    if (!shader) return;

    shader->Enable();

    // Upload matrices
    shader->SetMatrix44("u_model", uniformData.model);
    shader->SetMatrix44("u_viewprojection", uniformData.viewprojection);
    
    // Compute and upload normal matrix (inverse transpose of model matrix)
    Matrix44 normalMatrix = uniformData.model;
    normalMatrix.Inverse();
    normalMatrix.Transpose();
    shader->SetMatrix44("u_normal_matrix", normalMatrix);

    // Upload camera position
    shader->SetVector3("u_eye", uniformData.eye);

    // Upload ambient light
    shader->SetVector3("u_ambient_light", uniformData.ambient_light);

    // Upload lights
    shader->SetInt("u_num_lights", uniformData.num_lights);
    for (int i = 0; i < uniformData.num_lights; ++i) {
        char pos_name[32], color_name[32];
        sprintf(pos_name, "u_lights[%d].position", i);
        sprintf(color_name, "u_lights[%d].color", i);
        shader->SetVector3(pos_name, uniformData.lights[i].position);
        shader->SetVector3(color_name, uniformData.lights[i].color);
    }

    // Upload material properties
    shader->SetVector3("u_ka", ka);
    shader->SetVector3("u_kd", kd);
    shader->SetVector3("u_ks", ks);
    shader->SetFloat("u_ns", ns);

    // Upload texture flags
    shader->SetInt("u_use_color_texture", uniformData.use_color_texture ? 1 : 0);
    shader->SetInt("u_use_specular_texture", uniformData.use_specular_texture ? 1 : 0);
    shader->SetInt("u_use_normal_texture", uniformData.use_normal_texture ? 1 : 0);

    // Bind textures
    int texture_unit = 0;
    if (color_texture && uniformData.use_color_texture) {
        glActiveTexture(GL_TEXTURE0 + texture_unit);
        color_texture->Bind();
        shader->SetInt("u_color_texture", texture_unit++);
    }
    if (specular_texture && uniformData.use_specular_texture) {
        glActiveTexture(GL_TEXTURE0 + texture_unit);
        specular_texture->Bind();
        shader->SetInt("u_specular_texture", texture_unit++);
    }
    if (normal_texture && uniformData.use_normal_texture) {
        glActiveTexture(GL_TEXTURE0 + texture_unit);
        normal_texture->Bind();
        shader->SetInt("u_normal_texture", texture_unit++);
    }
}

void Material::Disable() {
    if (shader) {
        shader->Disable();
    }
}