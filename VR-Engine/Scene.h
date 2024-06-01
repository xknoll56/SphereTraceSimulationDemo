#pragma once

#include "Camera.h"

class Renderer;
struct VertexBuffer;
struct Material;
struct Model;
struct Time;

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
    Camera* pBoundLightCamera;
    Time* pTimer;
    void updateCamera(float dt);
    // Called once from camera perspective and once per main camera
    virtual void draw() = 0;
    // Called at startup
    virtual void init() = 0;
    // Called once per frame
    virtual void update(float dt) = 0;
    // Called once from light camera
    virtual void lightDraw();
    // Called once from main camera 
    virtual void mainDraw();
    // Called at the end of the frame
    virtual void lateUpdate();
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
    void lightDraw() override;
    void mainDraw() override;

    std::vector<ST_Collider*> renderableColliders;
    float period = 0.0f;
};

struct ColliderModel
{
    ST_Collider* pCollider;
    ST_Vector4 color;
};
struct scenePhysicsTest : Scene
{
    ST_SimulationSpace simSpace;
    ST_SphereCollider* psc;
    ST_Vector3 testPos;
    ST_IndexList viewColliders;
    std::vector<ST_Collider*> closestLights;
    bool started = false;
    std::vector<ColliderModel> models;
    void update(float dt) override;
    void draw() override;
    void init() override;
    void mainDraw() override;
    void lateUpdate() override;
};