#pragma once
#include <framework/image.h>
#include <framework/framework.h>

class ParticleSystem {

    static const int MAX_PARTICLES = 1000;

    struct Particle {
        Vector2 position;
        Vector2 velocity; // Normalized speed and direction of the particle
        Color color;
        float acceleration;
        float ttl; // Time left until the particle expires
        bool inactive; // Particle is not used/expired, so it can be recreated
    };

    Particle particles[MAX_PARTICLES];

public:
    void Init(int w, int h);
    void Render(Image* framebuffer, int w, int h);
    void Update(float dt);
};
