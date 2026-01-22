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
    
    this->animation_mode = false;
    this->borderWidth = 2;
    this->fill = false;
}

Application::~Application()
{
}

void Application::Init(void)
{
    std::cout << "Initiating app..." << std::endl;
    
    // Init Canvas and Particles
    canvas.Resize(window_width, window_height);
    canvas.Fill(Color::BLACK);
    particles.Init(window_width, window_height);
    
    // Load Images
    pencilImage.LoadPNG("images/pencil.png", true);
    eraserImage.LoadPNG("images/eraser.png", true);
    lineImage.LoadPNG("images/line.png", true);
    rectangleImage.LoadPNG("images/rectangle.png", true);
    triangleImage.LoadPNG("images/triangle.png", true);
    clearImage.LoadPNG("images/clear.png", true);
    loadImage.LoadPNG("images/load.png", true);
    saveImage.LoadPNG("images/save.png", true);
    blackImage.LoadPNG("images/black.png", true);
    cyanImage.LoadPNG("images/cyan.png", true);
    greenImage.LoadPNG("images/green.png", true);
    pinkImage.LoadPNG("images/pink.png", true);
    redImage.LoadPNG("images/red.png", true);
    whiteImage.LoadPNG("images/white.png", true);
    yellowImage.LoadPNG("images/yellow.png", true);

    // Buttons Setup
    int buttonSize = 32;
    int buttonSpacing = 8;
    int startX = 10;
    int startY = window_height - (buttonSize + 10);

    auto addButton = [&](Image& img, Button::ButtonType type) {
        buttons.push_back(Button(img,
            Vector2(startX, startY),
            Vector2(startX + buttonSize, startY),
            Vector2(startX + buttonSize, startY + buttonSize),
            Vector2(startX, startY + buttonSize),
            type));
        startX += buttonSize + buttonSpacing;
    };

    // Tools
    addButton(pencilImage, Button::PENCIL);
    addButton(eraserImage, Button::ERASER);
    addButton(lineImage, Button::LINE);
    addButton(rectangleImage, Button::RECTANGLE);
    addButton(triangleImage, Button::TRIANGLE);
    addButton(clearImage, Button::CLEARIMAGE);
    addButton(loadImage, Button::LOADIMAGE);
    addButton(saveImage, Button::SAVEIMAGE);

    // Colors
    addButton(blackImage, Button::BLACKIMAGE);
    addButton(whiteImage, Button::WHITEIMAGE);
    addButton(redImage, Button::REDIMAGE);
    addButton(greenImage, Button::GREENIMAGE);
    addButton(cyanImage, Button::CYANIMAGE);
    addButton(pinkImage, Button::PINKIMAGE);
    addButton(yellowImage, Button::YELLOWIMAGE);

    // Default
    drawingTool = Button::PENCIL;
    borderColor = Color::WHITE;
    fillColor = Color::WHITE;
}

void Application::Render(void)
{
    framebuffer.Fill(Color::BLACK);
    framebuffer.DrawImage(canvas, 0, 0);
    
    // Particle animation
    if (animation_mode) {
        particles.Render(&framebuffer, framebuffer.width, framebuffer.height);
    }

    // UI
    for (auto& button : buttons) {
        button.Render(framebuffer);
    }
    framebuffer.DrawRect(window_width - 40, 10, 30, 30, Color::WHITE, 0, true, fill ? fillColor : borderColor);
    framebuffer.Render();
}

void Application::Update(float seconds_elapsed)
{
    particles.Update(seconds_elapsed);
}

void Application::OnMouseButtonDown(SDL_MouseButtonEvent event)
{
    if (event.button == SDL_BUTTON_LEFT) {
        int correctedY = (window_height - 1) - event.y;
        Vector2 mousePos(event.x, (float)correctedY);
        
        bool clicked_button = false;
        for (auto& button : buttons) {
            if (button.IsMouseInside(Vector2((float)event.x, (float)event.y))) {
                clicked_button = true;
                Button::ButtonType type = button.GetType();
                
                if (type == Button::WHITEIMAGE) fill ? fillColor = Color::WHITE : borderColor = Color::WHITE;
                else if (type == Button::BLACKIMAGE) fill ? fillColor = Color::BLACK : borderColor = Color::BLACK;
                else if (type == Button::REDIMAGE) fill ? fillColor = Color::RED : borderColor = Color::RED;
                else if (type == Button::GREENIMAGE) fill ? fillColor = Color::GREEN : borderColor = Color::GREEN;
                else if (type == Button::CYANIMAGE) fill ? fillColor = Color::CYAN : borderColor = Color::CYAN;
                else if (type == Button::PINKIMAGE) fill ? fillColor = Color::PURPLE : borderColor = Color::PURPLE;
                else if (type == Button::YELLOWIMAGE) fill ? fillColor = Color::YELLOW : borderColor = Color::YELLOW;
            
                else if (type == Button::CLEARIMAGE) {
                    canvas.Fill(Color::BLACK);
                }
                else if (type == Button::SAVEIMAGE) {
                    canvas.SaveTGA("images/saved_drawing.tga");
                }
                else if (type == Button::LOADIMAGE) {
                    canvas.LoadTGA("images/saved_drawing.tga", true);
                }
                
                else if (type <= Button::RECTANGLE) {
                    drawingTool = type;
                }
                break;
            }
        }

        if (!clicked_button) {
            is_drawing = true;
            draw_start_pos = mousePos;
            draw_prev_pos = mousePos;
            
            if (drawingTool == Button::PENCIL) {
                canvas.SetPixel((int)mousePos.x, (int)mousePos.y, borderColor);
            } else if (drawingTool == Button::ERASER) {
                canvas.SetPixel((int)mousePos.x, (int)mousePos.y, Color::BLACK);
            }
        }
    }
}

void Application::OnMouseButtonUp(SDL_MouseButtonEvent event)
{
    if (event.button == SDL_BUTTON_LEFT && is_drawing) {
        int correctedY = (window_height - 1) - event.y;
        Vector2 mousePos((float)event.x, (float)correctedY);
        
        if (drawingTool == Button::LINE) {
            canvas.DrawLineDDA((int)draw_start_pos.x, (int)draw_start_pos.y,
                               (int)mousePos.x, (int)mousePos.y, borderColor);
        } else if (drawingTool == Button::RECTANGLE) {
            int x = (int)std::min(draw_start_pos.x, mousePos.x);
            int y = (int)std::min(draw_start_pos.y, mousePos.y);
            int w = (int)std::abs(mousePos.x - draw_start_pos.x);
            int h = (int)std::abs(mousePos.y - draw_start_pos.y);
            canvas.DrawRect(x, y, w, h, borderColor, borderWidth, fill, fillColor);
        } else if (drawingTool == Button::TRIANGLE) {
            Vector2 p1 = { draw_start_pos.x, draw_start_pos.y };
            Vector2 p2 = { mousePos.x, draw_start_pos.y };
            float midX = (draw_start_pos.x + mousePos.x) / 2.0f;
            Vector2 p3 = { midX, mousePos.y };
            canvas.DrawTriangle(p1, p2, p3, borderColor, fill, fillColor);
        }
        is_drawing = false;
    }
}

void Application::OnMouseMove(SDL_MouseMotionEvent event)
{
    if (is_drawing) {
        int correctedY = (window_height - 1) - event.y;
        Vector2 mousePos((float)event.x, (float)correctedY);
        
        switch (drawingTool) {
            case Button::PENCIL:
                canvas.DrawLineDDA((int)draw_prev_pos.x, (int)draw_prev_pos.y,
                                   (int)mousePos.x, (int)mousePos.y, borderColor);
                draw_prev_pos = mousePos;
                break;

            case Button::ERASER:
                canvas.DrawLineDDA((int)draw_prev_pos.x, (int)draw_prev_pos.y,
                                   (int)mousePos.x, (int)mousePos.y, Color::BLACK);
                draw_prev_pos = mousePos;
                break;
            default: break;
        }
    }
}

void Application::OnKeyPressed(SDL_KeyboardEvent event)
{
    switch(event.keysym.sym) {
        case SDLK_ESCAPE: exit(0); break;
        case SDLK_f: fill = !fill; break;
        case SDLK_PLUS: borderWidth++; break;
        case SDLK_MINUS: if (borderWidth > 1) borderWidth--; break;
        case SDLK_1: drawingTool = Button::PENCIL; break;
        case SDLK_2:
            animation_mode = !animation_mode;
            if (animation_mode) {
                particles.Init(window_width, window_height);
            }
            break;
    }
}

void Application::OnWheel(SDL_MouseWheelEvent event) {}
void Application::OnFileChanged(const char* filename) { Shader::ReloadSingleShader(filename); }
