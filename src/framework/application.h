/* + This class encapsulates the application, is in charge of creating the data, getting the user input, process the update and render.
*/

#pragma once

#include "main/includes.h"
#include "framework.h"
#include "image.h"
#include "button.h"
#include "particle.h"
#include "entity.h"
#include "shader.h"

class Application
{
public:

    // Window
    SDL_Window* window = nullptr;
    int window_width;
    int window_height;

    float time;

    // Input
    const Uint8* keystate;
    int mouse_state;
    Vector2 mouse_position; // Last mouse position
    Vector2 mouse_delta;

    // Event Handlers
    void OnKeyPressed(SDL_KeyboardEvent event);
    void OnMouseButtonDown(SDL_MouseButtonEvent event);
    void OnMouseButtonUp(SDL_MouseButtonEvent event);
    void OnMouseMove(SDL_MouseMotionEvent event);
    void OnWheel(SDL_MouseWheelEvent event);
    void OnFileChanged(const char* filename);

    // Images and Buffers
    Image framebuffer;

    // Constructor and main methods
    Application(const char* caption, int width, int height);
    ~Application();

    void Init( void );
    void Render( void );
    void Update( float dt );

    // Window Control
    void SetWindowSize(int width, int height) {
        glViewport( 0,0, width, height );
        this->window_width = width;
        this->window_height = height;
        this->framebuffer.Resize(width, height);
    }

    Vector2 GetWindowSize() {
        int w,h;
        SDL_GetWindowSize(window,&w,&h);
        return Vector2(float(w), float(h));
    }

    // LAB 2 - objects (entities) 
    std::vector<Entity> entities;
    Mesh* mesh = nullptr;
    bool one_entity = false; // Initially, we draw 3 entities
    float total_pitch = 0.0;
    float yaw = 0.0;
    float pitch = 0.0;
    float distance = 5.0; // Distance from center to eye

    // LAB 2 - camera control
    Camera camera;
    bool orbiting = false;  // Left mouse button drag to orbit
    bool move = false;   // Right mouse button drag to move target
    float camera_fov = 60;      // Field of view
    float camera_near = 0.1;
    float camera_far = 100;
    enum CameraProperty { C_NEAR, C_FAR, C_FOV };
    CameraProperty current_property = C_FOV; // default

    // LAB 3 - triangular meshes
    FloatImage z_buffer;

    // LAB 4 - shader
    Shader* shader = nullptr;
};
