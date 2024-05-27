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
	pBoundLightCamera = &Renderer::instance.pointLightCamera;
	pBoundLightCamera->cameraPos = ST_VECTOR3(0, 8, -2);
	Renderer::instance.overrideLightViewMatrix = true;
}

void SceneRender::update(float dt)
{
	updateCamera(dt);
	ST_Vector3 position = ST_VECTOR3(2.0f * sinf(pTimer->currentTimeInSeconds), 8, 2.0f * cosf(pTimer->currentTimeInSeconds));
	Renderer::instance.setSpotLight(position, sphereTraceVector3Normalize(sphereTraceVector3Subtract(ST_VECTOR3(0, 3, 0), position)), gVector3One);
}

void SceneRender::draw()
{
	Renderer::instance.drawPrimitive(ST_VECTOR3(0,3,0), gQuaternionIdentity, gVector3One, gVector4ColorGreen, PRIMITIVE_BOX);
	Renderer::instance.drawPrimitive(gVector3Zero, gQuaternionIdentity, ST_VECTOR3(1000, 1, 1000), gVector4ColorWhite, PRIMITIVE_PLANE);
}

void SceneRender::lightDraw()
{

}
void SceneRender::mainDraw()
{
	Renderer::instance.drawPrimitive(pBoundLightCamera->cameraPos, gQuaternionIdentity, gVector3One, sphereTraceVector4ColorSetAlpha(gVector4ColorRed, 0.7f), PRIMITIVE_SPHERE);
}

float timeGetRandomFloatBetween0And1()
{
	return (float)rand() / RAND_MAX;
}

ST_Vector3 getRandomPointInAABB(ST_AABB aabb)
{
	ST_Vector3 offset = ST_VECTOR3(2.0f * aabb.halfExtents.x * (timeGetRandomFloatBetween0And1() - 0.5f),
		2.0f * aabb.halfExtents.y * (timeGetRandomFloatBetween0And1() - 0.5f),
		2.0f * aabb.halfExtents.z * (timeGetRandomFloatBetween0And1() - 0.5f));
	return sphereTraceVector3Add(aabb.center, offset);
}

ST_Vector3 getRandomPointInVector3(ST_Vector3 vec)
{
	ST_Vector3 offset = ST_VECTOR3(timeGetRandomFloatBetween0And1() * vec.x,
		timeGetRandomFloatBetween0And1() * vec.y,
		timeGetRandomFloatBetween0And1() * vec.z);
	return offset;
}
void scenePhysicsTest::init()
{
	Renderer::instance.skipShadowPass = true;
	Renderer::instance.mainCamera.cameraMovementSpeed = 22.0f;
	simSpace = sphereTraceSimulationConstruct();
	ST_SphereCollider* pSphere;
	ST_PlaneCollider* pPlane;
	ColliderModel model;
	ST_AABB spawnAABB = sphereTraceAABBConstruct1(ST_VECTOR3(-400, -400, -400), ST_VECTOR3(400, 400, 400));
	models.reserve(3000);
	for (int i = 0; i < 1000; i++)
	{
		pSphere = (ST_SphereCollider*)sphereTraceAllocatorAllocateCollider(COLLIDER_SPHERE);
		*pSphere = sphereTraceColliderSphereConstruct(0.5f);
		sphereTraceColliderSphereSetPosition(pSphere, getRandomPointInAABB(simSpace.worldAABB));
		model.pCollider = (ST_Collider*)pSphere;
		if (i == 0)
		{
			psc = pSphere;
		}
		model.color = ST_VECTOR4(timeGetRandomFloatBetween0And1(), timeGetRandomFloatBetween0And1(), timeGetRandomFloatBetween0And1(), 1.0f);
		models.push_back(model);
		pSphere->collider.pWhatever = (ST_Index)&models[models.size() - 1];
		sphereTraceSimulationInsertSphereCollider(&simSpace, pSphere);
	}

	for (int i = 0; i < 1000; i++)
	{
		pPlane = (ST_PlaneCollider*)sphereTraceAllocatorAllocateCollider(COLLIDER_PLANE);
		*pPlane = sphereTraceColliderPlaneConstruct(sphereTraceVector3Subtract(getRandomPointInVector3(ST_VECTOR3(1.0f, 1.0f, 1.0f)), ST_VECTOR3(0.5, -0.5, 0.5)),
			timeGetRandomFloatBetween0And1() * M_PI, timeGetRandomFloatBetween0And1() * 100.0f, timeGetRandomFloatBetween0And1() * 100.0f, getRandomPointInAABB(spawnAABB));
		model.pCollider = (ST_Collider*)pPlane;
		model.color = ST_VECTOR4(timeGetRandomFloatBetween0And1(), timeGetRandomFloatBetween0And1(), timeGetRandomFloatBetween0And1(), 1.0f);
		models.push_back(model);
		pPlane->collider.pWhatever = (ST_Index)&models[models.size() - 1];
		sphereTraceSimulationInsertPlaneCollider(&simSpace, pPlane);
	}
}

void scenePhysicsTest::update(float dt)
{
	updateCamera(dt);
	if(started)
		sphereTraceSimulationOctTreeGridSolveDiscrete(&simSpace, dt);
	ST_IndexListData* pild = simSpace.octTreeGrid.outsideColliders.pFirst;
	ST_Collider* pCollider;
	ST_SphereCollider* pSphere;
	for (int i = 0; i < simSpace.octTreeGrid.outsideColliders.count; i++)
	{
		pCollider = (ST_Collider*)pild->value;
		if (pCollider->colliderType == COLLIDER_SPHERE)
		{
			pSphere = (ST_SphereCollider*)pCollider;
			sphereTraceRigidBodyResetMomentum(&pSphere->rigidBody);
			sphereTraceRigidBodyResetAngularMomentum(&pSphere->rigidBody);
			sphereTraceColliderSphereSetPosition(pSphere, sphereTraceVector3Construct(simSpace.worldAABB.halfExtents.x * 0.5f * (timeGetRandomFloatBetween0And1() - 0.5f),
				simSpace.worldAABB.halfExtents.y * 0.45f, simSpace.worldAABB.halfExtents.z * (timeGetRandomFloatBetween0And1() - 0.5f)));
		}
		pild = pild->pNext;
	}

	if (Input::keysDown[VK_SPACE])
	{
		started = true;
		ST_Vector3 spawnPos = sphereTraceVector3AddAndScale(Renderer::instance.mainCamera.cameraPos, Renderer::instance.mainCamera.cameraFwd, 2.0f);
		sphereTraceColliderSphereSetPosition(psc, spawnPos);
		sphereTraceRigidBodyResetMomentum(&psc->rigidBody);
	}
	if (Input::mouse[MOUSE_LEFT])
	{
		sphereTraceSimulationRayTrace(&simSpace, Renderer::instance.mainCamera.cameraPos, sphereTraceDirectionConstruct(Renderer::instance.mainCamera.cameraFwd, 1), FLT_MAX, &rtd);
	}
}


void scenePhysicsTest::draw()
{
	addAABB(simSpace.worldAABB, gVector4ColorGreen);

	ST_IndexList viewColliders = sphereTraceIndexListConstruct();
	ST_IndexList leafNodes = sphereTraceIndexListConstruct();
	sphereTraceOctTreeGridSampleIntersectionLeafsAndCollidersFromPerspective(&simSpace.octTreeGrid, Renderer::instance.mainCamera.cameraPos,
		sphereTraceDirectionConstruct(Renderer::instance.mainCamera.cameraFwd, 1), sphereTraceDegreesToRadians(140.0f), 200.0f, &leafNodes, &viewColliders);
	ColliderModel model;
	ST_Collider* pCollider;
	ST_IndexListData* pild = viewColliders.pFirst;
	for (int i = 0; i < viewColliders.count; i++)
	{
		pCollider = (ST_Collider*)pild->value;
		model = *(ColliderModel*)pCollider->pWhatever;
		switch (model.pCollider->colliderType)
		{
		case COLLIDER_SPHERE:
			addSphereCollider(*(ST_SphereCollider*)model.pCollider, model.color);
			break;
		case COLLIDER_PLANE:
			addPlaneCollider(*(ST_PlaneCollider*)model.pCollider, model.color);
			//Renderer::instance.addPrimitiveInstance(model.pCollider->aabb.center, gQuaternionIdentity, sphereTraceVector3UniformSize(2.0f*model.pCollider->boundingRadius),
			//	sphereTraceVector4ColorSetAlpha(model.color, 0.5f), PRIMITIVE_SPHERE);
			break;

		}
		pild = pild->pNext;
	}
	sphereTraceIndexListFree(&viewColliders);
	sphereTraceIndexListFree(&leafNodes);

	Renderer::instance.drawPrimitive(rtd.contact.point, gQuaternionIdentity, gVector3One, gVector4ColorWhite, PRIMITIVE_SPHERE);

	//for (int i = 0; i < models.size(); i++)
	//{
	//	switch (models[i].pCollider->colliderType)
	//	{
	//	case COLLIDER_SPHERE:
	//		addSphereCollider(*(ST_SphereCollider*)models[i].pCollider, models[i].color);
	//		break;
	//	case COLLIDER_PLANE:
	//		addPlaneCollider(*(ST_PlaneCollider*)models[i].pCollider, models[i].color);
	//		break;
	//	}
	//}
}