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
    quadShader = Shader::Get("shaders/quad.vs", "shaders/quad.fs");

    
    // LAB 2 - Position the camera so the mesh is visible
    camera.SetOrthographic(-1,1,1,-1,-1,1);
    camera.SetPerspective(60, window_width / (float)window_height, 0.1, 100);
    camera.LookAt(Vector3(0, 1, 3), Vector3(0, 1, 0), Vector3(0, 1, 0));
 

    // LAB 4 - 2.3 image filters
    fruitsImage = Texture::Get("images/fruits.png");

    // LAB 4 - 2.5 shader, mesh and GPU texture
    rasterShader = Shader::Get("shaders/raster.vs", "shaders/raster.fs");
    meshLee = new Mesh();
    meshLee->LoadOBJ("meshes/lee.obj");
    texLee = Texture::Get("textures/lee_color_specular.tga");

    // LAB 4 - 2.4 Initialize Entity
    Entity e;
    e.Init(meshLee, rasterShader, texLee);
    entities.push_back(e);

}

void Application::Render(){
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (!is_lab5) { // LAB 4
        if (current_task == 4) { // Task 4
            glEnable(GL_DEPTH_TEST); // Enable occlusions for 3D
            glDepthFunc(GL_LESS);
            rasterShader->Enable();
            for (auto& e : entities) {
                e.Render(&camera); 
            }
            rasterShader->Disable();
        }
        else { // Tasks 1, 2 and 3
            glDisable(GL_DEPTH_TEST);
            if (!quadShader) { return; }
            quadShader->Enable();
            quadShader->SetFloat("u_time", time);
            quadShader->SetVector2("u_res", Vector2(window_width, window_height));
            // interactivity variables
            quadShader->SetInt("u_task", current_task);
            quadShader->SetInt("u_subtask", current_subtask);
            fruitsImage->Bind();
            quadShader->SetInt("u_tex", 0);
            mesh->Render();
            quadShader->Disable();
        }
    }
}

void Application::Update(float seconds_elapsed){
    time += seconds_elapsed;
    camera.UpdateViewMatrix();
    camera.UpdateProjectionMatrix();
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
        case SDLK_ESCAPE: exit(0); break;

        // LAB 4 - task selection
        case SDLK_1: current_task = 1; current_subtask = 0; break;
        case SDLK_2: current_task = 2; current_subtask = 0; break;
        case SDLK_3: current_task = 3; current_subtask = 0; break;
        case SDLK_4: current_task = 4; current_subtask = 0; break;

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
            //current_task = 1; current_subtask = 0;
            break;
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