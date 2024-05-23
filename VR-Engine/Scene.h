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
    virtual void update(float dt) = 0;
protected:
    void drawAABB(ST_AABB& aabb, ST_Vector4 color);
    void drawSphereCollider(ST_SphereCollider& sphereCollider, ST_Vector4 color);
    void drawPlaneCollider(ST_PlaneCollider& planeCollider, ST_Vector4 color);
    void drawSphereCubeCluster(ST_SphereCubeCluster& cluster, ST_Vector4 color, ST_Vector4 boundColor);
private:

};

struct SceneTest : Scene
{
    ST_SphereCollider sphereCollider;
    ST_PlaneCollider planeCollider;
    ST_SimulationSpace simSpace;
    ST_SphereCubeCluster cluster;

    void update(float dt) override;
    void draw() override;
    void init() override;
};