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
    gpu_texture = t;

    // color = Color(56 + rand() % 200, 56 + rand() % 200, 56 + rand() % 200); // We avoid dark colors for visibility
    
    // Random position inside [-2, 2] for x, y and z
    position.x = ((float)rand() / RAND_MAX) * 4 - 2;
    position.y = ((float)rand() / RAND_MAX) * 4 - 2;
    position.z = ((float)rand() / RAND_MAX) * 4 - 2;

    model.SetIdentity();
    model.MakeTranslationMatrix(position.x, position.y, position.z);
}

// void Entity::Update(float dt) {
//     if (animType == ROTATE) {
//         rotation.x += animAxis.x * animSpeed * dt;
//         rotation.y += animAxis.y * animSpeed * dt;
//         rotation.z += animAxis.z * animSpeed * dt;
//     }
//     else if (animType == SCALE) {
//         scale.x += animSpeed * dt;
//         scale.y += animSpeed * dt;
//         scale.z += animSpeed * dt;
//     }
//     else if (animType == TRANSLATE) {
//         position.x += translation.x * dt;
//         position.y += translation.y * dt;
//         position.z += translation.z * dt;
//     }
//     Matrix44 T, R, S;
//     T.MakeTranslationMatrix(position.x, position.y, position.z);
//     R.MakeRotationMatrix(rotation.x + rotation.y + rotation.z, animAxis);
//     S.MakeScaleMatrix(scale.x, scale.y, scale.z);
//     model = T * R * S;

// }

void Entity::Render(Camera* camera) {
    if (!mesh || !shader) return;

    // pass matrices
    shader->SetMatrix44("u_model", model);
    shader->SetMatrix44("u_viewprojection", camera->viewprojection_matrix);

    // pass texture
    if (gpu_texture) {
        gpu_texture->Bind();
        shader->SetInt("u_texture", 0); // Texture unit 0
    }
    // Tell the mesh to draw itself using the currently enabled shader
    mesh->Render();

}

// void Entity::Render(Image* framebuffer, Camera* camera, const Color& c, FloatImage* z_buffer) {
//     if (!mesh) return;

//     const auto& vertices = mesh->GetVertices();

//     for (int i = 0; i < vertices.size(); i += 3) {
//         // transform to World Space
//         Vector3 v0 = model * vertices[i];
//         Vector3 v1 = model * vertices[i + 1];
//         Vector3 v2 = model * vertices[i + 2];

//         // project vertices to Clip Space (p.x and p.y are NDC, p.z is depth)
//         Vector3 p0 = camera->ProjectVector(v0);
//         Vector3 p1 = camera->ProjectVector(v1);
//         Vector3 p2 = camera->ProjectVector(v2);

//         // Simple Clipping (Skip if the whole triangle is behind the camera)
//         if (p0.z < -1 && p1.z < -1 && p2.z < -1) continue;

//         // Viewport Transform (NDC to Screen Pixels)
//         // We keep the Z value in the Vector3 for use in the interpolation function
//         Vector3 s0((p0.x * 0.5f + 0.5f) * framebuffer->width, (p0.y * 0.5f + 0.5f) * framebuffer->height, p0.z);
//         Vector3 s1((p1.x * 0.5f + 0.5f) * framebuffer->width, (p1.y * 0.5f + 0.5f) * framebuffer->height, p1.z);
//         Vector3 s2((p2.x * 0.5f + 0.5f) * framebuffer->width, (p2.y * 0.5f + 0.5f) * framebuffer->height, p2.z);

//         Color c0 = Color::RED;
//         Color c1 = Color::GREEN;
//         Color c2 = Color::BLUE;

//         // We load the mesh' UVs
//         const auto& uvs = mesh->GetUVs();
//         Vector2 uv0 = uvs[i];
//         Vector2 uv1 = uvs[i + 1];
//         Vector2 uv2 = uvs[i + 2];

//         switch (mode) {

//             case eRenderMode::WIREFRAME:
//                 framebuffer->DrawLineDDA(s0.x, s0.y, s1.x, s1.y, c);
//                 framebuffer->DrawLineDDA(s1.x, s1.y, s2.x, s2.y, c);
//                 framebuffer->DrawLineDDA(s2.x, s2.y, s0.x, s0.y, c);
//                 break;

//             case eRenderMode::TRIANGLES:
//                 framebuffer->DrawTriangle(Vector2(s0.x, s0.y), Vector2(s1.x, s1.y), Vector2(s2.x, s2.y), c, true, c);
//                 break;

//             case eRenderMode::TRIANGLES_INTERPOLATED:
//                 sTriangleInfo tri;
//                 tri.p0 = s0; tri.p1 = s1; tri.p2 = s2;
//                 tri.uv0 = uv0; tri.uv1 = uv1; tri.uv2 = uv2;

//                 if (useTexture) {
//                     tri.texture = texture;
//                     tri.c0 = tri.c1 = tri.c2 = Color::WHITE;
//                 }
//                 else {
//                     tri.texture = nullptr;
//                 }

//                 // color interpolation vs plain color 
//                 if (!useTexture) {
//                     if (interpolateUVs) {
//                         tri.c0 = Color::RED;
//                         tri.c1 = Color::GREEN;
//                         tri.c2 = Color::BLUE;
//                     }
//                     else {
//                         tri.c0 = tri.c1 = tri.c2 = this->color;
//                     }
//                 }

//                 framebuffer->DrawTriangleInterpolated(tri, useOcclusion ? z_buffer : nullptr);
//         }
//     }
// }

// ADDITIONAL FUNCTION
// void Entity::RandomAnim() {
//     // We randomly choose an animation axis
//     int axis = rand() % 3;
//     if (axis == 0) { animAxis = Vector3(1, 0, 0); 
//     }
//     else if (axis == 1) {
//         animAxis = Vector3(0, 1, 0);
//     }
//     else { animAxis = Vector3(0, 0, 1); }

//     // Random speed
//     animSpeed = 0.5 + ((float)rand() / RAND_MAX) * 1;

//     // We randomly choose an animation type
//     int choice = rand() % 3;

//     if (choice == 0) {
//         animType = ROTATE;
//         rotation = Vector3(
//             ((float)rand() / RAND_MAX) * 3,
//             ((float)rand() / RAND_MAX) * 3,
//             ((float)rand() / RAND_MAX) * 3
//         );

//     }
//     else if (choice == 1) {
//         animType = SCALE;
//         float s = 1 + ((float)rand() / RAND_MAX) * 0.5;
//         scale = Vector3(s, s, s);
//     }
//     else {
//         animType = TRANSLATE;
//         translation = animAxis * (0.5 + ((float)rand() / RAND_MAX));
//     }
// }

