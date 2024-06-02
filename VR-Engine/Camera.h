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
	ST_Matrix4 projectionMatrix;
	float lerpSpeed;


	static Camera cameraConstructDefault()
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


	static Camera cameraConstruct(ST_Vector3 cameraPos, ST_Vector3 cameraFwd, ST_Vector3 cameraRight, float cameraPitch, float cameraYaw)
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

	static Camera cameraConstruct(ST_Vector3 cameraPos, float cameraPitch, float cameraYaw)
	{
		Camera camera;
		camera.cameraPos = cameraPos;
		camera.cameraPitch = cameraPitch;
		camera.cameraYaw = cameraYaw;
		camera.cameraLerp = cameraPos;
		camera.cameraLerpPitch = cameraPitch;
		camera.cameraLerpYaw = cameraYaw;
		camera.cameraMovementSpeed = 5.0f;
		camera.lerpSpeed = 3.0f;
		camera.cameraTurningSpeed = 1.0f;
		camera.cameraSetViewMatrix();
		camera.cameraSetRightAndFwdVectors();
		return camera;
	}

	void cameraSetViewMatrix()
	{
		viewMatrix = sphereTraceMatrixMult(sphereTraceMatrixMult(sphereTraceMatrixRotateX(cameraPitch), sphereTraceMatrixRotateY(cameraYaw)), sphereTraceMatrixTranslation(sphereTraceVector3Negative(cameraPos)));
	}

	void cameraSetRightAndFwdVectors()
	{
		cameraFwd = { -viewMatrix.m20, -viewMatrix.m21, -viewMatrix.m22 };
		cameraRight = { viewMatrix.m00, viewMatrix.m01, viewMatrix.m02 };
		cameraUp = { viewMatrix.m10, viewMatrix.m11, viewMatrix.m12 };
	}

	void cameraLookAt(ST_Vector3 pos, ST_Vector3 at)
	{
		ST_Vector3 dir = sphereTraceVector3Normalize(sphereTraceVector3Subtract(at, pos));
		ST_Vector3 xz = sphereTraceVector3Normalize(sphereTraceVector3Construct(dir.x, 0.0f, dir.z));
		cameraYaw = acosf(sphereTraceVector3Dot(xz, gVector3Back));
		cameraPitch = -asinf(dir.y);
		cameraPos = pos;
		cameraSetViewMatrix();
		cameraSetRightAndFwdVectors();
	}

	void cameraSetPosition(const ST_Vector3 pos)
	{
		cameraPos = pos;
		cameraLerp = pos;
	}
} Camera;

//Camera gMainCamera;
//Camera gSunCamera;
//Camera gReflectionCamera;

