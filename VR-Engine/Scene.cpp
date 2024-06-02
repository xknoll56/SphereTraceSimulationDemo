#include "stdafx.h"
#include "Scene.h"
#include "Input.h"
#include "SphereTracePhysicsEngine/src/SphereTraceMath.h"
#include "SphereTracePhysicsEngine/src/SphereTraceGlobals.h"
#include "Renderer.h"
#include <algorithm>


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

void Scene::lightDraw()
{

}

void Scene::mainDraw()
{

}

void Scene::lateUpdate()
{

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
	Renderer::instance.pixelShaderConstantBuffer.numSpotLights = 2;
	Renderer::instance.pixelShaderConstantBuffer.numShadowTextures = 2;
}

void SceneRender::update(float dt)
{
	updateCamera(dt);
	if (Input::keys[VK_SPACE])
	{
		period += dt;
	}

	ST_Vector3 position = ST_VECTOR3(2.0f * sinf(period), 8, 2.0f * cosf(period));
	Renderer::instance.setSpotLight(position, gVector3Down, gVector3One, 0);

	position = ST_VECTOR3(2.0f * sinf(period +M_PI)+15, 8, 2.0f * cosf(period + M_PI));
	Renderer::instance.setSpotLight(position, sphereTraceVector3Normalize(sphereTraceVector3Subtract(ST_VECTOR3(15, 3, 0), position)), gVector3One, 1);

	Renderer::instance.setSpotLight(ST_VECTOR3(0,8, 25), gVector3Down, gVector3One, 2);
	Renderer::instance.setSpotLight(ST_VECTOR3(15, 8, 25), gVector3Down, gVector3One, 3);
}

void SceneRender::draw()
{
	Renderer::instance.addPrimitiveInstance(ST_VECTOR3(0,3,0), gQuaternionIdentity, gVector3One, gVector4ColorGreen, PRIMITIVE_BOX);
	Renderer::instance.addPrimitiveInstance(ST_VECTOR3(15,3,0), gQuaternionIdentity, gVector3One, gVector4ColorBlue, PRIMITIVE_BOX);
	Renderer::instance.addPrimitiveInstance(gVector3Zero, gQuaternionIdentity, ST_VECTOR3(1000, 1, 1000), gVector4ColorWhite, PRIMITIVE_PLANE);
}

void SceneRender::lightDraw()
{

}
void SceneRender::mainDraw()
{
	Renderer::instance.drawPrimitive(Renderer::instance.pixelShaderConstantBuffer.spotLights[0].position, gQuaternionIdentity, gVector3One, sphereTraceVector4ColorSetAlpha(gVector4ColorRed, 0.7f), PRIMITIVE_SPHERE);
	Renderer::instance.drawPrimitive(Renderer::instance.pixelShaderConstantBuffer.spotLights[1].position, gQuaternionIdentity, gVector3One, sphereTraceVector4ColorSetAlpha(gVector4ColorBlue, 0.7f), PRIMITIVE_SPHERE);
	Renderer::instance.drawPrimitive(Renderer::instance.pixelShaderConstantBuffer.spotLights[2].position, gQuaternionIdentity, gVector3One, sphereTraceVector4ColorSetAlpha(gVector4ColorBlue, 0.7f), PRIMITIVE_SPHERE);
	Renderer::instance.drawPrimitive(Renderer::instance.pixelShaderConstantBuffer.spotLights[3].position, gQuaternionIdentity, gVector3One, sphereTraceVector4ColorSetAlpha(gVector4ColorBlue, 0.7f), PRIMITIVE_SPHERE);

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
	pBoundLightCamera = &Renderer::instance.pointLightCamera;
	Renderer::instance.pixelShaderConstantBuffer.numSpotLights = 10;
	Renderer::instance.pixelShaderConstantBuffer.numShadowTextures = 2;

	Renderer::instance.mainCamera.cameraMovementSpeed = 22.0f;
	Renderer::instance.mainCamera.cameraLerp = ST_VECTOR3(0, 400, 10);
	simSpace = sphereTraceSimulationConstruct();
	ST_SphereCollider* pSphere;
	ST_PlaneCollider* pPlane;
	ST_Collider* pLightCollider;
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
		sphereTraceColliderSphereSetPosition(pSphere, ST_VECTOR3(pSphere->rigidBody.position.x, 400.0f, pSphere->rigidBody.position.z));
	}

	for (int i = 0; i < 800; i++)
	{
		pPlane = (ST_PlaneCollider*)sphereTraceAllocatorAllocateCollider(COLLIDER_PLANE);
		*pPlane = sphereTraceColliderPlaneConstruct(sphereTraceVector3Subtract(getRandomPointInVector3(ST_VECTOR3(1.0f, 1.0f, 1.0f)), ST_VECTOR3(0.5, -0.5, 0.5)),
			timeGetRandomFloatBetween0And1() * M_PI, timeGetRandomFloatBetween0And1() * 50.0f+20.0f, timeGetRandomFloatBetween0And1() * 50.0f+20.0f, getRandomPointInAABB(spawnAABB));
		model.pCollider = (ST_Collider*)pPlane;
		model.color = ST_VECTOR4(timeGetRandomFloatBetween0And1(), timeGetRandomFloatBetween0And1(), timeGetRandomFloatBetween0And1(), 1.0f);
		models.push_back(model);
		pPlane->collider.pWhatever = (ST_Index)&models[models.size() - 1];
		sphereTraceSimulationInsertPlaneCollider(&simSpace, pPlane);
		if (i % 4 == 0)
		{
			pLightCollider = new ST_Collider();
			*pLightCollider = sphereTraceColliderAABBConstruct(sphereTraceAABBConstruct2(sphereTraceVector3Add(pPlane->position, ST_VECTOR3(0,25,0)), ST_VECTOR3(0.5, 0.5, 0.5f)));
			model.pCollider = pLightCollider;
			model.color = gVector4ColorWhite;
			models.push_back(model);
			pLightCollider->pWhatever = (ST_Index)&models[models.size() - 1];
			sphereTraceSimulationInsertAABBCollider(&simSpace, pLightCollider);
		}

	}
	sphereTraceSimulationOctTreeGridSolveDiscrete(&simSpace, 0.001f);
}

bool compareLightsByDistance(const ST_Collider* pa, const ST_Collider* pb)
{
	float da = sphereTraceVector3Distance(pa->aabb.center, Renderer::instance.mainCamera.cameraPos);
	float db = sphereTraceVector3Distance(pb->aabb.center, Renderer::instance.mainCamera.cameraPos);
	return da < db;
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

	if (Input::keysDown[KEY_L])
	{
		testPos = Renderer::instance.mainCamera.cameraPos;
	}

	if (Input::keys[VK_DOWN])
	{
		sphereTraceVector3AddAndScaleByRef(&testPos, gVector3Down, dt);
	}

	if (Input::keysDown[VK_SPACE])
	{
		started = true;
		ST_Vector3 spawnPos = sphereTraceVector3AddAndScale(Renderer::instance.mainCamera.cameraPos, Renderer::instance.mainCamera.cameraFwd, 2.0f);
		sphereTraceColliderSphereSetPosition(psc, spawnPos);
		sphereTraceRigidBodyResetMomentum(&psc->rigidBody);
	}

	//std::vector<ColliderModel> closestLights;
	{
		
		ColliderModel model;
		closestLights.clear();
		viewColliders = sphereTraceIndexListConstruct();
		ST_IndexList leafNodes = sphereTraceIndexListConstruct();
		sphereTraceOctTreeGridSampleIntersectionLeafsAndCollidersFromPerspective(&simSpace.octTreeGrid, Renderer::instance.mainCamera.cameraPos,
			sphereTraceDirectionConstruct(Renderer::instance.mainCamera.cameraFwd, 1), sphereTraceDegreesToRadians(140.0f), 200.0f, &leafNodes, &viewColliders);
		sphereTraceIndexListFree(&leafNodes);


		lightColliders = sphereTraceIndexListConstruct();
		ST_AABB lightsRenderSample = sphereTraceAABBConstruct2(sphereTraceVector3AddAndScale(Renderer::instance.mainCamera.cameraPos, Renderer::instance.mainCamera.cameraFwd, 10.0f), ST_VECTOR3(100.0f, 100.0f, 100.0f));
		sphereTraceOctTreeGridReSampleIntersectionLeafsAndColliders(&simSpace.octTreeGrid, &lightsRenderSample, &leafNodes, &lightColliders, true, true);
		sphereTraceIndexListFree(&leafNodes);

		pild = lightColliders.pFirst;
		for (int i = 0; i < lightColliders.count; i++)
		{
			pCollider = (ST_Collider*)pild->value;
			model = *(ColliderModel*)pCollider->pWhatever;
			switch (model.pCollider->colliderType)
			{
			case COLLIDER_AABB:;
				closestLights.push_back(model.pCollider);

			}
			pild = pild->pNext;
		}

		pild = viewColliders.pFirst;
		for (int i = 0; i < viewColliders.count; i++)
		{
			pCollider = (ST_Collider*)pild->value;
			model = *(ColliderModel*)pCollider->pWhatever;
			switch (model.pCollider->colliderType)
			{
			case COLLIDER_AABB:
				if (std::find(closestLights.begin(), closestLights.end(), model.pCollider) == closestLights.end())
				closestLights.push_back(model.pCollider);

			}
			pild = pild->pNext;
		}



		std::sort(closestLights.begin(), closestLights.end(), compareLightsByDistance);

		if (closestLights.size() > 10) 
		{
			closestLights.erase(closestLights.begin() + 10, closestLights.end());
		}

		for (int i = 0; i < closestLights.size(); i++)
		{
			Renderer::instance.setSpotLight(closestLights[i]->aabb.center, gVector3Down, gVector3One, i);
		}
	}
}

void scenePhysicsTest::lightDraw()
{
	ST_IndexListData* pild = lightColliders.pFirst;
	ColliderModel model;
	ST_Collider* pCollider;
	for (int i = 0; i < lightColliders.count; i++)
	{
		pCollider = (ST_Collider*)pild->value;
		model = *(ColliderModel*)pCollider->pWhatever;
		switch (model.pCollider->colliderType)
		{
		case COLLIDER_PLANE:
			addPlaneCollider(*(ST_PlaneCollider*)pCollider, model.color);
			break;
		case COLLIDER_SPHERE:
			addSphereCollider(*(ST_SphereCollider*)pCollider, model.color);
			break;
		}
		pild = pild->pNext;
	}
}

void scenePhysicsTest::mainDraw()
{


	for (int i = 0; i < closestLights.size(); i++)
	{
		if(i==0)
			Renderer::instance.addPrimitiveInstance(Renderer::instance.pixelShaderConstantBuffer.spotLights[i].position, gQuaternionIdentity, sphereTraceVector3UniformSize(2.0f), gVector4ColorGreen, PRIMITIVE_SPHERE);
		else
			Renderer::instance.addPrimitiveInstance(Renderer::instance.pixelShaderConstantBuffer.spotLights[i].position, gQuaternionIdentity, sphereTraceVector3UniformSize(2.0f) , gVector4ColorWhite, PRIMITIVE_SPHERE);
		//Renderer::instance.setSpotLight(closestLights[i], gVector3Down, gVector3One, i);
	}

	ColliderModel model;
	ST_Collider* pCollider;
	ST_IndexListData* pild = viewColliders.pFirst;
	for (int i = 0; i < viewColliders.count; i++)
	{
		pCollider = (ST_Collider*)pild->value;
		model = *(ColliderModel*)pCollider->pWhatever;
		switch (model.pCollider->colliderType)
		{
		case COLLIDER_PLANE:
			addPlaneCollider(*(ST_PlaneCollider*)pCollider, model.color);
			break;
		case COLLIDER_SPHERE:
			addSphereCollider(*(ST_SphereCollider*)pCollider, model.color);
			break;
		case COLLIDER_AABB:
			addAABB(pCollider->aabb, gVector4ColorWhite);
			break;
		}
		pild = pild->pNext;
	}

	ST_AABB lightsRenderSample = sphereTraceAABBConstruct2(sphereTraceVector3AddAndScale(Renderer::instance.mainCamera.cameraPos, Renderer::instance.mainCamera.cameraFwd, 10.0f), ST_VECTOR3(100.0f, 100.0f, 100.0f));
	Renderer::instance.addWireFrameInstance(lightsRenderSample.center, gQuaternionIdentity, lightsRenderSample.halfExtents, gVector4ColorGreen, PRIMITIVE_BOX);
	Renderer::instance.addWireFrameInstance(simSpace.octTreeGrid.worldaabb.center, gQuaternionIdentity, sphereTraceVector3Scale(simSpace.octTreeGrid.worldaabb.halfExtents, 2.0f), gVector4ColorCyan, PRIMITIVE_BOX);
}

void scenePhysicsTest::lateUpdate()
{
	sphereTraceIndexListFree(&viewColliders);
	sphereTraceIndexListFree(&lightColliders);
}


void scenePhysicsTest::draw()
{
	Renderer::instance.addPrimitiveInstance(testPos, gQuaternionIdentity, gVector3One, gVector4ColorWhite, PRIMITIVE_BOX);
}
