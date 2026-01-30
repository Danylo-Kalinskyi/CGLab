#include "particle.h"
#include "image.h"
#include <cstdlib>
#include <cmath>

void ParticleSystem::Init(int w, int h) {
	// added attributes: window width and height
	width = w;
	height = h;

	for (int i = 0; i < MAX_PARTICLES; i++) {
		particles[i].position = Vector2(rand() % width, rand() % height);
		particles[i].velocity = Vector2(0, -((rand() % 75) + 1)); // we add 1 to avoid still particles
		particles[i].color = Color(155 + rand()%100, 155 + rand() % 100, 155 + rand() % 100); // randomize light colors
		particles[i].acceleration = 1.5;
		particles[i].ttl = 100;
		particles[i].inactive = false;
		
	}
}

void ParticleSystem::Render(Image* framebuffer)
{
	for (int i = 0; i < MAX_PARTICLES; i++) {
		if (particles[i].inactive) {
			continue; // we skip inactive particles
		}
		// we convert position to integers
		int x = (int)particles[i].position.x;
		int y = (int)particles[i].position.y;
		// we prevent writing outside the window
		if (x < 0) { x = 0; }
		else if (x >= width) { x = width - 1; }
		if (y < 0) { y = 0; }
		else if (y >= height) { y = height - 1; }

		framebuffer->SetPixel(x, y, particles[i].color);
	}
}

void ParticleSystem::Update(float dt)
{
	for (int i = 0; i < MAX_PARTICLES; i++) {
		if (particles[i].inactive) { continue; };

		particles[i].velocity.y -= particles[i].acceleration * dt;
		particles[i].position += particles[i].velocity * dt;
		particles[i].ttl -= dt;

		if (particles[i].ttl <= 0 || particles[i].position.y < 0) {
			particles[i].position = Vector2(rand() % width, height - 1); // respawn at the top of the window
			particles[i].velocity = Vector2(0, - ((rand() % 75) + 1));
			particles[i].ttl = 100;
		}
	}
}