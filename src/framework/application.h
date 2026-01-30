/* + This class encapsulates the application, is in charge of creating the data, getting the user input, process the update and render.
*/

#pragma once

#include "main/includes.h"
#include "framework.h"
#include "image.h"
#include "button.h"
#include "particle.h"
#include "entity.h"

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

    // Drawing State
    Button::ButtonType drawingTool = Button::PENCIL;
    Color borderColor = Color::WHITE;
    Color fillColor = Color::WHITE;
	bool fill = false;
    int borderWidth = 2;
    
    bool is_drawing = false;
    Vector2 draw_start_pos;   // Starting position for line/rectangle tools
    Vector2 draw_prev_pos;    // Previous position for pencil/eraser tools

    // Event Handlers
    void OnKeyPressed(SDL_KeyboardEvent event);
    void OnMouseButtonDown(SDL_MouseButtonEvent event);
    void OnMouseButtonUp(SDL_MouseButtonEvent event);
    void OnMouseMove(SDL_MouseMotionEvent event);
    void OnWheel(SDL_MouseWheelEvent event);
    void OnFileChanged(const char* filename);

    // Images and Buffers
    Image framebuffer;
    Image canvas;

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

    Vector2 GetWindowSize()
    {
        int w,h;
        SDL_GetWindowSize(window,&w,&h);
        return Vector2(float(w), float(h));
    }

    ParticleSystem particles;
    bool animation_mode = false;
    
    // Images
    Image circleImage;
    Image eraserImage;
    Image lineImage;
    Image pencilImage;
    Image rectangleImage;
    Image triangleImage;
    Image loadImage;
    Image saveImage;
    Image clearImage;

    // Colors
    Image blackImage;
    Image cyanImage;
    Image greenImage;
    Image pinkImage;
    Image redImage;
    Image whiteImage;
    Image yellowImage;
    
    std::vector<Button> buttons;

    Entity entity;
    Camera camera;
};
