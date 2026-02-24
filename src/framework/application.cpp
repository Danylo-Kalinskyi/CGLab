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
   
    // LAB 4 - create quad mesh and load shaders
    mesh = new Mesh();
    mesh->CreateQuad();
    shader = Shader::Get("shaders/quad.vs", "shaders/quad.fs");

    /*
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
    */

}

void Application::Render(){
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shader->Enable();
    shader->SetFloat("u_time", time);
    shader->SetVector2("u_res", Vector2(window_width, window_height));
    mesh->Render();
    shader->Disable();
}

void Application::Update(float seconds_elapsed){
    time += seconds_elapsed;
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
        // LAB 4 - task selection
        case SDLK_1: current_task = 1; break;
        case SDLK_2: current_task = 2; break;
        case SDLK_3: current_task = 3; break;
        case SDLK_4: current_task = 4; break;

        // LAB 4 - subtask selection
        case SDLK_a: current_subtask = 0; break;
        case SDLK_b: current_subtask = 1; break;
        case SDLK_c: current_subtask = 2; break;
        case SDLK_d: current_subtask = 3; break;
        case SDLK_e: current_subtask = 4; break;
        case SDLK_f: current_subtask = 5; break;

        // LAB 4 & 5 - switch lab
        case SDLK_l:
            is_lab5 = !is_lab5;
            break;


        /*
        
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
        case SDLK_t: // texture
            for (auto& e : entities) e.useTexture = !e.useTexture;
            break;

        case SDLK_z: // Toggle Z-buffer
            for (auto& e : entities) e.useOcclusion = !e.useOcclusion;
            break;

        case SDLK_c: // Toggle UV interpolation
            for (auto& e : entities) e.interpolateUVs = !e.interpolateUVs;
            break;

        case SDLK_w: // Toggle wireframe / filled
            for (auto& e : entities) {
                if (e.mode == Entity::eRenderMode::WIREFRAME) { e.mode = Entity::eRenderMode::TRIANGLES_INTERPOLATED; }
                else { e.mode = Entity::eRenderMode::WIREFRAME; }
            }
            break;
        camera.SetPerspective(camera_fov, window_width / (float)window_height, camera_near, camera_far);
        */
    }
    
}

void Application::OnWheel(SDL_MouseWheelEvent event) {
    float dy = event.preciseY;
    float zoomSpeed = 0.2;

    Vector3 dir = camera.center - camera.eye;
    float dist = dir.Length();
    dir.Normalize();
    dist -= zoomSpeed * dy;

    if (dist < 0.1) dist = 0.1;

    camera.eye = camera.center - dir * dist;
    camera.UpdateViewMatrix();
}

void Application::OnFileChanged(const char* filename) { Shader::ReloadSingleShader(filename); }