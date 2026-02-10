#include "entity.h"
#include "camera.h"
#include "image.h"
#include "mesh.h"

Entity::Entity() {
    model.SetIdentity();
}

void Entity::Update(float dt) {
    if (animType == ROTATE) {
        rotation.x += animAxis.x * animSpeed * dt;
        rotation.y += animAxis.y * animSpeed * dt;
        rotation.z += animAxis.z * animSpeed * dt;
    }
    else if (animType == SCALE) {
        scale.x += scale.x * dt;
        scale.y += scale.y * dt;
        scale.z += scale.z * dt;
    }
    else if (animType == TRANSLATE) {
        position.x += translation.x * dt;
        position.y += translation.y * dt;
        position.z += translation.z * dt;
    }
    Matrix44 T, R, S;
    T.MakeTranslationMatrix(position.x, position.y, position.z);
    R.MakeRotationMatrix(rotation.y, Vector3(0, 1, 0));
    S.MakeScaleMatrix(scale.x, scale.y, scale.z);
    model = T * R * S;
}

void Entity::Render(Image* framebuffer, Camera* camera, const Color& c) {
    if (!mesh) return;
    
    auto inside = [](const Vector3& p) {
        return p.x >= -1 && p.x <= 1 &&
            p.y >= -1 && p.y <= 1 &&
            p.z >= -1 && p.z <= 1;
        };
    const auto& vertices = mesh->GetVertices();

    for (int i = 0; i < vertices.size(); i += 3) {
        Vector3 v0 = vertices[i];
        Vector3 v1 = vertices[i + 1];
        Vector3 v2 = vertices[i + 2];

        // Transform to world space
        v0 = model * v0;
        v1 = model * v1;
        v2 = model * v2;

        // Project to screen
        Vector3 p0 = camera->ProjectVector(v0);
        Vector3 p1 = camera->ProjectVector(v1);
        Vector3 p2 = camera->ProjectVector(v2);

        if (!inside(p0) || !inside(p1) || !inside(p2))
            continue;

        Vector2 s0((p0.x * 0.5f + 0.5f) * framebuffer->width,
            (p0.y * 0.5f + 0.5f) * framebuffer->height);

        Vector2 s1((p1.x * 0.5f + 0.5f) * framebuffer->width,
            (p1.y * 0.5f + 0.5f) * framebuffer->height);

        Vector2 s2((p2.x * 0.5f + 0.5f) * framebuffer->width,
            (p2.y * 0.5f + 0.5f) * framebuffer->height);

        // Draw in screen space
        framebuffer->DrawLineDDA(s0.x, s0.y, s1.x, s1.y, c);
        framebuffer->DrawLineDDA(s1.x, s1.y, s2.x, s2.y, c);
        framebuffer->DrawLineDDA(s2.x, s2.y, s0.x, s0.y, c);
    }

}

// ADDITIONAL FUNCTION
void Entity::RandomAnim() {
    // We randomly choose an animation axis
    int axis = rand() % 3;
    if (axis == 0) { animAxis = Vector3(1, 0, 0); 
    }
    else if (axis == 1) {
        animAxis = Vector3(0, 1, 0);
    }
    else { animAxis = Vector3(0, 0, 1); }

    // Random speed
    animSpeed = 0.5 + ((float)rand() / RAND_MAX) * 1;

    // We randomly choose an animation type
    int choice = rand() % 3;

    if (choice == 0) {
        animType = ROTATE;
        rotation = Vector3(
            ((float)rand() / RAND_MAX) * 3,
            ((float)rand() / RAND_MAX) * 3,
            ((float)rand() / RAND_MAX) * 3
        );

    }
    else if (choice == 1) {
        animType = SCALE;
        float s = 1 + ((float)rand() / RAND_MAX) * 0.5;
        scale = Vector3(s, s, s);
    }
    else {
        animType = TRANSLATE;
        translation = animAxis * (0.5 + ((float)rand() / RAND_MAX));
    }
}
