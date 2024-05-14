#include "stdafx.h"
#include "Scene.h"
#include "Input.h"
#include "SphereTracePhysicsEngine/src/SphereTraceMath.h"
#include "SphereTracePhysicsEngine/src/SphereTraceGlobals.h"
#include "Renderer.h"


void Scene::updateCamera(float dt)
{
    float boost = 1.0f;
    if (Input::keys[VK_SHIFT])
        boost = 3.0f;
    if (Input::keys[KEY_W])
    {
        camera.cameraLerp = sphereTraceVector3Add(sphereTraceVector3Scale(camera.cameraFwd, camera.cameraMovementSpeed * dt * boost), camera.cameraLerp);
    }
    if (Input::keys[KEY_A])
    {
        camera.cameraLerp = sphereTraceVector3Add(sphereTraceVector3Scale(camera.cameraRight, -camera.cameraMovementSpeed * dt * boost), camera.cameraLerp);
    }
    if (Input::keys[KEY_S])
    {
        camera.cameraLerp = sphereTraceVector3Add(sphereTraceVector3Scale(camera.cameraFwd, -camera.cameraMovementSpeed * dt * boost), camera.cameraLerp);
    }
    if (Input::keys[KEY_D])
    {
        camera.cameraLerp = sphereTraceVector3Add(sphereTraceVector3Scale(camera.cameraRight, camera.cameraMovementSpeed * dt * boost), camera.cameraLerp);
    }

    if (Input::mouse[MOUSE_RIGHT])
    {
        camera.cameraLerpYaw += Input::gDeltaMousePosition.x * camera.cameraTurningSpeed * dt;
        camera.cameraLerpPitch += Input::gDeltaMousePosition.y * camera.cameraTurningSpeed * dt;
    }
    camera.cameraPos = sphereTraceVector3Lerp(camera.cameraPos, camera.cameraLerp, dt * camera.lerpSpeed);
    camera.cameraYaw = sphereTraceLerp(camera.cameraYaw, camera.cameraLerpYaw, dt * camera.lerpSpeed);
    camera.cameraPitch = sphereTraceLerp(camera.cameraPitch, camera.cameraLerpPitch, dt * camera.lerpSpeed);
    camera.cameraSetViewMatrix();
    camera.cameraSetRightAndFwdVectors();
}

void Scene::draw()
{
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            //Renderer::instance.drawPrimitive(ST_VECTOR3(i, 0, j), gQuaternionIdentity, gVector3One, gVector4ColorBlue, PRIMITIVE_CYLINDER);
           // Renderer::instance.drawPrimitive(ST_VECTOR3(i, 0, j), gQuaternionIdentity, gVector3One, Renderer::instance.texture, PRIMITIVE_CYLINDER);
            Renderer::instance.drawPrimitive(ST_VECTOR3(i, 0, j), gQuaternionIdentity, gVector3One, gVector4ColorMagenta, Renderer::instance.texture, 0.5f, PRIMITIVE_BOX);
            //Renderer::instance.drawWireFrame(ST_VECTOR3(i, 0, j), gQuaternionIdentity, gVector3One, gVector4ColorGreen, PRIMITIVE_SPHERE);
        }
    }
    Renderer::instance.drawLine(gVector3Zero, ST_VECTOR3(10, 10, 10), gVector4ColorBlue);
    Renderer::instance.drawWireFrame(gVector3Zero, gQuaternionIdentity, gVector3One, ST_VECTOR4(0,1,1,1), PRIMITIVE_PLANE);

}