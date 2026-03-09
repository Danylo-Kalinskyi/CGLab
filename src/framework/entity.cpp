#include "entity.h"
#include "camera.h"
#include "image.h"
#include "mesh.h"

Entity::Entity() {
    model.SetIdentity();
}

void Entity::Init(Mesh* m, Material* mat) {
    mesh = m;
    material = mat;
    position = Vector3(0.0, 0.0, 0.0);
    translation = Vector3(0.0, 0.0, 0);
    rotation = Vector3(0.0, 0.0, 0.0);
    scale = Vector3(1, 1, 1);
}

void Entity::Update(float dt) {
    Matrix44 T, R, S; 
    T.MakeTranslationMatrix(position.x, position.y, position.z);
    S.MakeScaleMatrix(scale.x, scale.y, scale.z);
    model = T * R * S;

}

// LAB 5 - new Render function
void Entity::Render(sUniformData& uniformData) {
    if (!material || !mesh) { return; }

    // Update model matrix
    uniformData.model = model;

    // Enable material and upload uniforms
    material->Enable(uniformData);

    mesh->Render();

    // Note: Disable is called after rendering all lights in multipass
}

// LAB 4 - compatibility Render function
void Entity::Render(Camera* camera) {
    if (!material || !mesh) { return; }

    // For lab4, assume shader is already enabled
    if (material->shader) {
        material->shader->SetMatrix44("u_model", model);
        material->shader->SetMatrix44("u_viewprojection", camera->viewprojection_matrix);

        if (material->color_texture) {
            material->color_texture->Bind();
            material->shader->SetInt("u_texture", 0);
        }

        mesh->Render();
    }
}

