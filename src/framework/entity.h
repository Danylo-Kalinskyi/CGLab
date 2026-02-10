#pragma once
#include "mesh.h"
#include "image.h"
#include "camera.h"

class Entity {
public:
    Mesh* mesh = nullptr;
    Matrix44 model;
    Vector3 position;
    Vector3 rotation;
    Vector3 translation = (0, 0, 0);
    Vector3 scale = Vector3(1, 1, 1);
    Color color;

    // Randomly assign an animation type
    enum AnimationType { ROTATE, SCALE, TRANSLATE } animType;
    Vector3 animAxis;
    float animSpeed;

    Entity();
    void Render(Image* framebuffer, Camera* camera, const Color& c);
    void Update(float dt);
    void RandomAnim();
};