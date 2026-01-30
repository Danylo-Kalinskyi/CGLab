#include "entity.h"
#include "camera.h"
#include "image.h"
#include "mesh.h"

Entity::Entity() {
    model.SetIdentity();
}

void Entity::Update(float dt) {
    // Example animation (rotation)
    Matrix44 R;
    R.MakeRotationMatrix(dt * 0.5f, Vector3(0, 1, 0));
    model = R * model;
}

void Entity::Render(Image* framebuffer, Camera* camera, const Color& c) {
    if (!mesh) return;
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

        framebuffer->DrawLineDDA(p0.x, p0.y, p1.x, p1.y, c);
        framebuffer->DrawLineDDA(p1.x, p1.y, p2.x, p2.y, c);
        framebuffer->DrawLineDDA(p2.x, p2.y, p0.x, p0.y, c);
    }
}
