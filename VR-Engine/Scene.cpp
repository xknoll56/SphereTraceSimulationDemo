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

void Scene::baseInit()
{
	worldAABB = sphereTraceAABBConstruct1(sphereTraceVector3Construct(-500.0f, -100.0f, -500.0f),
		sphereTraceVector3Construct(500.0f, 100.0f, 500.0f));
     octTreeGrid = sphereTraceOctTreeGridConstruct(worldAABB, sphereTraceVector3Construct(100.0f, 100.0f, 100.0f));
}

void addColliderToRenderSpace(const ST_Collider& collider)
{
	
}

float timeGetRandomFloatBetween0And1()
{
	return (float)rand() / RAND_MAX;
}

void Scene::drawAABB(ST_AABB& aabb, ST_Vector4 color)
{
	Renderer::instance.addWireFrameInstance(aabb.center, gQuaternionIdentity, sphereTraceVector3Scale(aabb.halfExtents, 2.0f), color, PRIMITIVE_BOX);
}

void Scene::drawSphereCollider(ST_SphereCollider& sphereCollider, ST_Vector4 color)
{
	Renderer::instance.addPrimitiveInstance(sphereCollider.rigidBody.position, sphereCollider.rigidBody.rotation, sphereTraceVector3UniformSize(sphereCollider.radius * 2.0f), color, PRIMITIVE_SPHERE);
}

void Scene::drawPlaneCollider(ST_PlaneCollider& planeCollider, ST_Vector4 color)
{
	Renderer::instance.addPrimitiveInstance(planeCollider.position, planeCollider.rotation, ST_VECTOR3(planeCollider.xHalfExtent*2.0f, 1.0f, planeCollider.zHalfExtent*2.0f), color, PRIMITIVE_PLANE);
}

void Scene::drawSphereCubeCluster(ST_SphereCubeCluster& cluster, ST_Vector4 color, ST_Vector4 boundColor)
{
	for (int i = 0; i < 8; i++)
	{
		Renderer::instance.addPrimitiveInstance(sphereTraceVector3Add(cluster.rigidBody.position, cluster.elementPositions[i]), cluster.rigidBody.rotation, sphereTraceVector3UniformSize(cluster.halfWidth), color, PRIMITIVE_SPHERE);
	}
	Renderer::instance.addWireFrameInstance(cluster.rigidBody.position, cluster.rigidBody.rotation, sphereTraceVector3UniformSize(cluster.halfWidth * 2.0f), boundColor, PRIMITIVE_BOX);
}


void SceneTest::init()
{
	sphereCollider = sphereTraceColliderSphereConstruct(0.5f);
	sphereTraceColliderSphereSetPosition(&sphereCollider, ST_VECTOR3(0, 10, 0));
	planeCollider = sphereTraceColliderPlaneConstruct(gVector3Up, 0.0f, 50.0f, 50.0f, gVector3Zero);
	simSpace = sphereTraceSimulationConstruct();
	sphereTraceSimulationInsertSphereCollider(&simSpace, &sphereCollider);
	sphereTraceSimulationInsertPlaneCollider(&simSpace, &planeCollider);
	cluster = sphereTraceColliderSphereCubeClusterConstruct(3.0f);
	cluster.rigidBody.position = ST_VECTOR3(0, 10, 0);

	pBoundCamera->cameraMovementSpeed = 4.0f;


}

void SceneTest::update(float dt)
{
	sphereTraceSimulationOctTreeGridSolveDiscrete(&simSpace, dt);

}

void SceneTest::draw()
{

	drawSphereCollider(sphereCollider, gVector4ColorCyan);
	drawPlaneCollider(planeCollider, gVector4ColorWhite);
	drawSphereCubeCluster(cluster, gVector4ColorBlue, gVector4ColorGreen);


	ST_AABB aabb;
	for (ST_Index z = 0; z < octTreeGrid.zBuckets; z++)
	{
		for (ST_Index y = 0; y < octTreeGrid.yBuckets; y++)
		{
			for (ST_Index x = 0; x < octTreeGrid.xBuckets; x++)
			{
				ST_Index i = z * octTreeGrid.xBuckets * octTreeGrid.yBuckets + y * octTreeGrid.xBuckets + x;
				aabb = octTreeGrid.treeBuckets[i].root->aabb;
				drawAABB(aabb, gVector4ColorRed);
			}
		}
	}
}


