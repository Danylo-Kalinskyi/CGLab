#pragma once
#include "mesh.h"
#include "image.h"
#include "camera.h"

class Entity {
public:
    Mesh* mesh = nullptr;
    Matrix44 model;

    Entity();
    void Render(Image* framebuffer, Camera* camera, const Color& color);
    void Update(float dt);
};