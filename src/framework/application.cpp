#include "application.h"
#include "mesh.h"
#include "shader.h"
#include "utils.h"
#include <vector>
#include <algorithm>

Application::Application(const char* caption, int width, int height)
{
    this->window = createWindow(caption, width, height);

    int w, h;
    SDL_GetWindowSize(window, &w, &h);

    this->mouse_state = 0;
    this->time = 0.f;
    this->window_width = w;
    this->window_height = h;
    this->keystate = SDL_GetKeyboardState(nullptr);

    this->framebuffer.Resize(w, h);
    
}

Application::~Application()
{
}

void Application::Init(void) {
    std::cout << "Initiating app..." << std::endl;

    // Seed random generator
    std::srand(std::time(nullptr));
   
    // LAB 2 - Position the camera so the mesh is visible
    camera.SetOrthographic(-1,1,1,-1,-1,1);
    camera.SetPerspective(60, window_width / (float)window_height, 0.1, 100);
    camera.LookAt(Vector3(0, 1, 5), Vector3(0, 1, 0), Vector3(0, 1, 0));

    // LAB 2 - Initiate mesh and 3 entities
    mesh = new Mesh();
    mesh->LoadOBJ("meshes/lee.obj");

    for (int i = 0; i < 3; ++i){
        Entity e;
        e.Init(mesh);
        entities.push_back(e);
    }    

}

void Application::Render(void) {
    framebuffer.Fill(Color::BLACK);   

    if (one_entity && !entities.empty()) {
        entities[0].Render(&framebuffer, &camera, entities[0].color);
    }
    else {
        for (auto& e : entities) {
            e.Render(&framebuffer, &camera, e.color);
        }
    }

    framebuffer.Render();
}

void Application::Update(float seconds_elapsed){
    for (auto& e : entities) {e.Update(seconds_elapsed);} // LAB 2
}

void Application::OnMouseButtonDown(SDL_MouseButtonEvent event){
    int correctedY = (window_height - 1) - event.y;
    Vector2 mousePos(event.x, (float)correctedY);
    if (event.button == SDL_BUTTON_LEFT) { orbiting = true; } // Orbit around target
    else if (event.button == SDL_BUTTON_RIGHT) { move = true; } // Move target
}

void Application::OnMouseButtonUp(SDL_MouseButtonEvent event) {
    if (event.button == SDL_BUTTON_LEFT) { orbiting = false; }
    if (event.button == SDL_BUTTON_RIGHT) { move = false; }
}

void Application::OnMouseMove(SDL_MouseMotionEvent event) {
    int correctedY = (window_height - 1) - event.y;
    Vector2 mousePos((float)event.x, (float)correctedY);
    mouse_delta = mousePos - mouse_position;

    if (orbiting) {
        yaw -= mouse_delta.x * 0.01f;
        pitch -= mouse_delta.y * 0.01f;

        float limit = 1.5f; 
        if (pitch > limit) pitch = limit;
        if (pitch < -limit) pitch = -limit;

        Vector3 offset;
        offset.x = distance * cos(pitch) * sin(yaw);
        offset.y = distance * sin(pitch);
        offset.z = distance * cos(pitch) * cos(yaw);

        camera.eye = camera.center + offset;
        camera.UpdateViewMatrix();
    }
    else if (move) {
        Vector3 delta(-mouse_delta.x * 0.01f, mouse_delta.y * 0.01f, 0);
        camera.Move(delta);
    }
    mouse_position = mousePos;
}

void Application::OnKeyPressed(SDL_KeyboardEvent event)
{
    switch(event.keysym.sym) {
        // case SDLK_ESCAPE: exit(0); break;
        case SDLK_1: 
            one_entity = true; 
            // Reset
            if (!entities.empty()) {
                entities.resize(1); // remove extras 
                entities[0].Init(mesh); // reset first entity
            }
            break; // draw single entity
        case SDLK_2: 
            one_entity = false; 
            // Reset
            for (auto& e : entities) e.Init(mesh);
            while ((int)entities.size() < 3) {   // 3 entities
                Entity e;
                e.Init(mesh);
                entities.push_back(e);
            }
            break; // draw multiple animated entities
        case SDLK_n: current_property = C_NEAR; break; // set current property to camera near
        case SDLK_f: current_property = C_FAR; break; // set current property to camera far
        case SDLK_v: current_property = C_FOV; break; // set current property to FOV
        case SDLK_PLUS: 
            if (current_property == C_NEAR) camera_near += 0.1;
            else if (current_property == C_FAR) camera_far += 0.1;
            else if (current_property == C_FOV) camera_fov += 1.0;
            break; // increase current property
        case SDLK_MINUS: 
            if (current_property == C_NEAR) camera_near -= 0.1;
            else if (current_property == C_FAR) camera_far -= 0.1;
            else if (current_property == C_FOV) camera_fov -= 1.0;
            break; // decrease current property
    }
    camera.SetPerspective(camera_fov, window_width / (float)window_height, camera_near, camera_far);
}

void Application::OnWheel(SDL_MouseWheelEvent event) {}
void Application::OnFileChanged(const char* filename) { Shader::ReloadSingleShader(filename); }


