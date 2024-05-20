#pragma once

#include "Camera.h"

class Renderer;
struct VertexBuffer;
struct Model;

enum PrimitiveType
{
    PRIMITIVE_PLANE = 0,
    PRIMITIVE_BOX,
    PRIMITIVE_SPHERE,
    PRIMITIVE_CYLINDER
};

struct Scene
{
public:
    Camera* pBoundCamera;
    void updateCamera(float dt);
    void draw();
    void init();
protected:
    void addAABB(ST_AABB& aabb, ST_Vector4 color);
private:
    
    std::vector<VertexBuffer> sponza;
};
