#include "particle.h"
#include <framework/image.h>
#include <cstdlib>
#include <cmath>

void ParticleSystem::Init(int w, int h)
{
	for (int i = 0; i < MAX_PARTICLES; i++) {
		particles[i].position = Vector2(rand() % w, rand() % h);
		particles[i].velocity = Vector2(0,-50);
		particles[i].color = Color(255, 255, 255);
		particles[i].acceleration = 1.0;
		particles[i].ttl = 100;
		particles[i].inactive = false;
	}
}

void ParticleSystem::Render(Image* framebuffer, int w, int h)
{
	for (int i = 0; i < MAX_PARTICLES; i++) {
		if (particles[i].inactive) {
			continue;
		}
		int x = (int)particles[i].position.x;
		if (x < 0) { x = 0; }
		else if (x >= w) { x = w - 1; }
		int y = (int)particles[i].position.y;
		if (y < 0) { y = 0; }
		else if (y >= h) { y = h - 1; }
		framebuffer->SetPixel(x, y, particles[i].color);
	}
}

void ParticleSystem::Update(float dt)
{
	for (int i = 0; i < MAX_PARTICLES; i++) {
		if (particles[i].inactive) { continue; };
		particles[i].position += particles[i].velocity * dt;
		particles[i].ttl -= dt;
		if (particles[i].ttl <= 0) { particles[i].inactive = true; }
	}
}