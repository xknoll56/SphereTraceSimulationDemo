
#include "SphereTrace.h"
#include "SphereTraceGlobals.h"

PenetrationRestriction sphereTracePenetrationRestrictionConstruct()
{
	PenetrationRestriction penetrationRestriction;
	penetrationRestriction.penetrationRestrictionType = ST_PENETRATION_RESTRICTION_NONE;
	penetrationRestriction.restrictionDirectionSet = 0;
	return penetrationRestriction;
}

void sphereTraceSubscriberListAddOnCollisionEnterCallback(ST_SimulationSpace* pSimSpace, ST_Collider* pCollider, 
	void (*callback)(const ST_SphereContact* const contact, ST_Collider* pOtherCollider, void* pContext), void* pContext)
{
	ST_CallbackFunction* pcf = sphereTraceAllocatorAllocateCallbackFunction();
	pcf->callback = callback;
	sphereTraceIndexListAddFirst(&pCollider->subscriberList.onCollisionEnterCallbacks, pcf);
	pCollider->subscriberList.hasSubscriber = 1;
	sphereTraceIndexListAddUnique(&pSimSpace->callbackColliders, pCollider);
	pCollider->subscriberList.pSubscriberContext = pContext;
}
void sphereTraceSubscriberListAddOnCollisionStayCallback(ST_SimulationSpace* pSimSpace, ST_Collider* pCollider, 
	void (*callback)(const ST_SphereContact* const contact, ST_Collider* pOtherCollider, void* pContext), void* pContext)
{
	ST_CallbackFunction* pcf = sphereTraceAllocatorAllocateCallbackFunction();
	pcf->callback = callback;
	sphereTraceIndexListAddFirst(&pCollider->subscriberList.onCollisionStayCallbacks, pcf);
	pCollider->subscriberList.hasSubscriber = 1;
	sphereTraceIndexListAddUnique(&pSimSpace->callbackColliders, pCollider);
	pCollider->subscriberList.pSubscriberContext = pContext;
}
void sphereTraceSubscriberListAddOnCollisionExitCallback(ST_SimulationSpace* pSimSpace, ST_Collider* pCollider, 
	void (*callback)(const ST_SphereContact* const contact, ST_Collider* pOtherCollider, void* pContext), void* pContext)
{
	ST_CallbackFunction* pcf = sphereTraceAllocatorAllocateCallbackFunction();
	pcf->callback = callback;
	sphereTraceIndexListAddFirst(&pCollider->subscriberList.onCollisionExitCallbacks, pcf);
	pCollider->subscriberList.hasSubscriber = 1;
	sphereTraceIndexListAddUnique(&pSimSpace->callbackColliders, pCollider);
	pCollider->subscriberList.pSubscriberContext = pContext;
}

ST_SimulationSpace sphereTraceSimulationConstruct()
{
	ST_SimulationSpace simulationSpace;
	simulationSpace.sphereColliders = sphereTraceIndexListConstruct();
	simulationSpace.boxColliders = sphereTraceIndexListConstruct();
	simulationSpace.planeColliders = sphereTraceIndexListConstruct();
	simulationSpace.triangleColliders = sphereTraceIndexListConstruct();
	simulationSpace.uniformTerrainColliders = sphereTraceIndexListConstruct();
	simulationSpace.spherePairColliders = sphereTraceIndexListConstruct();
	simulationSpace.aabbColliders = sphereTraceIndexListConstruct();
	simulationSpace.callbackColliders = sphereTraceIndexListConstruct();
	//simulationSpace.spacialPartitionContainer = sphereTraceSpacialPartitionStaticConstruct(20.0f);
	simulationSpace.minDeltaTime = 1.0f / 60.0f;
	simulationSpace.gravitationalAcceleration = sphereTraceVector3Construct(0.0f, -9.81f, 0.0f);
	simulationSpace.defaultMaterial = sphereTraceMaterialConstruct(0.2f, 0.8f, 0.2f);
	simulationSpace.worldAABB = sphereTraceAABBConstruct1(sphereTraceVector3Construct(-500.0f, -500.0f, -500.0f),
		sphereTraceVector3Construct(500.0f, 500.0f, 500.0f));
	simulationSpace.octTreeGrid = sphereTraceOctTreeGridConstruct(simulationSpace.worldAABB, sphereTraceVector3Construct(100.0f, 100.0f, 100.0f));
	return simulationSpace;
}

ST_SimulationSpace sphereTraceSimulationConstruct1(ST_AABB worldaabb, ST_Vector3 gridHalfExtents)
{
	ST_SimulationSpace simulationSpace;
	simulationSpace.sphereColliders = sphereTraceIndexListConstruct();
	simulationSpace.boxColliders = sphereTraceIndexListConstruct();
	simulationSpace.planeColliders = sphereTraceIndexListConstruct();
	simulationSpace.triangleColliders = sphereTraceIndexListConstruct();
	simulationSpace.uniformTerrainColliders = sphereTraceIndexListConstruct();
	simulationSpace.spherePairColliders = sphereTraceIndexListConstruct();
	simulationSpace.aabbColliders = sphereTraceIndexListConstruct();
	simulationSpace.callbackColliders = sphereTraceIndexListConstruct();
	//simulationSpace.spacialPartitionContainer = sphereTraceSpacialPartitionStaticConstruct(20.0f);
	simulationSpace.minDeltaTime = 1.0f / 60.0f;
	simulationSpace.gravitationalAcceleration = sphereTraceVector3Construct(0.0f, -9.81f, 0.0f);
	simulationSpace.defaultMaterial = sphereTraceMaterialConstruct(0.2f, 0.8f, 0.2f);
	simulationSpace.octTreeGrid = sphereTraceOctTreeGridConstruct(worldaabb, gridHalfExtents);
	simulationSpace.worldAABB = worldaabb;
	return simulationSpace;
}

void sphereTraceSimulationFree(ST_SimulationSpace* const pSimulationSpace)
{
	sphereTraceIndexListFree(&pSimulationSpace->sphereColliders);
	sphereTraceIndexListFree(&pSimulationSpace->planeColliders);
	sphereTraceIndexListFree(&pSimulationSpace->triangleColliders);
	sphereTraceIndexListFree(&pSimulationSpace->aabbColliders);
	sphereTraceIndexListFree(&pSimulationSpace->callbackColliders);
	sphereTraceIndexListFree(&pSimulationSpace->spherePairColliders);
	ST_IndexListData* pild = pSimulationSpace->uniformTerrainColliders.pFirst;
	ST_UniformTerrainCollider* pTerrainCollider = NULL;
	for (int i = 0; i < pSimulationSpace->uniformTerrainColliders.count; i++)
	{
		pTerrainCollider = pild->value;
		sphereTraceColliderUniformTerrainFree(pTerrainCollider);
		pild = pild->pNext;
	}
}

void sphereTraceSimulationInsertCollider(ST_SimulationSpace* const pSimulationSpace, ST_Collider* const pCollider, b32 restructureTree)
{

}

void sphereTraceSimulationInsertPlaneCollider(ST_SimulationSpace* const pSimulationSpace, ST_PlaneCollider* const pPlaneCollider)
{
	//ST_ColliderIndex index = pSimulationSpace->planeColliders.count;
	pPlaneCollider->collider.colliderIndex = pPlaneCollider;
	if(sphereTraceSortedIndexListAddUnique(&pSimulationSpace->planeColliders, pPlaneCollider))
		sphereTraceOctTreeGridInsertCollider(&pSimulationSpace->octTreeGrid, pPlaneCollider, ST_TRUE);
	else
		sphereTraceOctTreeGridReInsertCollider(&pSimulationSpace->octTreeGrid, pPlaneCollider, ST_TRUE);
}

void sphereTraceSimulationInsertTriangleCollider(ST_SimulationSpace* const pSimulationSpace, ST_TriangleCollider* const pTriangleCollider)
{
	//ST_ColliderIndex index = pSimulationSpace->planeColliders.count;
	pTriangleCollider->collider.colliderIndex = pTriangleCollider;
	if(sphereTraceSortedIndexListAddUnique(&pSimulationSpace->triangleColliders, pTriangleCollider))
		sphereTraceOctTreeGridInsertCollider(&pSimulationSpace->octTreeGrid, pTriangleCollider, ST_TRUE);
	else
		sphereTraceOctTreeGridReInsertCollider(&pSimulationSpace->octTreeGrid, pTriangleCollider, ST_TRUE);

}

void sphereTraceSimulationInsertSphereCollider(ST_SimulationSpace* const pSimulationSpace, ST_SphereCollider* const pSphereCollider)
{
	//ST_ColliderIndex index = pSimulationSpace->sphereColliders.count;
	pSphereCollider->collider.colliderIndex = pSphereCollider;
	if(sphereTraceSortedIndexListAddUnique(&pSimulationSpace->sphereColliders, pSphereCollider))
		sphereTraceOctTreeGridInsertCollider(&pSimulationSpace->octTreeGrid, pSphereCollider, ST_TRUE);
	else
		sphereTraceOctTreeGridReInsertCollider(&pSimulationSpace->octTreeGrid, pSphereCollider, ST_TRUE);

}

void sphereTraceSimulationInsertBoxCollider(ST_SimulationSpace* const pSimulationSpace, ST_BoxCollider* const pBoxCollider)
{
	pBoxCollider->collider.colliderIndex = pBoxCollider;
	if (sphereTraceSortedIndexListAddUnique(&pSimulationSpace->boxColliders, pBoxCollider))
		sphereTraceOctTreeGridInsertCollider(&pSimulationSpace->octTreeGrid, pBoxCollider, ST_TRUE);
	else
		sphereTraceOctTreeGridReInsertCollider(&pSimulationSpace->octTreeGrid, pBoxCollider, ST_TRUE);
}

void sphereTraceSimulationInsertUniformTerrainCollider(ST_SimulationSpace* const pSimulationSpace, ST_UniformTerrainCollider* const pTerrainCollider)
{
	//ST_ColliderIndex index = pSimulationSpace->uniformTerrainColliders.count;
	pTerrainCollider->collider.colliderIndex = pTerrainCollider;
	if(sphereTraceSortedIndexListAddUnique(&pSimulationSpace->uniformTerrainColliders, pTerrainCollider))
		sphereTraceOctTreeGridInsertCollider(&pSimulationSpace->octTreeGrid, pTerrainCollider, ST_TRUE);
	else
		sphereTraceOctTreeGridReInsertCollider(&pSimulationSpace->octTreeGrid, pTerrainCollider, ST_TRUE);

}

void sphereTraceSimulationInsertSpherePairCollider(ST_SimulationSpace* const pSimulationSpace, ST_SpherePair* const pSpherePair)
{
	pSpherePair->collider.colliderIndex = pSpherePair;
	if (sphereTraceSortedIndexListAddUnique(&pSimulationSpace->spherePairColliders, pSpherePair))
		sphereTraceOctTreeGridInsertCollider(&pSimulationSpace->octTreeGrid, pSpherePair, ST_TRUE);
	else
		sphereTraceOctTreeGridReInsertCollider(&pSimulationSpace->octTreeGrid, pSpherePair, ST_TRUE);
}

void sphereTraceSimulationInsertAABBCollider(ST_SimulationSpace* const pSimulationSpace, ST_Collider* const pCollider)
{
	//ST_ColliderIndex index = pSimulationSpace->uniformTerrainColliders.count;
	pCollider->colliderIndex = pCollider;
	if (sphereTraceSortedIndexListAddUnique(&pSimulationSpace->aabbColliders, pCollider))
		sphereTraceOctTreeGridInsertCollider(&pSimulationSpace->octTreeGrid, pCollider, ST_TRUE);
	else
		sphereTraceOctTreeGridReInsertCollider(&pSimulationSpace->octTreeGrid, pCollider, ST_TRUE);

}

void sphereTraceSimulationRemoveCollider(ST_SimulationSpace* const pSimulationSpace, ST_Collider* const pCollider, b32 restructureTree)
{
	//ST_IndexListData* pild = pCollider->bucketIndices.pFirst;
	//for (int i = 0; i < pCollider->bucketIndices.count; i++)
	//{
	//	sphereTraceIndexListFree(&pCollider->pLeafBucketLists[pild->value]);
	//	pild = pild->pNext;
	//}
	sphereTraceOctTreeGridRemoveCollider(&pSimulationSpace->octTreeGrid, pCollider, restructureTree);
	switch (pCollider->colliderType)
	{
	case COLLIDER_SPHERE:
	{
		sphereTraceSortedIndexListRemove(&pSimulationSpace->sphereColliders, pCollider);
	}
	break;
	case COLLIDER_PLANE:
	{
		sphereTraceSortedIndexListRemove(&pSimulationSpace->planeColliders, pCollider);
	}
	break;
	case COLLIDER_TRIANGLE:
	{
		sphereTraceSortedIndexListRemove(&pSimulationSpace->triangleColliders, pCollider);
	}
	break;
	case COLLIDER_TERRAIN:
	{
		sphereTraceSortedIndexListRemove(&pSimulationSpace->uniformTerrainColliders, pCollider);
	}
	break;
	case COLLIDER_AABB:
	{
		sphereTraceSortedIndexListRemove(&pSimulationSpace->aabbColliders, pCollider);
	}
	break;
	}
}

void sphereTraceSimulationSpaceTranslateStaticCollider(ST_SimulationSpace* const pSimulationSpace, ST_Collider* const pStaticCollider, ST_Vector3 translation, b32 restructureTree)
{
	switch (pStaticCollider->colliderType)
	{
	case COLLIDER_PLANE:
		sphereTraceColliderPlaneTranslate(pStaticCollider, translation);
		sphereTraceOctTreeGridReInsertCollider(&pSimulationSpace->octTreeGrid, pStaticCollider, restructureTree);
		break;
	}
}

void sphereTraceSimulationSpaceMoveStaticCollider(ST_SimulationSpace* const pSimulationSpace, ST_Collider* const pStaticCollider, ST_Vector3 newPosition, b32 restructureTree)
{
	switch (pStaticCollider->colliderType)
	{
	case COLLIDER_PLANE:;
		ST_PlaneCollider* pPlaneCollider = pStaticCollider;
		ST_Vector3 translation = sphereTraceVector3Subtract(newPosition, pPlaneCollider->position);
		sphereTraceColliderPlaneTranslate(pStaticCollider, translation);
		sphereTraceOctTreeGridReInsertCollider(&pSimulationSpace->octTreeGrid, pStaticCollider, restructureTree);
		break;
	}
}



void sphereTraceSimulationApplyForcesAndTorques(const ST_SimulationSpace* const pSimulationSpace, ST_RigidBody* const pRigidBody, float dt, b32 incrementGravity)
{
	//apply gravity
	if (incrementGravity)
		pRigidBody->linearMomentum = sphereTraceVector3Add(pRigidBody->linearMomentum, sphereTraceVector3Scale(pSimulationSpace->gravitationalAcceleration, pRigidBody->mass * dt));

	sphereTraceRigidBodyApplyForces(pRigidBody, dt);
	sphereTraceRigidBodyApplyDeltaMomentums(pRigidBody);
	//set velocity
	pRigidBody->velocity = sphereTraceVector3Scale(pRigidBody->linearMomentum, 1.0f/pRigidBody->mass);


	sphereTraceRigidBodyApplyTorques(pRigidBody, dt);
	sphereTraceRigidBodyApplyDeltaAngularMomentums(pRigidBody);
	//set angular velocity
	pRigidBody->angularVelocity = sphereTraceVector3Scale(pRigidBody->angularMomentum, 1.0f/pRigidBody->inertia);
}

void sphereTraceSimulationStepQuantity(const ST_SimulationSpace* const pSimulationSpace, ST_RigidBody* const pRigidBody, float dt)
{
	sphereTraceSimulationApplyForcesAndTorques(pSimulationSpace, pRigidBody, dt, 1);

	pRigidBody->rotation = sphereTraceQuaternionAdd(pRigidBody->rotation, sphereTraceQuaternionMultiply(sphereTraceQuaternionConstruct(0.0f, 0.5f * dt * pRigidBody->angularVelocity.x, 0.5f * dt * pRigidBody->angularVelocity.y, 0.5f * dt * pRigidBody->angularVelocity.z), pRigidBody->rotation));
	pRigidBody->rotation = sphereTraceQuaternionNormalize(pRigidBody->rotation);
	pRigidBody->rotationMatrix = sphereTraceMatrixFromQuaternion(pRigidBody->rotation);

	pRigidBody->prevPosition = pRigidBody->position;
	pRigidBody->position = sphereTraceVector3Add(pRigidBody->position, sphereTraceVector3Scale(pRigidBody->velocity, dt));
}

ST_Vector3 sphereTraceSimulationImposedStepPosition(const ST_SimulationSpace* const pSimulationSpace, ST_RigidBody* const pRigidBody, float dt)
{
	//apply gravity
	ST_Vector3 imposedLinearMomentum = sphereTraceVector3Add(pRigidBody->linearMomentum, sphereTraceVector3Scale(pSimulationSpace->gravitationalAcceleration, pRigidBody->mass * dt));

	//apply forces
	ST_Vector3ListData* pData = pRigidBody->appliedForces.pFirst;
	for (int i = 0; i < pRigidBody->appliedForces.count; i++)
	{
		imposedLinearMomentum = sphereTraceVector3Add(imposedLinearMomentum, sphereTraceVector3Scale(pData->value, dt));
		pData = pData->pNext;
	}

	//apply dps
	pData = pRigidBody->appliedDeltaMomentums.pFirst;
	for (int i = 0; i < pRigidBody->appliedDeltaMomentums.count; i++)
	{
		imposedLinearMomentum = sphereTraceVector3Add(imposedLinearMomentum, pData->value);
		pData = pData->pNext;
	}

	//set velocity
	ST_Vector3 imposedVelocity = sphereTraceVector3Scale(imposedLinearMomentum, 1.0f/pRigidBody->mass);

	//return the imposed next position
	return sphereTraceVector3Add(pRigidBody->position, sphereTraceVector3Scale(imposedVelocity, dt));
}



void sphereTraceSimulationStepQuantities(const ST_SimulationSpace* const pSimulationSpace, float dt)
{
	ST_IndexListData* pild = pSimulationSpace->sphereColliders.pFirst;
	ST_SphereCollider* pSphereCollider;
	for (int i = 0; i < pSimulationSpace->sphereColliders.count; i++)
	{
		pSphereCollider = pild->value;
		sphereTraceSimulationStepQuantity(pSimulationSpace, &pSphereCollider->rigidBody, dt);
		pild = pild->pNext;
	}

	pild = pSimulationSpace->spherePairColliders.pFirst;
	ST_SpherePair* pSpherePair;
	for (int i = 0; i < pSimulationSpace->spherePairColliders.count; i++)
	{
		pSpherePair = pild->value;
		sphereTraceSimulationStepQuantity(pSimulationSpace, &pSpherePair->rigidBody, dt);
		pild = pild->pNext;
	}

	ST_BoxCollider* pBoxCollider;
	pild = pSimulationSpace->boxColliders.pFirst;
	for (int i = 0; i < pSimulationSpace->boxColliders.count; i++)
	{
		pBoxCollider = pild->value;
		if (pBoxCollider->rigidBody.isAsleep)
		{
			pild = pild->pNext;
			continue;
		}
		sphereTraceSimulationStepQuantity(pSimulationSpace, &pBoxCollider->rigidBody, dt);
		pild = pild->pNext;
	}
}

void sphereTraceSimulationSpherePlaneResponse(const ST_SimulationSpace* const pSimulationSpace, const ST_SphereContact* const pContactInfo, float dt)
{
	ST_SphereCollider* pSphereCollider = sphereTraceColliderSphereGetFromContact(pContactInfo);
	ST_PlaneCollider* pPlaneCollider = sphereTraceColliderPlaneGetFromContact(pContactInfo);
	pSphereCollider->rigidBody.position = sphereTraceVector3Add(pSphereCollider->rigidBody.position, sphereTraceVector3Scale(pContactInfo->normal.v, pContactInfo->penetrationDistance));
	ST_Vector3 r = sphereTraceVector3Subtract(pContactInfo->point, pSphereCollider->rigidBody.position);
	float vnMag = sphereTraceVector3Dot(pContactInfo->normal.v, pSphereCollider->rigidBody.velocity);
	ST_Vector3 vn = sphereTraceVector3Scale(pContactInfo->normal.v, vnMag);
	ST_Vector3 vt = sphereTraceVector3Subtract(pSphereCollider->rigidBody.velocity, vn);
	float j = -(1.0f + pSimulationSpace->defaultMaterial.restitution) * vnMag * pSphereCollider->rigidBody.mass;
	ST_Vector3 dp = sphereTraceVector3Scale(pContactInfo->normal.v, j);
	float slope = sphereTraceVector3Dot(pContactInfo->normal.v, gVector3Up);

	if (sphereTraceAbs(vnMag) > ST_VELOCITY_THRESHOLD || slope < 0.0f)
	{
		pSphereCollider->restingContact = ST_FALSE;
		sphereTraceRigidBodyAddDeltaMomentum(&pSphereCollider->rigidBody, dp);
		ST_Vector3 ft = sphereTraceVector3Scale(sphereTraceVector3Normalize(vt), -sphereTraceVector3Length(vt) * j / vnMag);
		ST_Vector3 dl = sphereTraceVector3Cross(r, sphereTraceVector3Scale(ft, -pSphereCollider->radius));
		//torque will be nan when ft is in direction of normal
		if (!sphereTraceVector3Nan(dl))
			sphereTraceRigidBodyAddDeltaAngularMomentum(&pSphereCollider->rigidBody, dl);
	}
	else
	{
		if((1.0f-slope)<ST_SPHERE_RESTING_SLOPE)
			pSphereCollider->restingContact = ST_TRUE;
		pSphereCollider->rigidBody.linearMomentum = sphereTraceVector3Cross(sphereTraceVector3Cross(pContactInfo->normal.v, pSphereCollider->rigidBody.linearMomentum), pContactInfo->normal.v);
		sphereTraceRigidBodySetAngularVelocity(&pSphereCollider->rigidBody, sphereTraceVector3Cross(pContactInfo->normal.v, sphereTraceVector3Scale(pSphereCollider->rigidBody.velocity, 1.0f / pSphereCollider->radius)));
		sphereTraceRigidBodyAddForce(&pSphereCollider->rigidBody, sphereTraceVector3Scale(sphereTraceVector3Normalize(sphereTraceVector3Scale(pSphereCollider->rigidBody.velocity, 
			sphereTraceVector3Dot(pSimulationSpace->gravitationalAcceleration, pContactInfo->normal.v))), pSimulationSpace->defaultMaterial.staticFriction));
	}
}

void sphereTraceSimulationSphereTriangleResponse(const ST_SimulationSpace* const pSimulationSpace, const ST_SphereContact* const pContactInfo, float dt)
{
	ST_SphereCollider* pSphereCollider = sphereTraceColliderSphereGetFromContact(pContactInfo);;
	ST_TriangleCollider* pTriangleCollider = sphereTraceColliderTriangleGetFromContact(pContactInfo);;
	pSphereCollider->rigidBody.position = sphereTraceVector3Add(pSphereCollider->rigidBody.position, sphereTraceVector3Scale(pContactInfo->normal.v, pContactInfo->penetrationDistance));
	ST_Vector3 r = sphereTraceVector3Subtract(pContactInfo->point, pSphereCollider->rigidBody.position);
	float vnMag = sphereTraceVector3Dot(pContactInfo->normal.v, pSphereCollider->rigidBody.velocity);
	ST_Vector3 vn = sphereTraceVector3Scale(pContactInfo->normal.v, vnMag);
	ST_Vector3 vt = sphereTraceVector3Subtract(pSphereCollider->rigidBody.velocity, vn);
	float j = -(1.0f + pSimulationSpace->defaultMaterial.restitution) * vnMag * pSphereCollider->rigidBody.mass;
	ST_Vector3 dp = sphereTraceVector3Scale(pContactInfo->normal.v, j);
	float slope = sphereTraceVector3Dot(pContactInfo->normal.v, gVector3Up);

	if (sphereTraceAbs(vnMag) > ST_VELOCITY_THRESHOLD || slope < 0.0f)
	{
		pSphereCollider->restingContact = ST_FALSE;
		sphereTraceRigidBodyAddDeltaMomentum(&pSphereCollider->rigidBody, dp);
		ST_Vector3 ft = sphereTraceVector3Scale(sphereTraceVector3Normalize(vt), -sphereTraceVector3Length(vt) * j / vnMag);
		ST_Vector3 dl = sphereTraceVector3Cross(r, sphereTraceVector3Scale(ft, -pSphereCollider->radius));
		//torque will be nan when ft is in direction of normal
		if (!sphereTraceVector3Nan(dl))
			sphereTraceRigidBodyAddDeltaAngularMomentum(&pSphereCollider->rigidBody, dl);
	}
	else
	{
		if ((1.0f - slope) < ST_SPHERE_RESTING_SLOPE)
			pSphereCollider->restingContact = ST_TRUE;
		pSphereCollider->rigidBody.linearMomentum = sphereTraceVector3Cross(sphereTraceVector3Cross(pContactInfo->normal.v, pSphereCollider->rigidBody.linearMomentum), pContactInfo->normal.v);
		sphereTraceRigidBodySetAngularVelocity(&pSphereCollider->rigidBody, sphereTraceVector3Cross(pContactInfo->normal.v, sphereTraceVector3Scale(pSphereCollider->rigidBody.velocity, 1.0f / pSphereCollider->radius)));
		ST_Vector3 f = sphereTraceVector3Scale(sphereTraceVector3Normalize(sphereTraceVector3Scale(pSphereCollider->rigidBody.velocity, 
			sphereTraceVector3Dot(pSimulationSpace->gravitationalAcceleration, pContactInfo->normal.v))), pSimulationSpace->defaultMaterial.staticFriction);
		if (!sphereTraceVector3Nan(f))
			sphereTraceRigidBodyAddForce(&pSphereCollider->rigidBody, f);
	}
}

b32 sphereTraceSimulationSphereContactIsRestingCheck(const ST_SphereContact* const pContactInfo)
{
	ST_SphereCollider* pSphereCollider = sphereTraceColliderSphereGetFromContact(pContactInfo);
	float vnMag = sphereTraceVector3Dot(pContactInfo->normal.v, pSphereCollider->rigidBody.velocity);
	return sphereTraceAbs(vnMag) < ST_VELOCITY_THRESHOLD;
}

void sphereTraceSimulationSphereContactResponse(const ST_SimulationSpace* const pSimulationSpace, const ST_SphereContact* const pContactInfo, float dt, ST_Index* pNumImpulses)
{
	//ST_SphereCollider* pSphereCollider = sphereTraceColliderSphereGetFromContact(pContactInfo);
	ST_SphereCollider* pSphereCollider = pContactInfo->pSphereCollider;
	pSphereCollider->rigidBody.position = sphereTraceVector3Add(pSphereCollider->rigidBody.position, sphereTraceVector3Scale(pContactInfo->normal.v, pContactInfo->penetrationDistance));
	ST_Vector3 r = sphereTraceVector3Subtract(pContactInfo->point, pSphereCollider->rigidBody.position);
	float vnMag = sphereTraceVector3Dot(pContactInfo->normal.v, pSphereCollider->rigidBody.velocity);
	ST_Vector3 vn = sphereTraceVector3Scale(pContactInfo->normal.v, vnMag);
	ST_Vector3 vt = sphereTraceVector3Subtract(pSphereCollider->rigidBody.velocity, vn);
	float j = -(1.0f + pSimulationSpace->defaultMaterial.restitution) * vnMag * pSphereCollider->rigidBody.mass;
	//float slope = sphereTraceVector3Dot(pContactInfo->normal.v, gVector3Up);
	b32 restingContactCondition = (sphereTraceAbs(vnMag) < ST_VELOCITY_THRESHOLD);
	float accelNormal = sphereTraceAbs(vnMag);
	//printf("accel normal: %f\n", accelNormal);
	//sphereTraceVector3Print(pContactInfo->normal.v);
	if (pContactInfo->collisionType == ST_COLLISION_INWARD_CIRCULAR)
	{
		ST_Vector3 circularTangent = sphereTraceVector3Cross(pContactInfo->normal.v, pContactInfo->bitangent.v);
		float centripitalAccel = sphereTraceVector3Dot(pSphereCollider->rigidBody.velocity, circularTangent);
		if (sphereTraceAbs(centripitalAccel) / sphereTraceAbs(vnMag) > ST_VELOCITY_THRESHOLD)
		{
			centripitalAccel = centripitalAccel * centripitalAccel / pContactInfo->radiusOfCurvature - sphereTraceVector3Dot(pSimulationSpace->gravitationalAcceleration, pContactInfo->normal.v);
			if (centripitalAccel < 0.0f)
				restingContactCondition = 0;
			else
			{
				restingContactCondition = 1;
				accelNormal += centripitalAccel;
			}
		}
	}
	else if (pContactInfo->collisionType == ST_COLLISION_INWARD_SPHEREICAL)
	{
		float centripitalAccel = sphereTraceVector3Length(vt);
		if (centripitalAccel / sphereTraceAbs(vnMag) > ST_VELOCITY_THRESHOLD)
		{
			centripitalAccel = centripitalAccel * centripitalAccel / pContactInfo->radiusOfCurvature - sphereTraceVector3Dot(pSimulationSpace->gravitationalAcceleration, pContactInfo->normal.v);
			if (centripitalAccel < 0.0f)
				restingContactCondition = 0;
			else
			{
				restingContactCondition = 1;
				accelNormal += centripitalAccel;
			}
		}
	}
	if (!restingContactCondition)
	{
		pSphereCollider->restingContact = ST_FALSE;
		ST_Vector3 dp = sphereTraceVector3Scale(pContactInfo->normal.v, j);
		if (vnMag < 0.0f)
		{
			sphereTraceRigidBodyAddDeltaMomentum(&pSphereCollider->rigidBody, dp);
			*pNumImpulses = *pNumImpulses + 1;
		}
		ST_Vector3 ft = sphereTraceVector3Scale(sphereTraceVector3Normalize(vt), 
			j*sphereTraceVector3Length(vt)/sphereTraceVector3Length(pSphereCollider->rigidBody.velocity));
		ST_Vector3 dl = sphereTraceVector3Cross(r, sphereTraceVector3Scale(ft, -pSimulationSpace->defaultMaterial.kineticFriction*pSphereCollider->radius));
		if (!sphereTraceVector3NanAny(dl))
			sphereTraceRigidBodyAddDeltaAngularMomentum(&pSphereCollider->rigidBody, dl);
	}
	else
	{
		//if ((1.0f - slope) < ST_SPHERE_RESTING_SLOPE)
		pSphereCollider->restingContact = ST_TRUE;
		pSphereCollider->rigidBody.linearMomentum = sphereTraceVector3Scale(vt, pSphereCollider->rigidBody.mass);
		ST_Vector3 rollingWithoutSlipAngularVelocity = sphereTraceVector3Cross(pContactInfo->normal.v, sphereTraceVector3Scale(pSphereCollider->rigidBody.velocity, 1.0f / pSphereCollider->radius));
		float dav = sphereTraceVector3Distance(pSphereCollider->rigidBody.angularVelocity, rollingWithoutSlipAngularVelocity);

		//if (dav < 0.)
		{
			sphereTraceRigidBodyAddDeltaAngularMomentum(&pSphereCollider->rigidBody, sphereTraceVector3Scale(sphereTraceVector3Subtract(
				rollingWithoutSlipAngularVelocity, pSphereCollider->rigidBody.angularVelocity),
				(1.0f+accelNormal)*pSimulationSpace->defaultMaterial.kineticFriction*ST_KINETIC_FRICTION_MODIFIER*dt));
			//sphereTraceRigidBodySetAngularVelocity(&pSphereCollider->rigidBody, rollingWithoutSlipAngularVelocity);
		}
		//else
		{
			//sphereTraceRigidBodySetAngularVelocity(&pSphereCollider->rigidBody, rollingWithoutSlipAngularVelocity);
		}
		//ST_Vector3 f = sphereTraceVector3Scale(sphereTraceVector3Scale(sphereTraceVector3Normalize(pSphereCollider->rigidBody.velocity), 
		//	sphereTraceVector3Dot(pSimulationSpace->gravitationalAcceleration, pContactInfo->normal.v)), 
		//	(pSphereCollider->rigidBody.mass-j)*pSimulationSpace->defaultMaterial.staticFriction *ST_FRICTION_MODIFIER);
		ST_Vector3 f = sphereTraceVector3Scale(sphereTraceVector3Normalize(pSphereCollider->rigidBody.velocity),
			(-accelNormal+pSimulationSpace->gravitationalAcceleration.y) * pSphereCollider->rigidBody.mass * pSimulationSpace->defaultMaterial.staticFriction * ST_FRICTION_MODIFIER);
		if (!sphereTraceVector3NanAny(f))
			sphereTraceRigidBodyAddForce(&pSphereCollider->rigidBody, f);
	}
}

void sphereTraceSimulationBoxContactResponse(const ST_SimulationSpace* const pSimulationSpace, const ST_BoxContact* const pContactInfo, float dt)
{
	//ST_SphereCollider* pSphereCollider = sphereTraceColliderSphereGetFromContact(pContactInfo);
	ST_BoxCollider* pBoxCollider = pContactInfo->pBoxCollider;
	sphereTraceVector3AddAndScaleByRef(&pBoxCollider->rigidBody.position, pContactInfo->contactPoints[pContactInfo->maxPenetrationIndex].normal.v, 
		pContactInfo->contactPoints[pContactInfo->maxPenetrationIndex].penetrationDistance);
	ST_Direction normal = pContactInfo->contactPoints[pContactInfo->maxPenetrationIndex].normal;
	float vncom = sphereTraceVector3Dot(normal.v, pBoxCollider->rigidBody.velocity);
	ST_Vector3 vr = pBoxCollider->rigidBody.velocity;
	float vnMag = sphereTraceVector3Dot(normal.v, vr);
	ST_Vector3 vn = sphereTraceVector3Scale(normal.v, vnMag);
	float vmag = sphereTraceVector3Length(pBoxCollider->rigidBody.velocity);
	//ST_Vector3 vt = sphereTraceVector3Subtract(vr, vn);
	//ST_Vector3 vtDir = sphereTraceVector3Normalize(vt);
	float j = -1.1f * vnMag / pContactInfo->numContacts;

	ST_BoxFace upface = sphereTraceColliderBoxGetFaceClosestToDirection(pBoxCollider, normal);
	ST_Direction up = sphereTraceColliderBoxFaceGetNormal(pBoxCollider, &upface);
	ST_Vector3 cross = sphereTraceVector3Normalize(sphereTraceVector3Cross(up.v, normal.v));
	float angle = acosf(sphereTraceVector3Dot(up.v, normal.v));
	b32 restingContactCondition = (sphereTraceAbs(vmag) < (ST_VELOCITY_THRESHOLD* pBoxCollider->collider.boundingRadius));
	b32 ignoreForce = ST_FALSE;
	if (restingContactCondition && sphereTraceAbs(angle) < (0.001f * pBoxCollider->collider.boundingRadius))
	{
		if (sphereTraceAbs(angle) > ST_COLLIDER_TOLERANCE)
		{
			ST_Quaternion rot = sphereTraceQuaternionFromAngleAxis(cross, angle);
			sphereTraceRigidBodyRotateAroundPoint(&pBoxCollider->rigidBody, pContactInfo->contactPoints[pContactInfo->maxPenetrationIndex].point, rot);
		}
		sphereTraceRigidBodySetVelocity(&pBoxCollider->rigidBody, gVector3Zero);
		sphereTraceRigidBodySetAngularVelocity(&pBoxCollider->rigidBody, gVector3Zero);
		pBoxCollider->rigidBody.isAsleep = ST_TRUE;
		return;
	}
	for (int i = 0; i < pContactInfo->numContacts; i++)
	{
		ST_Vector3 r = sphereTraceVector3Subtract(pContactInfo->contactPoints[i].point, pBoxCollider->rigidBody.position);
		float rmag = sphereTraceVector3Length(r);
		ST_Vector3 rNormal = sphereTraceVector3Scale(r, 1.0f /( rmag + pBoxCollider->collider.boundingRadius));
		ST_Vector3 vp = sphereTraceVector3Add(pBoxCollider->rigidBody.velocity, sphereTraceVector3Cross(pBoxCollider->rigidBody.angularVelocity, r));
		float vpMag = sphereTraceVector3Dot(normal.v, vp);
		ST_Vector3 vpn = sphereTraceVector3Scale(normal.v, vpMag);
		ST_Vector3 vt = sphereTraceVector3Subtract(vp, vpn);
		ST_Vector3 vtDir = sphereTraceVector3Normalize(vt);
		b32 restingContactCondition = (sphereTraceAbs(vpMag) < ST_VELOCITY_THRESHOLD);
		float accelNormal = sphereTraceAbs(vnMag);
		//j = -1.1f * vpMag / pContactInfo->numContacts;
		ST_Vector3 dp = sphereTraceVector3Scale(normal.v, j);
		float mag = sphereTraceVector3Dot(normal.v, r);
		if (vnMag < 0.0f)
		{
			if (!ignoreForce)
			{
				if (vncom < 0.0f)
				{
					sphereTraceRigidBodyAddDeltaMomentum(&pBoxCollider->rigidBody, sphereTraceVector3Scale(dp, -mag / rmag));
					//sphereTraceRigidBodyAddDeltaMomentum(&pBoxCollider->rigidBody, dp);
				}
				sphereTraceRigidBodyAddDeltaAngularMomentum(&pBoxCollider->rigidBody, sphereTraceVector3Scale(sphereTraceVector3Cross(rNormal, dp),
					1.0f));
			}

			ST_Vector3 dpt;

			if (restingContactCondition)
			{
				dpt = sphereTraceVector3Scale(vtDir, -j * pSimulationSpace->defaultMaterial.staticFriction *
					sphereTraceVector3Length(vt) / sphereTraceVector3Length(vr));
			}
			else
			{
				dpt =  sphereTraceVector3Scale(vtDir, -j * pSimulationSpace->defaultMaterial.kineticFriction *
					pSimulationSpace->defaultMaterial.kineticFriction *
					sphereTraceVector3Length(vt) / sphereTraceVector3Length(vr));
			}
			if (!sphereTraceVector3NanAny(dpt))
				sphereTraceRigidBodyAddDeltaMomentum(&pBoxCollider->rigidBody, dpt);
		}


	}

}

void sphereTraceSimulationSphereTerrainTriangleResponse(const ST_SimulationSpace* const pSimulationSpace, const ST_SphereContact* const pContactInfo, float dt)
{
	//ST_SphereCollider* pSphereCollider = pContactInfo->sphereTriangleContactInfo.pSphereCollider;
	//ST_TriangleCollider* pTriangleCollider = pContactInfo->sphereTriangleContactInfo.pTriangleCollider;
	////pSphereCollider->pRigidBody->position = sphereTraceVector3Add(pSphereCollider->pRigidBody->position, sphereTraceVector3Scale(pContactInfo->normal, pContactInfo->penetrationDistance));
	//pSphereCollider->rigidBody.position = pContactInfo->downSphereTraceData.sphereCenter;
	//ST_Vector3 r = sphereTraceVector3Subtract(pContactInfo->downSphereTraceData.rayTraceData.contact.point, pSphereCollider->rigidBody.position);
	//float vnMag = sphereTraceVector3Dot(pContactInfo->downSphereTraceData.rayTraceData.contact.normal.v, pSphereCollider->rigidBody.velocity);
	//ST_Vector3 vn = sphereTraceVector3Scale(pContactInfo->downSphereTraceData.rayTraceData.contact.normal.v, vnMag);
	//ST_Vector3 vt = sphereTraceVector3Subtract(pSphereCollider->rigidBody.velocity, vn);
	//float j = -(1.0f + pSimulationSpace->eta) * vnMag * pSphereCollider->rigidBody.mass;
	//ST_Vector3 dp = sphereTraceVector3Scale(pContactInfo->downSphereTraceData.rayTraceData.contact.normal.v, j);
	//float slope = sphereTraceVector3Dot(pContactInfo->downSphereTraceData.rayTraceData.contact.normal.v, gVector3Up);

	//if (sphereTraceAbs(vnMag) > ST_VELOCITY_THRESHOLD || slope < 0.0f)
	//{
	//	sphereTraceRigidBodyAddDeltaMomentum(&pSphereCollider->rigidBody, dp);
	//	ST_Vector3 ft = sphereTraceVector3Scale(sphereTraceVector3Normalize(vt), -sphereTraceVector3Length(vt) * j / vnMag);
	//	ST_Vector3 dl = sphereTraceVector3Cross(r, sphereTraceVector3Scale(ft, -pSphereCollider->radius));
	//	//torque will be nan when ft is in direction of normal
	//	if (!sphereTraceVector3Nan(dl))
	//		sphereTraceRigidBodyAddDeltaAngularMomentum(&pSphereCollider->rigidBody, dl);
	//}
	//else
	//{
	//	pSphereCollider->rigidBody.linearMomentum = sphereTraceVector3Cross(sphereTraceVector3Cross(pContactInfo->downSphereTraceData.rayTraceData.contact.normal.v, pSphereCollider->rigidBody.linearMomentum), pContactInfo->downSphereTraceData.rayTraceData.contact.normal.v);
	//	sphereTraceRigidBodySetAngularVelocity(&pSphereCollider->rigidBody, sphereTraceVector3Cross(pContactInfo->downSphereTraceData.rayTraceData.contact.normal.v, sphereTraceVector3Scale(pSphereCollider->rigidBody.velocity, 1.0f / pSphereCollider->radius)));
	//	ST_Vector3 f = sphereTraceVector3Scale(sphereTraceVector3Normalize(sphereTraceVector3Scale(pSphereCollider->rigidBody.velocity, sphereTraceVector3Dot(pSimulationSpace->gravitationalAcceleration, pContactInfo->downSphereTraceData.rayTraceData.contact.normal.v))), pSimulationSpace->friction);
	//	if (!sphereTraceVector3Nan(f))
	//		sphereTraceRigidBodyAddForce(&pSphereCollider->rigidBody, f);
	//}
}

void sphereTraceSimulationSphereSphereResponse(const ST_SimulationSpace* const pSimulationSpace, const ST_SphereContact* const pContactInfo, float dt)
{
	ST_SphereCollider* pA = pContactInfo->pSphereCollider;
	ST_SphereCollider* pB = pContactInfo->pOtherCollider;
	pA->rigidBody.position = sphereTraceVector3AddAndScale(pA->rigidBody.position, pContactInfo->normal.v, -pContactInfo->penetrationDistance * 0.5f);
	pB->rigidBody.position = sphereTraceVector3AddAndScale(pB->rigidBody.position, pContactInfo->normal.v, pContactInfo->penetrationDistance * 0.5f);
	ST_Vector3 relativeVelocity = sphereTraceVector3Subtract(pB->rigidBody.velocity, pA->rigidBody.velocity);
	float sRel = sphereTraceVector3Dot(relativeVelocity, pContactInfo->normal.v);
	if (sphereTraceAbs(sRel) > ST_VELOCITY_THRESHOLD)
	{
		ST_Vector3 jdt = sphereTraceVector3Scale(pContactInfo->normal.v, -(1.0f + pSimulationSpace->defaultMaterial.restitution) * sRel/ ((1.0f / pA->rigidBody.mass) + (1.0f / pB->rigidBody.mass)));
		sphereTraceRigidBodyAddDeltaMomentum(&pB->rigidBody, jdt);
		sphereTraceRigidBodyAddDeltaMomentum(&pA->rigidBody, sphereTraceVector3Negative(jdt));
	}
}


void sphereTraceSimulationBoxBoxResponse(const ST_SimulationSpace* const pSimulationSpace, const ST_BoxContact* const pContactInfo, float dt, b32* pConvertToRestingContact)
{
	//ST_SphereCollider* pSphereCollider = sphereTraceColliderSphereGetFromContact(pContactInfo);
	ST_BoxCollider* pBoxColliderA = pContactInfo->pBoxCollider;
	ST_BoxCollider* pBoxColliderB = pContactInfo->pOtherCollider;
	if (!pBoxColliderA->rigidBody.isAsleep && !pBoxColliderB->rigidBody.isAsleep)
	{
		sphereTraceVector3AddAndScaleByRef(&pBoxColliderA->rigidBody.position, pContactInfo->contactPoints[pContactInfo->maxPenetrationIndex].normal.v,
			pContactInfo->contactPoints[pContactInfo->maxPenetrationIndex].penetrationDistance * 0.5f);
		sphereTraceVector3AddAndScaleByRef(&pBoxColliderB->rigidBody.position, pContactInfo->contactPoints[pContactInfo->maxPenetrationIndex].normal.v,
			-pContactInfo->contactPoints[pContactInfo->maxPenetrationIndex].penetrationDistance * 0.5f);
	}

	ST_Direction normal = pContactInfo->contactPoints[pContactInfo->maxPenetrationIndex].normal;
	ST_Vector3 vrel = sphereTraceVector3Subtract(pBoxColliderA->rigidBody.velocity, pBoxColliderB->rigidBody.velocity);
	float vncom = sphereTraceVector3Dot(normal.v, vrel);
	//ST_Vector3 vr = pBoxColliderA->rigidBody.velocity;
	float vnMag = sphereTraceVector3Dot(normal.v, vrel);
	ST_Vector3 vn = sphereTraceVector3Scale(normal.v, vnMag);
	//ST_Vector3 vt = sphereTraceVector3Subtract(vr, vn);
	//ST_Vector3 vtDir = sphereTraceVector3Normalize(vt);
	float j = -1.1f * vnMag / pContactInfo->numContacts;

	b32 restingContactCondition = sphereTraceAbs(vnMag) < ST_VELOCITY_THRESHOLD;
	if (!restingContactCondition)
	{
		pBoxColliderA->rigidBody.isAsleep = ST_FALSE;
		pBoxColliderB->rigidBody.isAsleep = ST_FALSE;
	}
	else if(pBoxColliderB->rigidBody.isAsleep)
	{
		*pConvertToRestingContact = ST_TRUE;
		return;
	}
	//if (pBoxColliderA->rigidBody.isAsleep && !pBoxColliderB->rigidBody.isAsleep)
	//{
	//	sphereTraceVector3AddAndScaleByRef(&pBoxColliderB->rigidBody.position, pContactInfo->contactPoints[pContactInfo->maxPenetrationIndex].normal.v,
	//		-pContactInfo->contactPoints[pContactInfo->maxPenetrationIndex].penetrationDistance);
	//}
	//else if(!pBoxColliderA->rigidBody.isAsleep && pBoxColliderB->rigidBody.isAsleep)
	//{
	//	sphereTraceVector3AddAndScaleByRef(&pBoxColliderA->rigidBody.position, pContactInfo->contactPoints[pContactInfo->maxPenetrationIndex].normal.v,
	//		pContactInfo->contactPoints[pContactInfo->maxPenetrationIndex].penetrationDistance);
	//}
	for (int i = 0; i < pContactInfo->numContacts; i++)
	{
		normal = pContactInfo->contactPoints[i].normal;
		ST_Vector3 ra = sphereTraceVector3Subtract(pContactInfo->contactPoints[i].point, pBoxColliderA->rigidBody.position);
		ST_Vector3 rb = sphereTraceVector3Subtract(pContactInfo->contactPoints[i].point, pBoxColliderB->rigidBody.position);
		float ramag = sphereTraceVector3Length(ra);
		ST_Vector3 vpa = sphereTraceVector3Add(pBoxColliderA->rigidBody.velocity, sphereTraceVector3Cross(pBoxColliderA->rigidBody.angularVelocity, ra));
		ST_Vector3 vpb = sphereTraceVector3Add(pBoxColliderB->rigidBody.velocity, sphereTraceVector3Cross(pBoxColliderB->rigidBody.angularVelocity, rb));
		ST_Vector3 vprel = sphereTraceVector3Subtract(vpa, vpb);
		float vpMag = sphereTraceVector3Dot(normal.v, vprel);
		ST_Vector3 vpn = sphereTraceVector3Scale(normal.v, vpMag);
		ST_Vector3 vt = sphereTraceVector3Subtract(vprel, vpn);
		ST_Vector3 vtDir = sphereTraceVector3Normalize(vt);
		b32 restingContactCondition = (sphereTraceAbs(vpMag) < ST_VELOCITY_THRESHOLD);
		float accelNormal = sphereTraceAbs(vnMag);
		j = -pSimulationSpace->defaultMaterial.restitution * vpMag * 
			(1.0f/((1.0f / pBoxColliderA->rigidBody.mass) + (1.0f / pBoxColliderB->rigidBody.mass))) / pContactInfo->numContacts;
		ST_Vector3 dp = sphereTraceVector3Scale(normal.v, j);
		if (vnMag < 0.0f)
		{
			if (vncom < 0.0f)
			{
				sphereTraceRigidBodyAddDeltaMomentum(&pBoxColliderA->rigidBody, sphereTraceVector3Scale(dp, 1.0f));
				sphereTraceRigidBodyAddDeltaMomentum(&pBoxColliderB->rigidBody, sphereTraceVector3Scale(dp, -1.0f));
			}
			sphereTraceRigidBodyAddDeltaAngularMomentum(&pBoxColliderA->rigidBody, sphereTraceVector3Cross(ra, dp));
			sphereTraceRigidBodyAddDeltaAngularMomentum(&pBoxColliderB->rigidBody, sphereTraceVector3Cross( dp, rb));
		}
	}
}

void sphereTraceSimulationAddCurFrameContactEntry(ST_SphereContact* const pContact)
{
	ST_Collider* pColliderA = pContact->pSphereCollider;
	ST_Collider* pColliderB = pContact->pOtherCollider;

	if (pColliderA->subscriberList.hasSubscriber)
	{
		ST_SphereContactEntry* pContactEntryA = sphereTraceAllocatorAllocateContactEntry();
		pContactEntryA->pOtherCollider = pColliderB;
		pContactEntryA->contact = *pContact;
		sphereTraceIndexListAddUnique(&pColliderA->subscriberList.curFrameContactEntries, pContactEntryA);
		//sphereTraceIndexListAddUnique(pCollidersThatHaveSubscribers, pColliderA);
	}

	if (pColliderB->subscriberList.hasSubscriber)
	{
		ST_SphereContactEntry* pContactEntryB = sphereTraceAllocatorAllocateContactEntry();
		pContactEntryB->pOtherCollider = pColliderA;
		pContactEntryB->contact = *pContact;
		sphereTraceIndexListAddUnique(&pColliderB->subscriberList.curFrameContactEntries, pContactEntryB);
		//sphereTraceIndexListAddUnique(pCollidersThatHaveSubscribers, pColliderB);
	}
}

void sphereTraceSimulationExecuteCallbacksOnCollider(ST_Collider* const pCollider)
{
	ST_IndexListData* pildCur;
	ST_IndexListData* pildPrev;
	ST_IndexListData* pNext;
	ST_IndexListData* pCallbackListData;
	ST_SphereContactEntry* pCurContactEntry;
	ST_SphereContactEntry* pPrevContactEntry;
	

	pildCur = pCollider->subscriberList.curFrameContactEntries.pFirst;
	for (int i = 0; i < pCollider->subscriberList.curFrameContactEntries.count; i++)
	{
		pCurContactEntry = pildCur->value;
		pildPrev = pCollider->subscriberList.contactEntries.pFirst;
		b32 foundEntry = 0;
		for (int j = 0; j < pCollider->subscriberList.contactEntries.count; j++)
		{
			pPrevContactEntry = pildPrev->value;
			if (pCurContactEntry->pOtherCollider == pPrevContactEntry->pOtherCollider)
			{
				foundEntry = 1;
				break;
			}
			pildPrev = pildPrev->pNext;
		}

		//if the entry isnt found, then execute the on trigger enters
		if (!foundEntry)
		{
			ST_CallbackFunction* pCallback;
			pCallbackListData = pCollider->subscriberList.onCollisionEnterCallbacks.pFirst;
			for (int j = 0; j < pCollider->subscriberList.onCollisionEnterCallbacks.count; j++)
			{
				pCallback = pCallbackListData->value;
				pCallback->callback(&pCurContactEntry->contact, pCurContactEntry->pOtherCollider, pCollider->subscriberList.pSubscriberContext);
				pCallbackListData = pCallbackListData->pNext;
			}

			//we can also call the collision stay for this collider as needed
			pCallbackListData = pCollider->subscriberList.onCollisionStayCallbacks.pFirst;
			for (int j = 0; j < pCollider->subscriberList.onCollisionStayCallbacks.count; j++)
			{
				pCallback = pCallbackListData->value;
				pCallback->callback(&pCurContactEntry->contact, pCurContactEntry->pOtherCollider, pCollider->subscriberList.pSubscriberContext);
				pCallbackListData = pCallbackListData->pNext;
			}
		}
		else
		{
			//the entry was found again, call collision stay callback
			ST_CallbackFunction* pCallback;
			pCallbackListData = pCollider->subscriberList.onCollisionStayCallbacks.pFirst;
			for (int j = 0; j < pCollider->subscriberList.onCollisionStayCallbacks.count; j++)
			{
				pCallback = pCallbackListData->value;
				pCallback->callback(&pCurContactEntry->contact, pCurContactEntry->pOtherCollider, pCollider->subscriberList.pSubscriberContext);
				pCallbackListData = pCallbackListData->pNext;
			}
		}
		pildCur = pildCur->pNext;
	}

	//now to find all collisions that have left
	pildPrev = pCollider->subscriberList.contactEntries.pFirst;
	for (int i = 0; i < pCollider->subscriberList.contactEntries.count; i++)
	{
		pPrevContactEntry = pildPrev->value;
		pildCur = pCollider->subscriberList.curFrameContactEntries.pFirst;
		b32 foundEntry = 0;
		for (int j = 0; j < pCollider->subscriberList.curFrameContactEntries.count; j++)
		{
			pCurContactEntry = pildCur->value;
			if (pCurContactEntry->pOtherCollider == pPrevContactEntry->pOtherCollider)
			{
				foundEntry = 1;
				break;
			}
			pildCur = pildCur->pNext;
		}

		//if the entry on the previous frame is no longer found, then call on collision exit
		if (!foundEntry)
		{
			ST_CallbackFunction* pCallback;
			pCallbackListData = pCollider->subscriberList.onCollisionExitCallbacks.pFirst;
			for (int j = 0; j < pCollider->subscriberList.onCollisionExitCallbacks.count; j++)
			{
				pCallback = pCallbackListData->value;
				pCallback->callback(&pPrevContactEntry->contact, pPrevContactEntry->pOtherCollider, pCollider->subscriberList.pSubscriberContext);
				pCallbackListData = pCallbackListData->pNext;
			}
		}
		pildPrev = pildPrev->pNext;
	}

	pildPrev = pCollider->subscriberList.contactEntries.pFirst;
	for (int i = 0; i < pCollider->subscriberList.contactEntries.count; i++)
	{
		pNext = pildPrev->pNext;
		sphereTraceAllocatorFreeContactEntry(pildPrev->value);
		sphereTraceAllocatorFreeIndexListData(pildPrev);
		pildPrev = pNext;
	}

	pCollider->subscriberList.contactEntries = pCollider->subscriberList.curFrameContactEntries;
	pCollider->subscriberList.curFrameContactEntries.count = 0;
	pCollider->subscriberList.curFrameContactEntries.pFirst = NULL;
}


b32 sphereTraceColliderSphereTraceOut(ST_Vector3 spherePos, float sphereRadius, ST_Direction clipoutDir, ST_Collider* pCollider, ST_SphereTraceData* const pSphereCastData)
{
	switch (pCollider->colliderType)
	{
	case COLLIDER_SPHERE:;
		ST_SphereCollider* pSphere = (ST_SphereCollider*)pCollider;
		return sphereTraceColliderSphereSphereTraceOut(spherePos, sphereRadius, clipoutDir, pSphere->rigidBody.position, pSphere->radius, pSphereCastData);
		break;
	case COLLIDER_PLANE:
		return sphereTraceColliderPlaneSphereTraceOut(spherePos, sphereRadius, clipoutDir, pCollider, pSphereCastData);
		break;
	case COLLIDER_TRIANGLE:
		return sphereTraceColliderTriangleSphereTraceOut(spherePos, sphereRadius, clipoutDir, pCollider, pSphereCastData);
		break;
	case COLLIDER_AABB:
		return sphereTraceColliderAABBSphereTraceOut(spherePos, sphereRadius, clipoutDir, pCollider, pSphereCastData);
		break;
	}
}

b32 sphereTraceColliderBoxTraceOut(ST_BoxContact* const pBoxContact, ST_Direction clipoutDir, ST_BoxTraceData* const pBoxTraceData)
{
	ST_Collider* pOther = pBoxContact->pOtherCollider;
	switch (pOther->colliderType)
	{
	case COLLIDER_SPHERE:;
		break;
	case COLLIDER_PLANE:
		return sphereTraceColliderPlaneBoxTraceOut1(pBoxContact->pBoxCollider, clipoutDir, pBoxContact->pOtherCollider, pBoxTraceData);
		break;
	case COLLIDER_BOX:
		return sphereTraceColliderBoxBoxTraceOut2(pBoxContact->pBoxCollider, clipoutDir, pBoxContact->pOtherCollider, pBoxTraceData);
		break;
	case COLLIDER_TRIANGLE:
		break;
	case COLLIDER_AABB:
		break;
	}
	return ST_FALSE;
}

b32 sphereTraceColliderSphereCollisionTest(const ST_SphereCollider* const pSphereCollider, ST_Collider* pOtherCollider, ST_SphereContact* const pContact)
{
	switch (pOtherCollider->colliderType)
	{
	//case COLLIDER_SPHERE:;
	//	return sphereTraceColliderSphereSphereCollisionTest(pSphereCollider, pOtherCollider, pContact);
	//	break;
	case COLLIDER_PLANE:
		return sphereTraceColliderPlaneSphereCollisionTest(pOtherCollider, pSphereCollider, pContact);
		break;
	case COLLIDER_TRIANGLE:
		return sphereTraceColliderTriangleSphereCollisionTest(pOtherCollider, pSphereCollider, pContact);
		break;
	case COLLIDER_AABB:
		return sphereTraceColliderAABBSphereCollisionTest(pOtherCollider, pSphereCollider, pContact);
		break;
	}
}


void sphereTraceSimulationResolvePenetration(ST_SphereCollider* pSphereCollider, ST_SphereContact* pContact, PenetrationRestriction* pPenetrationRestriction)
{
	switch (pPenetrationRestriction->penetrationRestrictionType)
	{
	case ST_PENETRATION_RESTRICTION_NONE:
	{
		pSphereCollider->rigidBody.position = sphereTraceVector3Add(pSphereCollider->rigidBody.position, sphereTraceVector3Scale(pContact->normal.v, pContact->penetrationDistance));
		pPenetrationRestriction->positionOnNormal = pSphereCollider->rigidBody.position;
		pPenetrationRestriction->planeNormal = pContact->normal;
		pPenetrationRestriction->penetrationRestrictionType = ST_PENETRATION_RESTRICTION_PLANE;
	}
	break;
	case ST_PENETRATION_RESTRICTION_PLANE:
	{
		pPenetrationRestriction->tangent = sphereTraceDirectionProjectDirectionOntoPlane(pContact->normal, pPenetrationRestriction->planeNormal);
		ST_SphereTraceData std;
		sphereTraceColliderSphereTraceOut(pSphereCollider->rigidBody.position, pSphereCollider->radius,
			pPenetrationRestriction->tangent, pContact->pOtherCollider, &std);
		pSphereCollider->rigidBody.position = std.sphereCenter;
		pPenetrationRestriction->penetrationRestrictionType = ST_PENETRATION_RESTRICTION_DIRECTION;
		pContact->normal = std.rayTraceData.contact.normal;
		pContact->penetrationDistance = 0.0f;
		pContact->point = std.rayTraceData.contact.point;
	}
	break;
	case ST_PENETRATION_RESTRICTION_DIRECTION:
	{
		if (!pPenetrationRestriction->restrictionDirectionSet)
		{
			ST_Vector3 cross = sphereTraceVector3Cross(pPenetrationRestriction->tangent.v, pPenetrationRestriction->planeNormal.v);
			float d = sphereTraceVector3Dot(cross, pContact->normal.v);
			float mul = 1.0f;
			if (d < 0.0f)
				mul = -1.0f;
			pPenetrationRestriction->restrictedDir = sphereTraceDirectionConstruct(sphereTraceVector3Scale(cross, mul), 1);
			pPenetrationRestriction->restrictionDirectionSet = 1;
		}
		ST_SphereTraceData std;
		sphereTraceColliderSphereTraceOut(pSphereCollider->rigidBody.position, pSphereCollider->radius,
			pPenetrationRestriction->restrictedDir, pContact->pOtherCollider, &std);
		pSphereCollider->rigidBody.position = std.sphereCenter;
		pContact->normal = std.rayTraceData.contact.normal;
		pContact->penetrationDistance = 0.0f;
		pContact->point = std.rayTraceData.contact.point;
	}
	break;
	}
}

void sphereTraceSimulationResolveBoxPenetration(ST_BoxCollider* pBoxCollider, ST_BoxContact* pContact, PenetrationRestriction* pPenetrationRestriction)
{
	//sphereTraceColliderBoxForceSolvePenetration(pBoxCollider, pContact);
	pBoxCollider->rigidBody.position = sphereTraceVector3Add(pBoxCollider->rigidBody.position,
		sphereTraceVector3Scale(pContact->contactPoints[pContact->maxPenetrationIndex].normal.v,
			pContact->contactPoints[pContact->maxPenetrationIndex].penetrationDistance));
	return;
	switch (pPenetrationRestriction->penetrationRestrictionType)
	{
	case ST_PENETRATION_RESTRICTION_NONE:
	{
		pBoxCollider->rigidBody.position = sphereTraceVector3Add(pBoxCollider->rigidBody.position,
			sphereTraceVector3Scale(pContact->contactPoints[pContact->maxPenetrationIndex].normal.v,
				pContact->contactPoints[pContact->maxPenetrationIndex].penetrationDistance));
		//pBoxCollider->rigidBody.prevPosition = pBoxCollider->rigidBody.position;
		//pPenetrationRestriction->positionOnNormal = pBoxCollider->rigidBody.position;
		pPenetrationRestriction->planeNormal = pContact->contactPoints[pContact->maxPenetrationIndex].normal;
		pPenetrationRestriction->penetrationRestrictionType = ST_PENETRATION_RESTRICTION_PLANE;
		//ST_Collider* pcol = pContact->pOtherCollider;
		//float test = sphereTraceVector3Dot(pContact->contactPoints[pContact->maxPenetrationIndex].normal.v,
		//	sphereTraceVector3Subtract(pcol->aabb.center, pBoxCollider->rigidBody.prevPosition));
		//if (pcol->colliderType == COLLIDER_PLANE && test > 0.0f)
		//{
		//	ST_BoxTraceData btd;
		//	pPenetrationRestriction->planeNormal = sphereTraceDirectionNegative(pPenetrationRestriction->planeNormal);
		//	sphereTraceColliderBoxTraceOut(pContact, pPenetrationRestriction->planeNormal, &btd);
		//	pBoxCollider->rigidBody.position = btd.boxCenter;
		//	for (int i = 0; i < btd.numContacts; i++)
		//	{
		//		pContact->contactPoints[i] = btd.contacts[i];
		//		pContact->contactPoints[i].penetrationDistance = 0.0f;
		//	}
		//	pContact->maxPenetrationIndex = 0;
		//	pContact->numContacts = btd.numContacts;
		//}
	}
	break;
	case ST_PENETRATION_RESTRICTION_PLANE:
	{
		pPenetrationRestriction->tangent = sphereTraceDirectionProjectDirectionOntoPlane(pContact->contactPoints[pContact->maxPenetrationIndex].normal,
			pPenetrationRestriction->planeNormal);
		ST_BoxTraceData btd;
		if (sphereTraceColliderBoxTraceOut(pContact, pPenetrationRestriction->tangent, &btd))
		{
			//if (sphereTraceVector3AnyGreaterThan(btd.boxCenter, 1000.0f))
			//{
			//	sphereTraceColliderBoxTraceOut(pContact, pPenetrationRestriction->tangent, &btd);
			//}
			pBoxCollider->rigidBody.position = btd.boxCenter;
			pPenetrationRestriction->penetrationRestrictionType = ST_PENETRATION_RESTRICTION_DIRECTION;
			for (int i = 0; i < btd.numContacts; i++)
			{
				pContact->contactPoints[i] = btd.contacts[i];
				pContact->contactPoints[i].penetrationDistance = 0.0f;
			}
			pContact->maxPenetrationIndex = 0;
			pContact->numContacts = btd.numContacts;
		}
		else
		{
			if (pContact->pOtherCollider->colliderType == COLLIDER_BOX)
			{
				//ST_PlaneCollider pc;
				//sphereTraceColliderBoxConstructImposedPlaneWithFace(pContact->pOtherCollider, pContact->otherBoxPlaneDirection, &pc.position, &pc.normal,
				//	&pc.right, &pc.forward, &pc.xHalfExtent, &pc.zHalfExtent);
				ST_BoxContact bc;
				if (sphereTraceColliderBoxBoxCollisionTest(pContact->pBoxCollider, pContact->pOtherCollider, &bc));
				//if (sphereTraceColliderPlaneBoxCollisionTest(pContact->pBoxCollider, pContact->pOtherCollider, &bc));
				//{
				//	
				//}
					sphereTraceColliderBoxTraceOut(pContact, pPenetrationRestriction->tangent, &btd);
			}
			
			
			pBoxCollider->rigidBody.position = sphereTraceVector3Add(pBoxCollider->rigidBody.position,
				sphereTraceVector3Scale(pContact->contactPoints[pContact->maxPenetrationIndex].normal.v,
					pContact->contactPoints[pContact->maxPenetrationIndex].penetrationDistance));
		}

	}
	break;
	case ST_PENETRATION_RESTRICTION_DIRECTION:
	{
		if (!pPenetrationRestriction->restrictionDirectionSet)
		{
			ST_Vector3 cross = sphereTraceVector3Cross(pPenetrationRestriction->tangent.v, pPenetrationRestriction->planeNormal.v);
			float d = sphereTraceVector3Dot(cross, pContact->contactPoints[pContact->maxPenetrationIndex].normal.v);
			//float d = sphereTraceVector3Dot(cross, sphereTraceVector3Subtract(pBoxCollider->rigidBody.position, pContact->contactPoints[pContact->maxPenetrationIndex].point));
			float mul = 1.0f;
			if (d < 0.0f)
				mul = -1.0f;
			pPenetrationRestriction->restrictedDir = sphereTraceDirectionConstruct(sphereTraceVector3Scale(cross, mul), 1);
			pPenetrationRestriction->restrictionDirectionSet = 1;
		}
		ST_BoxTraceData btd;
		if (sphereTraceColliderBoxTraceOut(pContact, pPenetrationRestriction->restrictedDir, &btd))
		{
			pBoxCollider->rigidBody.position = btd.boxCenter;
			for (int i = 0; i < btd.numContacts; i++)
			{
				pContact->contactPoints[i] = btd.contacts[i];
				pContact->contactPoints[i].penetrationDistance = 0.0f;
			}
			pContact->maxPenetrationIndex = 0;
			pContact->numContacts = btd.numContacts;
		}
		else
		{
			if (pContact->pOtherCollider->colliderType == COLLIDER_BOX)
			{
				ST_PlaneCollider pc;
				sphereTraceColliderBoxConstructImposedPlaneWithFace(pContact->pOtherCollider, pContact->otherBoxPlaneDirection, &pc.position, &pc.normal,
					&pc.right, &pc.forward, &pc.xHalfExtent, &pc.zHalfExtent);
				ST_BoxContact bc;
				if (sphereTraceColliderBoxBoxCollisionTest(pContact->pBoxCollider, pContact->pOtherCollider, &bc));
				if (sphereTraceColliderPlaneBoxCollisionTest(pContact->pBoxCollider, pContact->pOtherCollider, &bc))
				{
					sphereTraceColliderBoxTraceOut(pContact, pPenetrationRestriction->tangent, &btd);
				}
			}

			pBoxCollider->rigidBody.position = sphereTraceVector3Add(pBoxCollider->rigidBody.position,
				sphereTraceVector3Scale(pContact->contactPoints[pContact->maxPenetrationIndex].normal.v,
					pContact->contactPoints[pContact->maxPenetrationIndex].penetrationDistance));
		}
	}
	break;
	}
}


void sphereTraceSimulationSphereMultipleContactResponse(const ST_SimulationSpace* const pSimulationSpace, ST_SphereCollider* pSphereCollider, float dt)
{
	ST_SphereContact* pRestingContactWithMaxForce = NULL;
	float maxAccel = 0.0f;
	ST_Vector3List impulses = sphereTraceVector3ListConstruct();
	ST_Vector3List restingContactNormals = sphereTraceVector3ListConstruct();
	ST_Vector3List restingContactPoints = sphereTraceVector3ListConstruct();
	ST_Index numContacts = sphereTraceLinearAllocatorGetSphereContactCount();
	for (ST_Index contactIndex = 0; contactIndex < numContacts; contactIndex++)
	{
		ST_SphereContact* pContactInfo = sphereTraceLinearAllocatorGetSphereContactByIndex(contactIndex);
		ST_Vector3 r = sphereTraceVector3Subtract(pContactInfo->point, pSphereCollider->rigidBody.position);
		float vnMag = sphereTraceVector3Dot(pContactInfo->normal.v, pSphereCollider->rigidBody.velocity);
		ST_Vector3 vn = sphereTraceVector3Scale(pContactInfo->normal.v, vnMag);
		ST_Vector3 vt = sphereTraceVector3Subtract(pSphereCollider->rigidBody.velocity, vn);
		float j = -(1.0f + pSimulationSpace->defaultMaterial.restitution) * vnMag * (pSphereCollider->rigidBody.mass / (float)numContacts);
		b32 restingContactCondition = (sphereTraceAbs(vnMag) < ST_VELOCITY_THRESHOLD && pContactInfo->normal.v.y>0.0f);
		float accelNormal = sphereTraceAbs(vnMag);
		//printf("accel normal: %f\n", accelNormal);
		if (!restingContactCondition)
		{
			if (vnMag < 0.0f)
			{
				ST_Vector3 dp = sphereTraceVector3Scale(pContactInfo->normal.v, j);
				sphereTraceVector3ListAddFirst(&impulses, dp);
			}
			ST_Vector3 dl = sphereTraceVector3Cross(sphereTraceVector3Normalize(pSphereCollider->rigidBody.velocity), sphereTraceVector3Scale(pContactInfo->normal.v, -pSimulationSpace->defaultMaterial.kineticFriction * j));
			if (!sphereTraceVector3NanAny(dl))
				sphereTraceRigidBodyAddDeltaAngularMomentum(&pSphereCollider->rigidBody, dl);
			
		}
		else
		{
			if (accelNormal > maxAccel)
			{
				pRestingContactWithMaxForce = pContactInfo;
				maxAccel = accelNormal;
			}
			sphereTraceVector3ListAddFirst(&restingContactNormals, pContactInfo->normal.v);
			sphereTraceVector3ListAddFirst(&restingContactPoints, pContactInfo->point);


		}
	}

	if (pRestingContactWithMaxForce)
	{
		ST_Vector3 r = sphereTraceVector3Subtract(pRestingContactWithMaxForce->point, pSphereCollider->rigidBody.position);
		float vnMag = sphereTraceVector3Dot(pRestingContactWithMaxForce->normal.v, pSphereCollider->rigidBody.velocity);
		ST_Vector3 vn = sphereTraceVector3Scale(pRestingContactWithMaxForce->normal.v, vnMag);
		ST_Vector3 vt = sphereTraceVector3Subtract(pSphereCollider->rigidBody.velocity, vn);
		float j = -(1.0f + pSimulationSpace->defaultMaterial.restitution) * vnMag * pSphereCollider->rigidBody.mass;
		float accelNormal = sphereTraceAbs(vnMag);
		//pSphereCollider->restingContact = ST_TRUE;
		pSphereCollider->rigidBody.linearMomentum = sphereTraceVector3Scale(vt, pSphereCollider->rigidBody.mass);
		ST_Vector3 actualVelocity = sphereTraceVector3Subtract(pSphereCollider->rigidBody.position, pSphereCollider->rigidBody.prevPosition);
		sphereTraceVector3ScaleByRef(&actualVelocity, 1.0f/dt);
		//float actualvnMag = sphereTraceVector3Dot(pRestingContactWithMaxForce->normal.v, actualVelocity);
		//ST_Vector3 actualvn = sphereTraceVector3Scale(pRestingContactWithMaxForce->normal.v, actualvnMag);
		//ST_Vector3 actualvt = sphereTraceVector3Subtract(actualVelocity, actualvn);
		ST_Vector3 rollingWithoutSlipAngularVelocity = sphereTraceVector3Cross(pRestingContactWithMaxForce->normal.v, sphereTraceVector3Scale(vt, 1.0f / pSphereCollider->radius));
		if(pRestingContactWithMaxForce->collisionType==ST_COLLISION_FACE)
			sphereTraceRigidBodySetAngularVelocity(&pSphereCollider->rigidBody, rollingWithoutSlipAngularVelocity);
		//float dav = sphereTraceVector3Distance(pSphereCollider->rigidBody.angularVelocity, rollingWithoutSlipAngularVelocity);
		//sphereTraceRigidBodyAddDeltaAngularMomentum(&pSphereCollider->rigidBody, sphereTraceVector3Scale(sphereTraceVector3Subtract(
		//	rollingWithoutSlipAngularVelocity, pSphereCollider->rigidBody.angularVelocity),
		//	(1.0f + accelNormal) * pSimulationSpace->defaultMaterial.kineticFriction * ST_KINETIC_FRICTION_MODIFIER * dt));
		ST_Vector3 f = sphereTraceVector3Scale(sphereTraceVector3Normalize(pSphereCollider->rigidBody.velocity),
			(-accelNormal + pSimulationSpace->gravitationalAcceleration.y) * pSphereCollider->rigidBody.mass * pSimulationSpace->defaultMaterial.staticFriction * ST_FRICTION_MODIFIER);
		if (!sphereTraceVector3NanAny(f))
			sphereTraceRigidBodyAddForce(&pSphereCollider->rigidBody, f);
	}

	
	ST_Vector3ListData* pvld = impulses.pFirst;
	ST_Vector3 forceSum = gVector3Zero;
	for (int i = 0; i < impulses.count; i++)
	{

		ST_Vector3 modifiedForce = pvld->value;
		ST_Vector3ListData* pRestingContactNormalsData = restingContactNormals.pFirst;
		for (int j = 0; j < restingContactNormals.count; j++)
		{
			float fnm = sphereTraceVector3Dot(modifiedForce, pRestingContactNormalsData->value);
			if (fnm < 0.0f)
			{
				ST_Vector3 fn = sphereTraceVector3Scale(pRestingContactNormalsData->value, fnm);
				modifiedForce = sphereTraceVector3Subtract(modifiedForce, fn);
			}
			pRestingContactNormalsData = pRestingContactNormalsData->pNext;
		}

		//sphereTraceRigidBodyAddDeltaMomentum(&pSphereCollider->rigidBody, modifiedForce);
		sphereTraceVector3AddByRef(&forceSum, modifiedForce);
		pvld = pvld->pNext;
	}
	sphereTraceRigidBodyAddDeltaMomentum(&pSphereCollider->rigidBody, forceSum);
	sphereTraceVector3ListFree(&impulses);
	sphereTraceVector3ListFree(&restingContactNormals);
	sphereTraceVector3ListFree(&restingContactPoints);

}

void sphereTraceSimulationBoxMultiContactResponse(const ST_SimulationSpace* const pSimulationSpace, ST_BoxCollider* const pBoxCollider, float dt)
{
	ST_Index numContacts = sphereTraceLinearAllocatorGetBoxContactCount();
	ST_Vector3 linearMomentum = pBoxCollider->rigidBody.linearMomentum;
	ST_Vector3 angularMomentum = pBoxCollider->rigidBody.angularMomentum;
	ST_Index totalContacts = 0;
	ST_Vector3List impulses = sphereTraceVector3ListConstruct();
	ST_Vector3List restingContactNormals = sphereTraceVector3ListConstruct();
	ST_Vector3List radiusNormalized = sphereTraceVector3ListConstruct();
	ST_ContactPoint lerpPoints[3];
	int numLerpPoints = 0;
	for (ST_Index contactIndex = 0; contactIndex < numContacts; contactIndex++)
	{
		ST_BoxContact* pContactInfo = sphereTraceLinearAllocatorGetBoxContactByIndex(contactIndex);
		totalContacts += pContactInfo->numContacts;
		if (pContactInfo->numLerpPoints > 0 && numLerpPoints<3)
		{
			for (int i = 0; i < pContactInfo->numLerpPoints; i++)
			{
				lerpPoints[numLerpPoints++] = pContactInfo->lerpPoints[i];
			}
		}
		ST_Direction normal = pContactInfo->contactPoints[pContactInfo->maxPenetrationIndex].normal;
		float vncom = sphereTraceVector3Dot(normal.v, pBoxCollider->rigidBody.velocity);
		ST_Vector3 vr = pBoxCollider->rigidBody.velocity;
		float vnMag = sphereTraceVector3Dot(normal.v, vr);
		ST_Vector3 vn = sphereTraceVector3Scale(normal.v, vnMag);
		float vmag = sphereTraceVector3Length(pBoxCollider->rigidBody.velocity);
		//ST_Vector3 vt = sphereTraceVector3Subtract(vr, vn);
		//ST_Vector3 vtDir = sphereTraceVector3Normalize(vt);
		float jcom = -pSimulationSpace->defaultMaterial.restitution * vnMag / pContactInfo->numContacts;

		for (int i = 0; i < pContactInfo->numContacts; i++)
		{
			ST_Vector3 r = sphereTraceVector3Subtract(pContactInfo->contactPoints[i].point, pBoxCollider->rigidBody.position);
			float rmag = sphereTraceVector3Length(r);
			normal = pContactInfo->contactPoints[i].normal;
			sphereTraceVector3ListAddFirst(&restingContactNormals, normal.v);
			//ST_Vector3 rNormal = sphereTraceVector3Scale(r, 1.0f / (rmag + pBoxCollider->collider.boundingRadius));
			ST_Vector3 rNormal = sphereTraceVector3Scale(r, 1.0f / (rmag));
			sphereTraceVector3ListAddFirst(&radiusNormalized, rNormal);
			sphereTraceVector3ListAddFirst(&radiusNormalized, rNormal);
			ST_Vector3 vp = sphereTraceVector3Add(pBoxCollider->rigidBody.velocity, sphereTraceVector3Cross(pBoxCollider->rigidBody.angularVelocity, r));
			float vpMag = sphereTraceVector3Dot(normal.v, vp);
			ST_Vector3 vpn = sphereTraceVector3Scale(normal.v, vpMag);
			ST_Vector3 vt = sphereTraceVector3Subtract(vp, vpn);
			ST_Vector3 vtDir = sphereTraceVector3Normalize(vt);
			b32 restingContactCondition = (sphereTraceAbs(vpMag) < ST_VELOCITY_THRESHOLD);
			float accelNormal = sphereTraceAbs(vnMag);
			float gForce = sphereTraceVector3Dot(pSimulationSpace->gravitationalAcceleration, normal.v) * dt;
			float j = -pSimulationSpace->defaultMaterial.restitution * (vpMag+gForce) / pContactInfo->numContacts;
			ST_Vector3 dp = sphereTraceVector3Scale(normal.v, j);
			float mag = sphereTraceVector3Dot(normal.v, r);
			if (vnMag < 0.0f)
			{

				if (vncom < 0.0f)
				{
					//sphereTraceVector3AddByRef(&linearMomentum, sphereTraceVector3Scale(dp, -mag / rmag));
					sphereTraceVector3ListAddFirst(&impulses, dp);
					//sphereTraceVector3AddByRef(&angularMomentum, sphereTraceVector3Cross(rNormal, dp));
					//delta momentum tangential and tortional
					ST_Vector3 dptangential;

					if (restingContactCondition)
					{
						dptangential = sphereTraceVector3Scale(vtDir, -jcom * pSimulationSpace->defaultMaterial.staticFriction *
							sphereTraceVector3Length(vt) / sphereTraceVector3Length(vr));
					}
					else
					{
						dptangential = sphereTraceVector3Scale(vtDir, -jcom * pSimulationSpace->defaultMaterial.kineticFriction *
							pSimulationSpace->defaultMaterial.kineticFriction *
							sphereTraceVector3Length(vt) / sphereTraceVector3Length(vr));
					}
					if (!sphereTraceVector3NanAny(dptangential))
					{
						//sphereTraceVector3AddByRef(&linearMomentum, dptangential);
						//sphereTraceVector3ListAddFirst(&impulses, dptangential);
						sphereTraceVector3AddByRef(&angularMomentum, sphereTraceVector3Cross(rNormal, dptangential));
					}

					float twist = sphereTraceVector3Dot(normal.v, pBoxCollider->rigidBody.angularMomentum);
					sphereTraceVector3AddByRef(&angularMomentum, sphereTraceVector3Scale(normal.v, -twist*jcom *
						pSimulationSpace->defaultMaterial.kineticFriction));
				}

			}
		}
	}
	if (numContacts > 0)
	{
		float speed = sphereTraceVector3Length(linearMomentum) / pBoxCollider->rigidBody.mass;
		float angularSpeed = sphereTraceVector3Length(angularMomentum) / pBoxCollider->rigidBody.inertia;
		b32 sleepCond = (speed < ST_VELOCITY_THRESHOLD && angularSpeed < ST_VELOCITY_THRESHOLD && numContacts>2);
		b32 speedThreshold = speed < ST_VELOCITY_THRESHOLD && angularSpeed < ST_VELOCITY_THRESHOLD;
		b32 contactThreshold = numContacts > 2;
		if (speedThreshold)
		{
			if (numLerpPoints > 0)
			{
				
			}
		}
		if (sleepCond)
		{

			pBoxCollider->rigidBody.isAsleep = ST_TRUE;

		}
		else
		{
			//pBoxCollider->rigidBody.linearMomentum = linearMomentum;
			
			ST_Vector3ListData* pvld = impulses.pFirst;
			ST_Vector3 forceSum = gVector3Zero;
			ST_Vector3ListData* prnld = radiusNormalized.pFirst;
			for (int i = 0; i < impulses.count; i++)
			{

				ST_Vector3 modifiedForce = pvld->value;
				ST_Vector3ListData* pRestingContactNormalsData = restingContactNormals.pFirst;
				for (int j = 0; j < restingContactNormals.count; j++)
				{
					float fnm = sphereTraceVector3Dot(modifiedForce, pRestingContactNormalsData->value);
					if (fnm < 0.0f)
					{
						ST_Vector3 fn = sphereTraceVector3Scale(pRestingContactNormalsData->value, fnm);
						modifiedForce = sphereTraceVector3Subtract(modifiedForce, fn);
					}
					pRestingContactNormalsData = pRestingContactNormalsData->pNext;
				}

				//sphereTraceRigidBodyAddDeltaMomentum(&pSphereCollider->rigidBody, modifiedForce);
				sphereTraceVector3AddByRef(&forceSum, modifiedForce);
				ST_Vector3 rnorm = prnld->value;
				sphereTraceVector3AddByRef(&angularMomentum, sphereTraceVector3Scale(sphereTraceVector3Cross(rnorm, modifiedForce), 3.0f));
				pvld = pvld->pNext;
				prnld = prnld->pNext;
			}
			pBoxCollider->rigidBody.linearMomentum = forceSum;
			pBoxCollider->rigidBody.angularMomentum = angularMomentum;
		}
	}
	sphereTraceVector3ListFree(&impulses);
	sphereTraceVector3ListFree(&radiusNormalized);
	sphereTraceVector3ListFree(&restingContactNormals);

}



void sphereTraceSimulationOctTreeGridSolveDiscrete(ST_SimulationSpace* const pSimulationSpace, float dt)
{
	dt = sphereTraceMin(dt, pSimulationSpace->minDeltaTime);
	if (dt < 0.0f)
		dt = pSimulationSpace->minDeltaTime;

	//step all quantities
	sphereTraceSimulationStepQuantities(pSimulationSpace, dt);


	//update all sphere aabb's 
	ST_IndexListData* pSphereIndexData;
	pSphereIndexData = pSimulationSpace->sphereColliders.pFirst;
	for (int i = 0; i < pSimulationSpace->sphereColliders.count; i++)
	{
		ST_SphereCollider* pSphereCollider = (ST_SphereCollider*)pSphereIndexData->value;
		sphereTraceColliderSphereAABBSetTransformedVertices(pSphereCollider);
		sphereTraceOctTreeGridReInsertCollider(&pSimulationSpace->octTreeGrid, &pSphereCollider->collider, ST_FALSE);
		pSphereIndexData = pSphereIndexData->pNext;
	}


	ST_IndexListData* pOtherIndexData;

	PenetrationRestriction penetrationRestriction;

	//ST_SphereContact contacts[ST_CONTACT_MAX];
	ST_Index contactsCount;

	////first do all sphere-sphere collisions
	pSphereIndexData = pSimulationSpace->sphereColliders.pFirst;
	for (int sphereColliderIndex = 0; sphereColliderIndex < pSimulationSpace->sphereColliders.count; sphereColliderIndex++)
	{
		ST_SphereCollider* pSphereCollider = (ST_SphereCollider*)pSphereIndexData->value;
		ST_SphereContact contactInfo;
		ST_IndexList handledCollisionList = sphereTraceIndexListConstruct();
		ST_IndexList sampledColliders = sphereTraceIndexListConstruct();
		sphereTraceOctTreeGridReSampleIntersectionLeafsAndColliders(&pSimulationSpace->octTreeGrid, &pSphereCollider->collider.aabb, NULL, &sampledColliders, ST_TRUE, ST_FALSE);
		sphereTraceSortedIndexListRemove(&sampledColliders, &pSphereCollider->collider);
		pOtherIndexData = sampledColliders.pFirst;
		ST_Collider* pOtherCollider;
		for (ST_Index i = 0; i < sampledColliders.count; i++)
		{
			pOtherCollider = pOtherIndexData->value;
			if (!sphereTraceSortedIndexListContains(&handledCollisionList, pOtherCollider))
			{
				if (pOtherCollider->colliderType == COLLIDER_SPHERE)
				{
					ST_SphereCollider* pSphereColliderB = (ST_PlaneCollider*)pOtherCollider;
					if (sphereTraceColliderSphereSphereCollisionTest(pSphereCollider, pSphereColliderB, &contactInfo))
					{
						sphereTraceSimulationAddCurFrameContactEntry(&contactInfo);
						sphereTraceSimulationSphereSphereResponse(pSimulationSpace, &contactInfo, dt);
						sphereTraceColliderSphereAABBSetTransformedVertices(pSphereCollider);
						sphereTraceColliderSphereAABBSetTransformedVertices(pSphereColliderB);
						sphereTraceOctTreeGridReInsertCollider(&pSimulationSpace->octTreeGrid, &pSphereCollider->collider, ST_FALSE);
						sphereTraceOctTreeGridReInsertCollider(&pSimulationSpace->octTreeGrid, &pSphereColliderB->collider, ST_FALSE);
						sphereTraceIndexListFree(&sampledColliders);
						sphereTraceOctTreeGridReSampleIntersectionLeafsAndColliders(&pSimulationSpace->octTreeGrid, &pSphereCollider->collider.aabb, NULL, &sampledColliders, ST_TRUE, ST_FALSE);
						sphereTraceSortedIndexListRemove(&sampledColliders, &pSphereCollider->collider);
						sphereTraceSortedIndexListAddUnique(&handledCollisionList, pOtherCollider);
						i = 0;
						pOtherIndexData = sampledColliders.pFirst;
					}
				}
			}
			if (sampledColliders.count == 0)
				break;
			pOtherIndexData = pOtherIndexData->pNext;
		}
		sphereTraceIndexListFree(&sampledColliders);
		sphereTraceIndexListFree(&handledCollisionList);

		pSphereIndexData = pSphereIndexData->pNext;
	}

	ST_Collider* pOtherCollider;
	////move all static colliders that have moved
	//pOtherIndexData = pSimulationSpace->planeColliders.pFirst;
	//for (int i = 0; i < pSimulationSpace->planeColliders.count; i++)
	//{

	//	pOtherIndexData = pOtherIndexData->pNext;
	//}

	//check for all sphere plane collisions
	pSphereIndexData = pSimulationSpace->sphereColliders.pFirst;
	for (int sphereColliderIndex = 0; sphereColliderIndex < pSimulationSpace->sphereColliders.count; sphereColliderIndex++)
	{
		ST_SphereCollider* pSphereCollider = (ST_SphereCollider*)pSphereIndexData->value;
		ST_SphereContact contactInfo;
		penetrationRestriction.penetrationRestrictionType = ST_PENETRATION_RESTRICTION_NONE;
		penetrationRestriction.restrictionDirectionSet = 0;
		contactsCount = 0;
		sphereTraceLinearAllocatorResetSphereContacts();

		ST_IndexList handledCollisionList = sphereTraceIndexListConstruct();
		ST_IndexList sampledColliders = sphereTraceIndexListConstruct();
		sphereTraceOctTreeGridReSampleIntersectionLeafsAndColliders(&pSimulationSpace->octTreeGrid, &pSphereCollider->collider.aabb, NULL, &sampledColliders, ST_FALSE, ST_TRUE);
		sphereTraceSortedIndexListRemove(&sampledColliders, &pSphereCollider->collider);
		pOtherIndexData = sampledColliders.pFirst;
		
		for (ST_Index i = 0; i < sampledColliders.count; i++)
		{
			pOtherCollider = pOtherIndexData->value;
			if (!sphereTraceSortedIndexListContains(&handledCollisionList, pOtherCollider))
			{
				if (pOtherCollider->colliderType == COLLIDER_TERRAIN)
				{
					//handle terrain behavior
					if (sphereTraceColliderUniformTerrainSphereFindMaxPenetratingTriangle(pOtherCollider, pSphereCollider, &contactInfo))
					{
						sphereTraceSimulationAddCurFrameContactEntry(&contactInfo);
						sphereTraceSimulationResolvePenetration(pSphereCollider, &contactInfo, &penetrationRestriction);
						ST_SphereContact* pAllocatedContact = sphereTraceLinearAllocatorAllocateSphereContact();
						*pAllocatedContact = contactInfo;
						sphereTraceColliderSphereAABBSetTransformedVertices(pSphereCollider);
						sphereTraceOctTreeGridReInsertCollider(&pSimulationSpace->octTreeGrid, &pSphereCollider->collider, ST_FALSE);
						sphereTraceIndexListFree(&sampledColliders);
						sphereTraceOctTreeGridReSampleIntersectionLeafsAndColliders(&pSimulationSpace->octTreeGrid, &pSphereCollider->collider.aabb, NULL, &sampledColliders, ST_FALSE, ST_TRUE);
						sphereTraceSortedIndexListRemove(&sampledColliders, &pSphereCollider->collider);
						sphereTraceSortedIndexListAddUnique(&handledCollisionList, pOtherCollider);
						i = 0;
						pOtherIndexData = sampledColliders.pFirst;
					}
				}
				else if (sphereTraceColliderSphereCollisionTest(pSphereCollider, pOtherCollider, &contactInfo))
				{
					sphereTraceSimulationAddCurFrameContactEntry(&contactInfo);
					sphereTraceSimulationResolvePenetration(pSphereCollider, &contactInfo, &penetrationRestriction);
					ST_SphereContact* pAllocatedContact = sphereTraceLinearAllocatorAllocateSphereContact();
					*pAllocatedContact = contactInfo;
					sphereTraceColliderSphereAABBSetTransformedVertices(pSphereCollider);
					sphereTraceOctTreeGridReInsertCollider(&pSimulationSpace->octTreeGrid, &pSphereCollider->collider, ST_FALSE);
					sphereTraceIndexListFree(&sampledColliders);
					sphereTraceOctTreeGridReSampleIntersectionLeafsAndColliders(&pSimulationSpace->octTreeGrid, &pSphereCollider->collider.aabb, NULL, &sampledColliders, ST_FALSE, ST_TRUE);
					sphereTraceSortedIndexListRemove(&sampledColliders, &pSphereCollider->collider);
					sphereTraceSortedIndexListAddUnique(&handledCollisionList, pOtherCollider);
					i = 0;
					pOtherIndexData = sampledColliders.pFirst;
				}
			}
			if (sampledColliders.count == 0)
				break;
			pOtherIndexData = pOtherIndexData->pNext;
		}
		sphereTraceIndexListFree(&sampledColliders);
		sphereTraceIndexListFree(&handledCollisionList);


		ST_Vector3 dp = sphereTraceVector3Subtract(pSphereCollider->rigidBody.position, pSphereCollider->rigidBody.prevPosition);
		float wannabeSpeed = sqrtf(sphereTraceVector3Dot(dp, dp)) / dt;
		float rbSpeed = sphereTraceRigidBodyGetSpeed(&pSphereCollider->rigidBody);
		//if the actual position speed calculation is greater than the rigidbody speed
		//some sort of teleportation is taking place due to the penetration constraints
		//and we need to cap the teleportation
		if (wannabeSpeed > rbSpeed)
		{
			//printf("speed: %f, actual speed: %f\n", actualSpeed, rbSpeed);
			ST_Vector3 dir = sphereTraceVector3Normalize(dp);
			sphereTraceVector3AddAndScaleByRef(&pSphereCollider->rigidBody.prevPosition, dir, rbSpeed * dt);
		}

		//printf("contacts count: %i\n", contactsCount);
		sphereTraceSimulationSphereMultipleContactResponse(pSimulationSpace, pSphereCollider, dt);

		//if (sphereTraceLinearAllocatorGetSphereContactCount() > 0)
		//{
		//	rbSpeed = sphereTraceRigidBodyGetSpeed(&pSphereCollider->rigidBody);
		//	if (rbSpeed < ST_RESTING_SPEED_SQUARED)
		//	{
		//		sphereTraceVector3ScaleByRef(&pSphereCollider->rigidBody.linearMomentum, 0.0f);
		//	}
		//	if (wannabeSpeed < ST_RESTING_SPEED_SQUARED && rbSpeed>0.0f)
		//	{
		//		sphereTraceVector3ScaleByRef(&pSphereCollider->rigidBody.linearMomentum, wannabeSpeed / rbSpeed);
		//	}
		//}
		pSphereIndexData = pSphereIndexData->pNext;
	}

	//execute all the callback
	pOtherIndexData = pSimulationSpace->callbackColliders.pFirst;
	for (int i = 0; i < pSimulationSpace->callbackColliders.count; i++)
	{
		sphereTraceSimulationExecuteCallbacksOnCollider(pOtherIndexData->value);
		pOtherIndexData = pOtherIndexData->pNext;
	}
}

b32 sphereTraceSimulationRayTrace(ST_SimulationSpace* const pSimulationSpace, ST_Vector3 start, ST_Direction dir, float maxDist, ST_RayTraceData* const pData)
{
	sphereTraceDirectionNormalizeIfNotNormalizedByRef(&dir);
	pData->startPoint = start;
	ST_RayTraceData rtd;
	ST_Vector3 curStart;
	ST_Index breakCond = (ST_Index)-1;
	float minDist = maxDist;
	b32 retVal = ST_FALSE;
	ST_RayTraceData rtdReturn;
	if (sphereTraceColliderEmptyAABBRayTrace(start, dir, &pSimulationSpace->octTreeGrid.worldaabb, &rtd))
	{
		ST_Index curIndex;
		if (rtd.distance == 0.0f)
			curIndex = sphereTraceOctTreeGridGetBucketIndexFromPosition(&pSimulationSpace->octTreeGrid, rtd.contact.point);
		else
		{
			curIndex = sphereTraceOctTreeGridGetBucketIndexFromPosition(&pSimulationSpace->octTreeGrid,
				sphereTraceVector3AddAndScale(rtd.contact.point, rtd.contact.normal.v, -pSimulationSpace->octTreeGrid.minDim));
		}
		curStart = rtd.contact.point;
		float cumDist = rtd.distance;
		if (rtd.distance > maxDist)
			return 0;
		while (curIndex != breakCond)
		{
			if (sphereTraceColliderAABBRayTraceThrough(curStart, dir, &pSimulationSpace->octTreeGrid.treeBuckets[curIndex].root->aabb, &rtd))
			{
				if (sphereTraceOctTreeRayTraceFromWithin(curStart, dir, rtd.distance, &pSimulationSpace->octTreeGrid.treeBuckets[curIndex], &rtdReturn))
				{
					float fullDist = rtdReturn.distance + cumDist;
					if (fullDist < minDist)
					{
						minDist = fullDist;
						*pData = rtdReturn;
						pData->startPoint = start;
						pData->distance = fullDist;
						retVal = ST_TRUE;
					}
				}
				curIndex = sphereTraceOctTreeGridGetNextIndexByDirection(&pSimulationSpace->octTreeGrid, curIndex, rtd.directionType);
				curStart = rtd.contact.point;
				cumDist += rtd.distance;
				if (cumDist > minDist)
					return retVal;
			}
			else
				return ST_FALSE;
		}
		//if (sphereTraceColliderListRayTrace(start, dir, &pGrid->outsideColliders, pData))
		//	return ST_TRUE;
		return ST_FALSE;
	}
	return ST_FALSE;
}