#pragma once

#include "Camera.h"

class Renderer;
struct VertexBuffer;
struct Material;
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
    virtual void draw() = 0;
    virtual void init() = 0;
protected:
    void addAABB(ST_AABB& aabb, ST_Vector4 color);
private:

};

struct SceneTest : Scene
{
    void draw() override;
    void init() override;
};