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

float timeGetRandomFloatBetween0And1()
{
	return (float)rand() / RAND_MAX;
}

void Scene::draw()
{
    Renderer::instance.pixelShaderConstantBuffer.cameraPos = sphereTraceVector4ConstructWithVector3(camera.cameraPos, 1.0f);
    ST_Matrix4 mvps[100];
    ST_Matrix4 models[100];
    ST_Vector4 colors[100];
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            int index = i * 10 + j;
            models[index] = sphereTraceMatrixMult(sphereTraceMatrixTranslation(ST_VECTOR3(i, 0, j)),
                sphereTraceMatrixMult(sphereTraceMatrixFromQuaternion(gQuaternionIdentity), sphereTraceMatrixScale(gVector3One)));
            mvps[index] = sphereTraceMatrixMult(Renderer::instance.projection, sphereTraceMatrixMult(camera.viewMatrix, models[index]));
            colors[index] = sphereTraceVector4Construct(timeGetRandomFloatBetween0And1(), timeGetRandomFloatBetween0And1(), timeGetRandomFloatBetween0And1(), 1.0f);
        }
    }
    Renderer::instance.drawPrimitiveInstanced(100, models, mvps, colors, Renderer::instance.texture, 0.5f, PRIMITIVE_SPHERE);
    Renderer::instance.drawLine(gVector3Zero, ST_VECTOR3(10, 10, 10), gVector4ColorBlue);
    Renderer::instance.drawWireFrame(gVector3Zero, gQuaternionIdentity, gVector3One, ST_VECTOR4(0,1,1,1), PRIMITIVE_PLANE);

}