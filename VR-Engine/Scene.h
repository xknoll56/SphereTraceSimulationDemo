#pragma once

#include "Camera.h"

class Renderer;

enum PrimitiveType
{
    PRIMITIVE_PLANE = 0,
    PRIMITIVE_BOX,
    PRIMITIVE_SPHERE,
    PRIMITIVE_CYLINDER
};

struct Scene
{
    Camera camera;
    void updateCamera(float dt);
    void draw();
};
