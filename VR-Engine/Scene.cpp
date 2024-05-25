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
	worldAABB = sphereTraceAABBConstruct1(sphereTraceVector3Construct(-500.0f, -500.0f, -500.0f),
		sphereTraceVector3Construct(500.0f, 500.0f, 500.0f));
     octTreeGrid = sphereTraceOctTreeGridConstruct(worldAABB, sphereTraceVector3Construct(100.0f, 100.0f, 100.0f));
	 for (ST_Index z = 0; z < octTreeGrid.zBuckets; z++)
	 {
		 for (ST_Index y = 0; y < octTreeGrid.yBuckets; y++)
		 {
			 for (ST_Index x = 0; x < octTreeGrid.xBuckets; x++)
			 {
				 ST_Index i = z * octTreeGrid.xBuckets * octTreeGrid.yBuckets + y * octTreeGrid.xBuckets + x;
				 octTreeGrid.treeBuckets[i].maxDepth = 5;
			 }
		 }
	 }
}

void Scene::addColliderToOctTreeGrid(ST_Collider& collider, bool restructureTree)
{
	sphereTraceOctTreeGridInsertCollider(&octTreeGrid, &collider, restructureTree);
}

float timeGetRandomFloatBetween0And1()
{
	return (float)rand() / RAND_MAX;
}

void Scene::addAABB(ST_AABB& aabb, ST_Vector4 color)
{
	Renderer::instance.addWireFrameInstance(aabb.center, gQuaternionIdentity, sphereTraceVector3Scale(aabb.halfExtents, 2.0f), color, PRIMITIVE_BOX);
}

void Scene::drawOctTreeRecursive(ST_OctTreeNode& node, ST_Vector4 color)
{
	addAABB(node.aabb, color);
	if (node.hasChildren)
	{
		for (int i = 0; i < 8; i++)
		{
			drawOctTreeRecursive(*node.children[i], color);
		}
	}
}

void Scene::addSphereCollider(ST_SphereCollider& sphereCollider, ST_Vector4 color)
{
	Renderer::instance.addPrimitiveInstance(sphereCollider.rigidBody.position, sphereCollider.rigidBody.rotation, sphereTraceVector3UniformSize(sphereCollider.radius * 2.0f), color, PRIMITIVE_SPHERE);
}

void Scene::addPlaneCollider(ST_PlaneCollider& planeCollider, ST_Vector4 color)
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


void SceneRender::init()
{
	pBoundCamera->cameraMovementSpeed = 20.0f;

	for (int i = 0; i < 500; i++)
	{
		ST_Collider* pCollider = new ST_Collider();
		*pCollider = sphereTraceColliderAABBConstruct(sphereTraceAABBConstruct2(ST_VECTOR3(3 * i, 5, 0), ST_VECTOR3(1, 1, 1)));
		addColliderToOctTreeGrid(*pCollider, true);
		renderableColliders.push_back(pCollider);
	}

	ST_AABB aabb;
	for (ST_Index z = 0; z < octTreeGrid.zBuckets; z++)
	{
		for (ST_Index y = 0; y < octTreeGrid.yBuckets; y++)
		{
			for (ST_Index x = 0; x < octTreeGrid.xBuckets; x++)
			{
				ST_Index i = z * octTreeGrid.xBuckets * octTreeGrid.yBuckets + y * octTreeGrid.xBuckets + x;
				aabb = octTreeGrid.treeBuckets[i].root->aabb;
				if (octTreeGrid.treeBuckets[i].root->hasChildren)
				{
					drawOctTreeRecursive(*octTreeGrid.treeBuckets[i].root, gVector4ColorRed);
				}
			}
		}
	}
}

void SceneRender::update(float dt)
{
	updateCamera(dt);

}

void SceneRender::draw()
{
	for (int i = 0; i < renderableColliders.size(); i++)
	{
		Renderer::instance.addPrimitiveInstance(renderableColliders[i]->aabb.center, gQuaternionIdentity, sphereTraceVector3UniformSize(2.0f), gVector4ColorBlue, PRIMITIVE_BOX);
	}
	addAABB(worldAABB, gVector4ColorGreen);

	ST_AABB aabb;
	for (ST_Index z = 0; z < octTreeGrid.zBuckets; z++)
	{
		for (ST_Index y = 0; y < octTreeGrid.yBuckets; y++)
		{
			for (ST_Index x = 0; x < octTreeGrid.xBuckets; x++)
			{
				ST_Index i = z * octTreeGrid.xBuckets * octTreeGrid.yBuckets + y * octTreeGrid.xBuckets + x;
				aabb = octTreeGrid.treeBuckets[i].root->aabb;
				if (octTreeGrid.treeBuckets[i].root->hasChildren)
				{
					drawOctTreeRecursive(*octTreeGrid.treeBuckets[i].root, gVector4ColorRed);
				}
			}
		}
	}

	Renderer::instance.drawPrimitive(gVector3Zero, gQuaternionIdentity, ST_VECTOR3(5000, 5000, 5000), gVector4ColorWhite, PRIMITIVE_PLANE);
}


void scenePhysicsTest::init()
{
	sp = sphereTraceColliderSpherePairConstruct(1.0f, 1.0f);

	sphereTraceRigidBodyRotate(&sp.rigidBody, sphereTraceQuaternionFromAngleAxis(gVector3Forward, 0.3f));
	sphereTraceFrameUpdateWithRotationMatrix(&sp.frame, sp.rigidBody.rotationMatrix);
	sp.rigidBody.position = ST_VECTOR3(0, 2, 0);
}

void scenePhysicsTest::update(float dt)
{
	updateCamera(dt);
}


void scenePhysicsTest::draw()
{
	ST_Vector3 leftSphere, rightSphere;
	sphereTraceColliderSpherePairGetSpherePositions(&sp, &leftSphere, &rightSphere);
	Renderer::instance.drawPrimitive(leftSphere, gQuaternionIdentity, sphereTraceVector3UniformSize(2.0f*sp.radii), gVector4ColorGreen, PRIMITIVE_SPHERE);
	Renderer::instance.drawPrimitive(rightSphere, gQuaternionIdentity, sphereTraceVector3UniformSize(2.0f*sp.radii), gVector4ColorGreen, PRIMITIVE_SPHERE);

	Renderer::instance.drawPrimitive(gVector3Zero, gQuaternionIdentity, ST_VECTOR3(500, 1, 500), gVector4ColorWhite, PRIMITIVE_PLANE);
}