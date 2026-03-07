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
    // camera.SetOrthographic(-1, 1, 1, -1, -1, 1);
    camera.SetPerspective(60, window_width / (float)window_height, 0.1, 100);
    camera.LookAt(Vector3(0, 1, 2), Vector3(0, 0, 0), Vector3(0, 1, 0)); // Closer to the single mesh
    camera.UpdateViewMatrix();
    
    // Initialize camera control variables to match the initial setup
    Vector3 initial_offset = camera.eye - camera.center;
    distance = initial_offset.Length();
    yaw = atan2(initial_offset.x, initial_offset.z);
    pitch = asin(initial_offset.y / distance);
   
    // LAB 4 - 2.1 create quad mesh and load shaders
    meshQuad = new Mesh();
    meshQuad->CreateQuad();
    shaderQuad = Shader::Get("shaders/quad.vs", "shaders/quad.fs");

    // LAB 4 - 2.3 image filters
    fruitsImage = Texture::Get("images/fruits.png");

    // LAB 4 - 2.5 mesh and GPU texture
    meshLee = new Mesh();
    meshLee->LoadOBJ("meshes/lee.obj");
    rasterShader = Shader::Get("shaders/raster.vs", "shaders/raster.fs");
    texLee = Texture::Get("textures/lee_color_specular.tga");

    // LAB 5 - Load additional textures and shaders
    Shader* gouraudShader = Shader::Get("shaders/gouraud.vs", "shaders/gouraud.fs");
    Shader* phongShader = Shader::Get("shaders/phong.vs", "shaders/phong.fs");
    Texture* texLeeNormal = Texture::Get("textures/lee_normal.tga");

    // LAB 4 - 2.5 entities
    Entity e;
    e.Init(meshLee, phongShader, texLee); // Use Phong shader by default
    // Set up material with textures
    if (e.material) {
        e.material->color_texture = texLee;
        e.material->specular_texture = texLee; // Same texture contains specular in alpha
        e.material->normal_texture = texLeeNormal;
    }
    // Position the single mesh closer to camera
    e.position = Vector3(0.0f, 0.0f, 0.0f);
    e.model.SetIdentity();
    e.model.MakeTranslationMatrix(0.0f, 0.0f, 0.0f);
    entities.push_back(e);

    // LAB 5 - Initialize lighting
    uniformData.ambient_light = ambient_light;
    uniformData.num_lights = num_lights;
    uniformData.use_color_texture = use_color_texture;
    uniformData.use_specular_texture = use_specular_texture;
    uniformData.use_normal_texture = use_normal_texture;

    // Set up lights
    lights[0].position = Vector3(5.0f, 5.0f, 5.0f);
    lights[0].color = Vector3(1.0f, 1.0f, 1.0f);
    lights[1].position = Vector3(-5.0f, 5.0f, -5.0f);
    lights[1].color = Vector3(1.0f, 0.5f, 0.5f);
    lights[2].position = Vector3(0.0f, -5.0f, 5.0f);
    lights[2].color = Vector3(0.5f, 1.0f, 0.5f);
    lights[3].position = Vector3(5.0f, -5.0f, -5.0f);
    lights[3].color = Vector3(0.5f, 0.5f, 1.0f);
    lights[4].position = Vector3(-5.0f, -5.0f, 5.0f);
    lights[4].color = Vector3(1.0f, 1.0f, 0.5f);
    for (int i = 0; i < 10; ++i) {
        uniformData.lights[i] = lights[i];
    }
}

void Application::Render(){ // LAB 4
    // Clear buffers
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (!is_lab5) {
        if (!shaderQuad) { return; }
        shaderQuad->Enable();
        shaderQuad->SetFloat("u_time", time);
        shaderQuad->SetVector2("u_res", Vector2(window_width, window_height));
        // interactivity variables
        shaderQuad->SetInt("u_task", u_task);
        shaderQuad->SetInt("u_subtask", u_subtask);
        if (!fruitsImage) { return; }
        fruitsImage->Bind();
        shaderQuad->SetInt("u_tex", 0);
        meshQuad->Render();
        shaderQuad->Disable();
    }
    /*
    else {
        // IMPORTANT: Enable Depth Test for correct occlusions
        glEnable(GL_DEPTH_TEST);
        if (!rasterShader) { return; }
        rasterShader->Enable();
        rasterShader->SetMatrix44("u_viewprojection", camera.viewprojection_matrix);
        if (!texLee) { return; }
        texLee->Bind();      
        rasterShader->SetInt("u_texture", 0);
        // Render all entities
        for (auto& e : entities) {
            rasterShader->SetMatrix44("u_model", e.model);
            e.Render(&camera);
        }
        // Disable depth test if you plan to draw 2D UI/Quads later
        // glDisable(GL_DEPTH_TEST);
        rasterShader->Disable();
    }
    */
    else { 
        // IMPORTANT: Enable Depth Test for correct occlusions
        glEnable(GL_DEPTH_TEST);

        // Update uniform data
        uniformData.viewprojection = camera.viewprojection_matrix;
        uniformData.eye = camera.eye;
        uniformData.ambient_light = ambient_light;
        uniformData.use_color_texture = use_color_texture;
        uniformData.use_specular_texture = use_specular_texture;
        uniformData.use_normal_texture = use_normal_texture;

        // Multipass rendering for multiple lights
        for (int light_index = 0; light_index < num_lights; ++light_index) {
            // Set up blending for additional lights
            if (light_index > 0) {
                glEnable(GL_BLEND);
                glBlendFunc(GL_ONE, GL_ONE); // Additive blending
            }

            // Update uniform data for this light
            uniformData.num_lights = 1; // Only one light per pass
            uniformData.lights[0] = lights[light_index];

            // Render all entities with this light
            for (auto& e : entities) {
                e.Render(uniformData);
            }

            if (light_index > 0) {
                glDisable(GL_BLEND);
            }
        }

        // Disable depth test if you plan to draw 2D UI/Quads later
        glDisable(GL_DEPTH_TEST);
        
    }
}

void Application::Update(float seconds_elapsed){
    time += seconds_elapsed;
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
    // LAB 4 & 5 - switch lab
    if (event.keysym.sym == SDLK_l) {
        is_lab5 = !is_lab5;
        // Reset to Phong shader when switching to Lab 5
        if (is_lab5) {
            Shader* phongShader = Shader::Get("shaders/phong.vs", "shaders/phong.fs");
            for (auto& e : entities) {
                if (e.material) {
                    e.material->shader = phongShader;
                }
            }
        }
        return;
    }

    if (event.keysym.sym == SDLK_ESCAPE) {
        exit(0);
        return;
    }

    if (is_lab5) {
        // LAB 5 - Lighting controls
        switch(event.keysym.sym) {
            case SDLK_g: // Gouraud shading
                {
                    Shader* gouraudShader = Shader::Get("shaders/gouraud.vs", "shaders/gouraud.fs");
                    for (auto& e : entities) {
                        if (e.material) {
                            e.material->shader = gouraudShader;
                        }
                    }
                }
                break;
            case SDLK_p: // Phong shading
                {
                    Shader* phongShader = Shader::Get("shaders/phong.vs", "shaders/phong.fs");
                    for (auto& e : entities) {
                        if (e.material) {
                            e.material->shader = phongShader;
                        }
                    }
                }
                break;
            case SDLK_c: // Toggle color texture
                use_color_texture = !use_color_texture;
                break;
            case SDLK_s: // Toggle specular texture
                use_specular_texture = !use_specular_texture;
                break;
            case SDLK_n: // Toggle normal texture
                use_normal_texture = !use_normal_texture;
                break;
            case SDLK_1: num_lights = 1; break;
            case SDLK_2: num_lights = 2; break;
            case SDLK_3: num_lights = 3; break;
            case SDLK_4: num_lights = 4; break;
            case SDLK_5: num_lights = 5; break;
        }
    } else {
        // LAB 4 - task selection
        switch(event.keysym.sym) {
            case SDLK_1: u_task = 1; u_subtask = 1; break;
            case SDLK_2: u_task = 2; u_subtask = 1; break;
            case SDLK_3: u_task = 3; u_subtask = 1; break;
            case SDLK_4: u_task = 4; u_subtask = 1; break;

            // LAB 4 - subtask selection
            case SDLK_a: u_subtask = 1; break;
            case SDLK_b: u_subtask = 2; break;
            case SDLK_c: u_subtask = 3; break;
            case SDLK_d: u_subtask = 4; break;
            case SDLK_e: u_subtask = 5; break;
            case SDLK_f: u_subtask = 6; break;
        }
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
