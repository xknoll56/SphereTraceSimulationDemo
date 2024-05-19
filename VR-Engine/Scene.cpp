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
        pBoundCamera->cameraLerp = sphereTraceVector3Add(sphereTraceVector3Scale(pBoundCamera->cameraFwd, pBoundCamera->cameraMovementSpeed * dt * boost), pBoundCamera->cameraLerp);
    }
    if (Input::keys[KEY_A])
    {
        pBoundCamera->cameraLerp = sphereTraceVector3Add(sphereTraceVector3Scale(pBoundCamera->cameraRight, -pBoundCamera->cameraMovementSpeed * dt * boost), pBoundCamera->cameraLerp);
    }
    if (Input::keys[KEY_S])
    {
        pBoundCamera->cameraLerp = sphereTraceVector3Add(sphereTraceVector3Scale(pBoundCamera->cameraFwd, -pBoundCamera->cameraMovementSpeed * dt * boost), pBoundCamera->cameraLerp);
    }
    if (Input::keys[KEY_D])
    {
        pBoundCamera->cameraLerp = sphereTraceVector3Add(sphereTraceVector3Scale(pBoundCamera->cameraRight, pBoundCamera->cameraMovementSpeed * dt * boost), pBoundCamera->cameraLerp);
    }

    if (Input::mouse[MOUSE_RIGHT])
    {
        pBoundCamera->cameraLerpYaw += Input::gDeltaMousePosition.x * pBoundCamera->cameraTurningSpeed * dt;
        pBoundCamera->cameraLerpPitch += Input::gDeltaMousePosition.y * pBoundCamera->cameraTurningSpeed * dt;
    }
    pBoundCamera->cameraPos = sphereTraceVector3Lerp(pBoundCamera->cameraPos, pBoundCamera->cameraLerp, dt * pBoundCamera->lerpSpeed);
    pBoundCamera->cameraYaw = sphereTraceLerp(pBoundCamera->cameraYaw, pBoundCamera->cameraLerpYaw, dt * pBoundCamera->lerpSpeed);
    pBoundCamera->cameraPitch = sphereTraceLerp(pBoundCamera->cameraPitch, pBoundCamera->cameraLerpPitch, dt * pBoundCamera->lerpSpeed);
    pBoundCamera->cameraSetViewMatrix();
    pBoundCamera->cameraSetRightAndFwdVectors();
}

float timeGetRandomFloatBetween0And1()
{
	return (float)rand() / RAND_MAX;
}

void Scene::init()
{
    for (int i = 0; i < 20; i++)
    {
        for (int j = 0; j < 20; j++)
        {
            int index = i * 20 + j;
            Renderer::instance.addPrimitiveInstance(ST_VECTOR3(i, 2, j), gQuaternionIdentity, gVector3One,
                sphereTraceVector4Construct(timeGetRandomFloatBetween0And1(), timeGetRandomFloatBetween0And1(), timeGetRandomFloatBetween0And1(), 1.0f), PRIMITIVE_SPHERE);
        }
    }

    Renderer::instance.perPrimitiveInstanceBufferCounts[PRIMITIVE_SPHERE] = 0;
    pBoundCamera->cameraMovementSpeed = 4.0f;
    //pBoundCamera = &Renderer::instance.directionalLightCamera;
}

void Scene::draw()
{
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            int index = i * 20 + j;
            //Renderer::instance.drawPrimitive(ST_VECTOR3(i, 2, j), gQuaternionIdentity, gVector3One, gVector4ColorCyan, Renderer::instance.texture, 0.5f, PRIMITIVE_SPHERE);
            Renderer::instance.addPrimitiveInstance(ST_VECTOR3(i*1.05, 2, j * 1.05), gQuaternionIdentity, gVector3One, PRIMITIVE_SPHERE);
            Renderer::instance.addWireFrameInstance(ST_VECTOR3(i * 1.05, 2, j * 1.05), gQuaternionIdentity, gVector3One, gVector4ColorGreen, PRIMITIVE_BOX);
        }
    }

    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            int index = i * 20 + j;
            //Renderer::instance.drawPrimitive(ST_VECTOR3(i, 4, j), gQuaternionIdentity, gVector3One, gVector4ColorCyan, Renderer::instance.texture, 0.5f, PRIMITIVE_SPHERE);
            Renderer::instance.addPrimitiveInstance(ST_VECTOR3(i*1.1, 4, j*1.1), gQuaternionIdentity, gVector3One, PRIMITIVE_SPHERE);
            Renderer::instance.addWireFrameInstance(ST_VECTOR3(i * 1.1, 4, j * 1.1), gQuaternionIdentity, gVector3One, gVector4ColorGreen, PRIMITIVE_BOX);
        }
    }
    //Renderer::instance.drawPrimitive(Renderer::instance.directionalLightCamera.cameraPos, gQusaternionIdentity, gVector3One, gVector4ColorWhite, PRIMITIVE_BOX);
    //Renderer::instance.drawPrimitive(gVector3Zero, gQuaternionIdentity, ST_VECTOR3(100, 1, 100), gVector4ColorWhite, PRIMITIVE_PLANE);
    Renderer::instance.addPrimitiveInstance(gVector3Zero, gQuaternionIdentity, ST_VECTOR3(100, 1, 100), gVector4ColorWhite, PRIMITIVE_PLANE);
    //Renderer::instance.drawWireFrame(gVector3Zero, gQuaternionIdentity, gVector3One, gVector4ColorRed, PRIMITIVE_PLANE);

}