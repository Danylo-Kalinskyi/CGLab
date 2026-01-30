#include "framework.h"
#include "image.h"

class Button {
public:
    enum ButtonType {
        PENCIL,
        COLORS, 
        ERASER,
        TRIANGLE,
        LINE,
        RECTANGLE,
        CLEARIMAGE,
        LOADIMAGE,
        SAVEIMAGE,
        BLACKIMAGE,
        CYANIMAGE,
        GREENIMAGE,
        PINKIMAGE,
        REDIMAGE,
        WHITEIMAGE,
        YELLOWIMAGE
    };

private:
    Image& image;
    Vector2 p1, p2, p3, p4;
    ButtonType type;

public:
    // Constructor
    Button(Image& _image, const Vector2& _p1, const Vector2& _p2, const Vector2& _p3, const Vector2& _p4, ButtonType _type);
    
    bool IsMouseInside(const Vector2& mousePosition) const;
    void Render(Image& targetImage) const;
    ButtonType GetType() const { return type; }
};
