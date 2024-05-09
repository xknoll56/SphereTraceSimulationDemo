#pragma once
#include "SphereTracePhysicsEngine/src/SphereTraceMath.h"
#include "SphereTracePhysicsEngine/src/SphereTraceGlobals.h"

typedef struct Camera
{
	ST_Vector3 cameraPos;
	ST_Vector3 cameraLerp;
	ST_Vector3 cameraFwd;
	ST_Vector3 cameraRight;
	ST_Vector3 cameraUp;
	float cameraPitch;
	float cameraLerpPitch;
	float cameraYaw;
	float cameraLerpYaw;
	float cameraMovementSpeed;
	float cameraTurningSpeed;
	ST_Matrix4 viewMatrix;
	ST_Matrix4 perspectiveMatrix;
	float lerpSpeed;
} Camera;

Camera gMainCamera;
Camera gSunCamera;
Camera gReflectionCamera;

Camera cameraConstructDefault()
{
	Camera camera =
	{
		{0,2,2},
		{0,2,2},
		{0,0,-1},
		{1,0,0},
		{0,1,0},
		0,
		0,
		0,
		0,
		1,
		1 };
	camera.lerpSpeed = 3.0f;
	return camera;
};


Camera cameraConstruct(ST_Vector3 cameraPos, ST_Vector3 cameraFwd, ST_Vector3 cameraRight, float cameraPitch, float cameraYaw)
{
	Camera camera = {
		cameraPos,
		cameraPos,
		cameraFwd,
		cameraRight,
		cameraPitch,
		cameraPitch,
		cameraYaw,
		cameraYaw

	};
	camera.lerpSpeed = 3.0f;
	return camera;
}

void cameraSetViewMatrix(Camera* const pCamera)
{
	pCamera->viewMatrix = sphereTraceMatrixMult(sphereTraceMatrixMult(sphereTraceMatrixRotateX(pCamera->cameraPitch), sphereTraceMatrixRotateY(pCamera->cameraYaw)), sphereTraceMatrixTranslation(sphereTraceVector3Negative(pCamera->cameraPos)));
}

void cameraSetRightAndFwdVectors(Camera* const pCamera)
{
	pCamera->cameraFwd = { -pCamera->viewMatrix.m20, -pCamera->viewMatrix.m21, -pCamera->viewMatrix.m22 };
	pCamera->cameraRight = { pCamera->viewMatrix.m00, pCamera->viewMatrix.m01, pCamera->viewMatrix.m02 };
	pCamera->cameraUp = { pCamera->viewMatrix.m10, pCamera->viewMatrix.m11, pCamera->viewMatrix.m12 };
}

void cameraLookAt(Camera* const pCamera, ST_Vector3 pos, ST_Vector3 at)
{
	ST_Vector3 dir = sphereTraceVector3Normalize(sphereTraceVector3Subtract(at, pos));
	ST_Vector3 xz = sphereTraceVector3Normalize(sphereTraceVector3Construct(dir.x, 0.0f, dir.z));
	pCamera->cameraYaw = acosf(sphereTraceVector3Dot(xz, gVector3Back));
	pCamera->cameraPitch = -asinf(dir.y);
	pCamera->cameraPos = pos;
	cameraSetViewMatrix(pCamera);
	cameraSetRightAndFwdVectors(pCamera);
}