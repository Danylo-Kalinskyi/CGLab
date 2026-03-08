#pragma once
#include "mesh.h"
#include "image.h"
#include "camera.h"
#include "shader.h"
#include "texture.h"

class Entity {
public:
    Mesh* mesh = nullptr;
    Matrix44 model;
    Vector3 position;
    Vector3 rotation;
    Vector3 translation = (0, 0, 0);
    Vector3 scale = Vector3(1, 1, 1);

    // Randomly assign an animation type
    enum AnimationType { ROTATE, SCALE, TRANSLATE } animType;
    Vector3 animAxis;
    float animSpeed;

    // LAB 4 - shader and texture
    Shader* shader = nullptr;
    Texture* texture = nullptr;

    Entity();
    void Init(Mesh* m, Shader* s, Texture* t);
    void Render(Camera* camera); // LAB 4 - 2.5
    void Update(float dt);

    // LAB 3.4
    bool use_texture = true;

    // LAB 3 - interactivity
    enum class eRenderMode { POINTCOULD, WIREFRAME, TRIANGLES, TRIANGLES_INTERPOLATED };
    eRenderMode mode = eRenderMode::TRIANGLES_INTERPOLATED;
    bool useTexture = true;
    bool useOcclusion = true;
    bool interpolateUVs = true;
};