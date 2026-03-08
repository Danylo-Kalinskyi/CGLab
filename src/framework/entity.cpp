#include "entity.h"
#include "camera.h"
#include "image.h"
#include "mesh.h"

Entity::Entity() {
    model.SetIdentity();
}

void Entity::Init(Mesh* m, Shader* s, Texture* t) {
    mesh = m;
    shader = s;
    texture = t;
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

// LAB 4 - 2.5 new Render function
void Entity::Render(Camera* camera) {
    if (!shader || !mesh) { return; }

    // pass Model and ViewProjection matrices to GPU
    shader->SetMatrix44("u_model", model);
    shader->SetMatrix44("u_viewprojection", camera->viewprojection_matrix);

    // Bind the texture to the GPU
    if (texture) {
        texture->Bind();
        shader->SetInt("u_texture", 0);
    }

    mesh->Render();
}

