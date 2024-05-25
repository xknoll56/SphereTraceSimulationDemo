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
    void baseInit();
protected:
    void addColliderToOctTreeGrid(ST_Collider& collider, bool restructureTree);
    void addAABB(ST_AABB& aabb, ST_Vector4 color);
    void addSphereCollider(ST_SphereCollider& sphereCollider, ST_Vector4 color);
    void addPlaneCollider(ST_PlaneCollider& planeCollider, ST_Vector4 color);
    void drawSphereCubeCluster(ST_SphereCubeCluster& cluster, ST_Vector4 color, ST_Vector4 boundColor);
    void drawOctTreeRecursive(ST_OctTreeNode& node, ST_Vector4 color);
    ST_AABB worldAABB;
    ST_OctTreeGrid octTreeGrid;
    std::vector<Model> model;
private:

};

struct SceneRender : Scene
{

    void update(float dt) override;
    void draw() override;
    void init() override;

    std::vector<ST_Collider*> renderableColliders;
};

struct scenePhysicsTest : Scene
{
    ST_SimulationSpace simSpace;
    ST_SpherePair sp;
    ST_PlaneCollider pc;
    void update(float dt) override;
    void draw() override;
    void init() override;
};