#include "button.h"
#include <algorithm>

// Constructor 
Button::Button(Image& _image, const Vector2& _p1, const Vector2& _p2, const Vector2& _p3, const Vector2& _p4, ButtonType _type)
    : image(_image), p1(_p1), p2(_p2), p3(_p3), p4(_p4), type(_type) {}

bool Button::IsMouseInside(const Vector2& mousePosition) const {
    int minx = std::min(std::min(std::min(p1.x, p2.x), p3.x), p4.x);
    int maxx = std::max(std::max(std::max(p1.x, p2.x), p3.x), p4.x);
    int miny = std::min(std::min(std::min(p1.y, p2.y), p3.y), p4.y);
    int maxy = std::max(std::max(std::max(p1.y, p2.y), p3.y), p4.y);
    return (mousePosition.x >= minx && mousePosition.x <= maxx 
            && mousePosition.y >= miny && mousePosition.y <= maxy);
}

void Button::Render(Image& targetImage) const {
    targetImage.DrawImage(image, (int)p1.x, targetImage.height - p1.y - image.height);
}
