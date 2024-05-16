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
    Camera* pBoundCamera;
    void updateCamera(float dt);
    void draw();
    void init();
};
