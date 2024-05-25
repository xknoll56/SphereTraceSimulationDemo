#include "SphereTraceExperimental.h"

void sphereTraceSimulationGlobalSolveDiscreteFirstComeFirstServe(ST_SimulationSpace* const pSimulationSpace, float dt, ST_Index iterations)
{
	dt = sphereTraceMin(dt, pSimulationSpace->minDeltaTime);
	if (dt < 0.0f)
		dt = pSimulationSpace->minDeltaTime;
	//clear cur frame colliders
	//sphereTraceSimulationClearAllCurFrameContacts(pSimulationSpace);

	//update all sphere aabb's 
	ST_IndexListData* pSphereIndexData;
	ST_IndexListData* pOtherIndexData;



	float substepDt = dt / (float)iterations;
	for (ST_Index iters = 0; iters < iterations; iters++)
	{
		pSphereIndexData = pSimulationSpace->sphereColliders.pFirst;
		for (int sphereColliderIndex = 0; sphereColliderIndex < pSimulationSpace->sphereColliders.count; sphereColliderIndex++)
		{
			ST_SphereCollider* pSphereCollider = (ST_SphereCollider*)pSphereIndexData->value;
			sphereTraceColliderSphereAABBSetTransformedVertices(pSphereCollider);
			if (iters == 0)
				sphereTraceSimulationApplyForcesAndTorques(pSimulationSpace, &pSphereCollider->rigidBody, dt, 0);
			pSphereIndexData = pSphereIndexData->pNext;
		}

		sphereTraceSimulationStepQuantities(pSimulationSpace, substepDt);

		//ST_IndexList collidersThatHaveSubscribers = sphereTraceIndexListConstruct();

		//check for all sphere plane collisions
		pSphereIndexData = pSimulationSpace->sphereColliders.pFirst;
		for (int sphereColliderIndex = 0; sphereColliderIndex < pSimulationSpace->sphereColliders.count; sphereColliderIndex++)
		{
			ST_SphereCollider* pSphereCollider = (ST_SphereCollider*)pSphereIndexData->value;
			ST_Index numImpulses = 0;
			pOtherIndexData = pSimulationSpace->planeColliders.pFirst;
			for (int planeColliderIndex = 0; planeColliderIndex < pSimulationSpace->planeColliders.count; planeColliderIndex++)
			{
				ST_PlaneCollider* pPlaneCollider = (ST_PlaneCollider*)pOtherIndexData->value;
				ST_SphereContact contactInfo;
				if (sphereTraceColliderPlaneSphereCollisionTest(pPlaneCollider, pSphereCollider, &contactInfo))
				{
					sphereTraceSimulationAddCurFrameContactEntry(&contactInfo);
					sphereTraceSimulationSphereContactResponse(pSimulationSpace, &contactInfo, substepDt, &numImpulses);
				}
				pOtherIndexData = pOtherIndexData->pNext;
			}

			pOtherIndexData = pSimulationSpace->triangleColliders.pFirst;
			for (int triangleColliderIndex = 0; triangleColliderIndex < pSimulationSpace->triangleColliders.count; triangleColliderIndex++)
			{
				ST_TriangleCollider* pTriangleCollider = (ST_TriangleCollider*)pOtherIndexData->value;
				ST_SphereContact contactInfo;
				if (sphereTraceColliderTriangleSphereCollisionTest(pTriangleCollider, pSphereCollider, &contactInfo))
				{
					sphereTraceSimulationAddCurFrameContactEntry(&contactInfo);
					sphereTraceSimulationSphereContactResponse(pSimulationSpace, &contactInfo, substepDt, &numImpulses);
				}
				pOtherIndexData = pOtherIndexData->pNext;
			}

			pOtherIndexData = pSimulationSpace->uniformTerrainColliders.pFirst;
			for (int terrainColliderIndex = 0; terrainColliderIndex < pSimulationSpace->uniformTerrainColliders.count; terrainColliderIndex++)
			{
				ST_UniformTerrainCollider* pTerrainCollider = (ST_UniformTerrainCollider*)pOtherIndexData->value;
				ST_SphereContact contactInfo;
				if (sphereTraceColliderUniformTerrainSphereFindMaxPenetratingTriangle(pTerrainCollider, pSphereCollider, &contactInfo))
				{
					sphereTraceSimulationAddCurFrameContactEntry(&contactInfo);
					sphereTraceSimulationSphereContactResponse(pSimulationSpace, &contactInfo, substepDt, &numImpulses);
				}
				pOtherIndexData = pOtherIndexData->pNext;
			}

			//pOtherIndexData = pSimulationSpace->bowlColliders.pFirst;
			//for (int bowlColliderIndex = 0; bowlColliderIndex < pSimulationSpace->bowlColliders.count; bowlColliderIndex++)
			//{
			//	ST_BowlCollider* pBowlCollider = (ST_BowlCollider*)pOtherIndexData->value;
			//	ST_SphereContact contactInfo;
			//	if (sphereTraceColliderBowlSphereCollisionTest(pBowlCollider, pSphereCollider, &contactInfo))
			//	{
			//		sphereTraceSimulationAddCurFrameContactEntry(&contactInfo);
			//		sphereTraceSimulationSphereContactResponse(pSimulationSpace, &contactInfo, substepDt, &numImpulses);
			//	}
			//	pOtherIndexData = pOtherIndexData->pNext;
			//}

			//pOtherIndexData = pSimulationSpace->pipeColliders.pFirst;
			//for (int pipeColliderIndex = 0; pipeColliderIndex < pSimulationSpace->pipeColliders.count; pipeColliderIndex++)
			//{
			//	ST_PipeCollider* pPipeCollider = (ST_PipeCollider*)pOtherIndexData->value;
			//	ST_SphereContact contactInfo;
			//	if (sphereTraceColliderPipeSphereCollisionTest(pPipeCollider, pSphereCollider, &contactInfo))
			//	{
			//		sphereTraceSimulationAddCurFrameContactEntry(&contactInfo);
			//		sphereTraceSimulationSphereContactResponse(pSimulationSpace, &contactInfo, substepDt, &numImpulses);
			//	}
			//	pOtherIndexData = pOtherIndexData->pNext;
			//}

			//check for all sphere sphere collisions
			pOtherIndexData = pSimulationSpace->sphereColliders.pFirst->pNext;
			for (int sphereColliderBIndex = sphereColliderIndex + 1; sphereColliderBIndex < pSimulationSpace->sphereColliders.count; sphereColliderBIndex++)
			{
				ST_SphereCollider* pSphereColliderB = (ST_SphereCollider*)pOtherIndexData->value;
				ST_SphereContact contactInfo;
				if (sphereTraceColliderSphereSphereCollisionTest(pSphereCollider, pSphereColliderB, &contactInfo))
				{
					sphereTraceSimulationAddCurFrameContactEntry(&contactInfo);
					sphereTraceSimulationSphereSphereResponse(pSimulationSpace, &contactInfo, substepDt);
				}
				pOtherIndexData = pOtherIndexData->pNext;
			}

			pOtherIndexData = pSphereCollider->rigidBody.appliedDeltaMomentums.pFirst;
			float s = 1.0f / (float)numImpulses;
			for (int i = 0; i < pSphereCollider->rigidBody.appliedDeltaMomentums.count; i++)
			{
				sphereTraceVector3ScaleByRef(&pOtherIndexData->value, s);
			}

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
}


void sphereTraceSimulationGlobalSolveDiscrete(ST_SimulationSpace* const pSimulationSpace, float dt)
{
	dt = sphereTraceMin(dt, pSimulationSpace->minDeltaTime);
	if (dt < 0.0f)
		dt = pSimulationSpace->minDeltaTime;

	//step all quantities
	sphereTraceSimulationStepQuantities(pSimulationSpace, dt);

	//update all sphere aabb's 
	ST_IndexListData* pSphereIndexData;
	ST_IndexListData* pOtherIndexData;
	pSphereIndexData = pSimulationSpace->sphereColliders.pFirst;
	for (int sphereColliderIndex = 0; sphereColliderIndex < pSimulationSpace->sphereColliders.count; sphereColliderIndex++)
	{
		ST_SphereCollider* pSphereCollider = (ST_SphereCollider*)pSphereIndexData->value;
		sphereTraceColliderSphereAABBSetTransformedVertices(pSphereCollider);
		pSphereIndexData = pSphereIndexData->pNext;
	}

	PenetrationRestriction penetrationRestriction;

	//ST_SphereContact contacts[ST_CONTACT_MAX];
	ST_Index contactsCount;

	//first do all sphere-sphere collisions
	pSphereIndexData = pSimulationSpace->sphereColliders.pFirst;
	for (int sphereColliderIndex = 0; sphereColliderIndex < pSimulationSpace->sphereColliders.count; sphereColliderIndex++)
	{
		ST_SphereCollider* pSphereCollider = (ST_SphereCollider*)pSphereIndexData->value;

		//check for all sphere sphere collisions
		pOtherIndexData = pSimulationSpace->sphereColliders.pFirst;
		for (int sphereColliderBIndex = 0; sphereColliderBIndex < pSimulationSpace->sphereColliders.count; sphereColliderBIndex++)
		{
			ST_SphereCollider* pSphereColliderB = (ST_SphereCollider*)pOtherIndexData->value;

			if (pSphereCollider != pSphereColliderB)
			{
				ST_SphereContact contactInfo;
				if (sphereTraceColliderSphereSphereCollisionTest(pSphereCollider, pSphereColliderB, &contactInfo))
				{
					sphereTraceSimulationAddCurFrameContactEntry(&contactInfo);
					sphereTraceSimulationSphereSphereResponse(pSimulationSpace, &contactInfo, dt);
				}
			}
			pOtherIndexData = pOtherIndexData->pNext;
		}
		pSphereIndexData = pSphereIndexData->pNext;
	}

	//check for all sphere plane collisions
	pSphereIndexData = pSimulationSpace->sphereColliders.pFirst;
	for (int sphereColliderIndex = 0; sphereColliderIndex < pSimulationSpace->sphereColliders.count; sphereColliderIndex++)
	{
		ST_SphereCollider* pSphereCollider = (ST_SphereCollider*)pSphereIndexData->value;
		ST_SphereContact contactInfo;
		//penetrationRestriction.penetrationRestrictionType = ST_PENETRATION_RESTRICTION_NONE;
		//penetrationRestriction.restrictionDirectionSet = 0;
		penetrationRestriction = sphereTracePenetrationRestrictionConstruct();
		contactsCount = 0;
		sphereTraceLinearAllocatorResetSphereContacts();

		pOtherIndexData = pSimulationSpace->planeColliders.pFirst;
		for (int planeColliderIndex = 0; planeColliderIndex < pSimulationSpace->planeColliders.count; planeColliderIndex++)
		{
			ST_PlaneCollider* pPlaneCollider = (ST_PlaneCollider*)pOtherIndexData->value;
			if (sphereTraceColliderPlaneSphereCollisionTest(pPlaneCollider, pSphereCollider, &contactInfo))
			{
				if (!sphereTraceIndexListContains(&pSphereCollider->prevFrameContacts, pPlaneCollider))
				{
					sphereTraceSimulationAddCurFrameContactEntry(&contactInfo);
					sphereTraceSimulationResolvePenetration(pSphereCollider, &contactInfo, &penetrationRestriction);
					ST_SphereContact* pAllocatedContact = sphereTraceLinearAllocatorAllocateSphereContact();
					*pAllocatedContact = contactInfo;
				}
			}
			pOtherIndexData = pOtherIndexData->pNext;
		}



		pOtherIndexData = pSimulationSpace->triangleColliders.pFirst;
		for (int triangleColliderIndex = 0; triangleColliderIndex < pSimulationSpace->triangleColliders.count; triangleColliderIndex++)
		{
			ST_TriangleCollider* pTriangleCollider = (ST_TriangleCollider*)pOtherIndexData->value;
			if (sphereTraceColliderTriangleSphereCollisionTest(pTriangleCollider, pSphereCollider, &contactInfo))
			{
				if (!sphereTraceIndexListContains(&pSphereCollider->prevFrameContacts, pTriangleCollider))
				{
					sphereTraceSimulationAddCurFrameContactEntry(&contactInfo);
					sphereTraceSimulationResolvePenetration(pSphereCollider, &contactInfo, &penetrationRestriction);
					ST_SphereContact* pAllocatedContact = sphereTraceLinearAllocatorAllocateSphereContact();
					*pAllocatedContact = contactInfo;
				}
			}
			pOtherIndexData = pOtherIndexData->pNext;
		}

		pOtherIndexData = pSimulationSpace->uniformTerrainColliders.pFirst;
		for (int terrainColliderIndex = 0; terrainColliderIndex < pSimulationSpace->uniformTerrainColliders.count; terrainColliderIndex++)
		{
			ST_UniformTerrainCollider* pTerrainCollider = (ST_UniformTerrainCollider*)pOtherIndexData->value;
			if (sphereTraceColliderUniformTerrainSphereFindMaxPenetratingTriangle(pTerrainCollider, pSphereCollider, &contactInfo))
			{
				if (!sphereTraceIndexListContains(&pSphereCollider->prevFrameContacts, pTerrainCollider))
				{
					sphereTraceSimulationAddCurFrameContactEntry(&contactInfo);
					sphereTraceSimulationResolvePenetration(pSphereCollider, &contactInfo, &penetrationRestriction);
					ST_SphereContact* pAllocatedContact = sphereTraceLinearAllocatorAllocateSphereContact();
					*pAllocatedContact = contactInfo;
				}
			}
			pOtherIndexData = pOtherIndexData->pNext;
		}

		ST_Vector3 dp = sphereTraceVector3Subtract(pSphereCollider->rigidBody.position, pSphereCollider->rigidBody.prevPosition);
		float actualSpeed = sqrtf(sphereTraceVector3Dot(dp, dp)) / dt;
		float rbSpeed = sphereTraceRigidBodyGetSpeed(&pSphereCollider->rigidBody);
		//if the actual position speed calculation is greater than the rigidbody speed
		//some sort of teleportation is taking place due to the penetration constraints
		//and we need to cap the teleportation
		if (actualSpeed > rbSpeed)
		{
			//printf("speed: %f, actual speed: %f\n", actualSpeed, rbSpeed);
			ST_Vector3 dir = sphereTraceVector3Normalize(dp);
			sphereTraceVector3AddAndScaleByRef(&pSphereCollider->rigidBody.prevPosition, dir, rbSpeed * dt);
		}
		//printf("contacts count: %i\n", contactsCount);
		sphereTraceSimulationSphereMultipleContactResponse(pSimulationSpace, pSphereCollider, dt);

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


void sphereTraceBoxSimulationGlobalSolveDiscrete(ST_SimulationSpace* const pSimulationSpace, float dt)
{
	dt = sphereTraceMin(dt, pSimulationSpace->minDeltaTime);
	if (dt < 0.0f)
		dt = pSimulationSpace->minDeltaTime;

	//step all quantities
	sphereTraceSimulationStepQuantities(pSimulationSpace, dt);

	//update all sphere aabb's 
	ST_IndexListData* pBoxIndexData;
	ST_IndexListData* pOtherIndexData;
	pBoxIndexData = pSimulationSpace->boxColliders.pFirst;
	for (int boxColliderIndex = 0; boxColliderIndex < pSimulationSpace->boxColliders.count; boxColliderIndex++)
	{
		ST_BoxCollider* pBoxCollider = (ST_BoxCollider*)pBoxIndexData->value;
		if (pBoxCollider->rigidBody.isAsleep)
		{
			pBoxIndexData = pBoxIndexData->pNext;
			continue;
		}
		sphereTraceColliderBoxUpdateTransform(pBoxCollider);
		pBoxIndexData = pBoxIndexData->pNext;
	}
	PenetrationRestriction penetrationRestriction;
	b32 convertToRestingContact = ST_FALSE;

	//pBoxIndexData = pSimulationSpace->boxColliders.pFirst;
	//for (int boxColliderIndex = 0; boxColliderIndex < pSimulationSpace->boxColliders.count; boxColliderIndex++)
	//{
	//	ST_BoxCollider* pBoxCollider = (ST_BoxCollider*)pBoxIndexData->value;
	//	if (pBoxCollider->rigidBody.isAsleep)
	//	{
	//		pBoxIndexData = pBoxIndexData->pNext;
	//		continue;
	//	}
	//	ST_BoxContact contactInfo;
	//	penetrationRestriction = sphereTracePenetrationRestrictionConstruct();
	//	
	//	//pOtherIndexData = pSimulationSpace->boxColliders.pFirst;
	//	//for (int otherBoxColliderIndex = 0; otherBoxColliderIndex < pSimulationSpace->boxColliders.count; otherBoxColliderIndex++)
	//	//{
	//	//	convertToRestingContact = ST_FALSE;
	//	//	ST_BoxCollider* pOtherBoxCollider = (ST_BoxCollider*)pOtherIndexData->value;
	//	//	if (otherBoxColliderIndex != boxColliderIndex)
	//	//	{
	//	//		if (sphereTraceColliderBoxBoxCollisionTest(pBoxCollider, pOtherBoxCollider, &contactInfo))
	//	//		{
	//	//			sphereTraceSimulationBoxBoxResponse(pSimulationSpace, &contactInfo, dt, &convertToRestingContact);
	//	//			if (convertToRestingContact)
	//	//			{
	//	//				sphereTraceSimulationResolveBoxPenetration(pBoxCollider, &contactInfo, &penetrationRestriction);
	//	//				sphereTraceColliderBoxSetPosition(pBoxCollider, pBoxCollider->rigidBody.position);
	//	//				ST_BoxContact* pabc = sphereTraceLinearAllocatorAllocateBoxContact();
	//	//				*pabc = contactInfo;
	//	//			}
	//	//			else
	//	//			{
	//	//				sphereTraceColliderBoxUpdateTransform(pBoxCollider);
	//	//				sphereTraceColliderBoxUpdateTransform(pOtherBoxCollider);
	//	//			}
	//	//		}
	//	//	}
	//	//	pOtherIndexData = pOtherIndexData->pNext;
	//	//}

	//	pBoxIndexData = pBoxIndexData->pNext;
	//}


	//ST_SphereContact contacts[ST_CONTACT_MAX];
	ST_Index contactsCount;

	//check for all sphere plane collisions
	pBoxIndexData = pSimulationSpace->boxColliders.pFirst;
	for (int boxColliderIndex = 0; boxColliderIndex < pSimulationSpace->boxColliders.count; boxColliderIndex++)
	{
		ST_BoxCollider* pBoxCollider = (ST_BoxCollider*)pBoxIndexData->value;
		if (pBoxCollider->rigidBody.isAsleep)
		{
			pBoxIndexData = pBoxIndexData->pNext;
			continue;
		}
		ST_IndexList restingContactsList = sphereTraceIndexListConstruct();
		ST_BoxContact contactInfo;
		penetrationRestriction = sphereTracePenetrationRestrictionConstruct();
		contactsCount = 0;
		sphereTraceLinearAllocatorResetBoxContacts();

		//resolve box-box
		pOtherIndexData = pSimulationSpace->boxColliders.pFirst;
		for (int otherBoxColliderIndex = 0; otherBoxColliderIndex < pSimulationSpace->boxColliders.count; otherBoxColliderIndex++)
		{
			convertToRestingContact = ST_FALSE;
			ST_BoxCollider* pOtherBoxCollider = (ST_BoxCollider*)pOtherIndexData->value;
			if (otherBoxColliderIndex != boxColliderIndex)
			{
				if (sphereTraceColliderBoxBoxCollisionTest(pBoxCollider, pOtherBoxCollider, &contactInfo))
				{
					sphereTraceSimulationBoxBoxResponse(pSimulationSpace, &contactInfo, dt, &convertToRestingContact);
					if (convertToRestingContact)
					{
						sphereTraceSimulationResolveBoxPenetration(pBoxCollider, &contactInfo, &penetrationRestriction);
						sphereTraceColliderBoxSetPosition(pBoxCollider, pBoxCollider->rigidBody.position);
						ST_BoxContact* pabc = sphereTraceLinearAllocatorAllocateBoxContact();
						*pabc = contactInfo;
					}
					else
					{
						sphereTraceColliderBoxUpdateTransform(pBoxCollider);
						sphereTraceColliderBoxUpdateTransform(pOtherBoxCollider);
					}
				}
			}
			pOtherIndexData = pOtherIndexData->pNext;
		}


		//resolve planes
		pOtherIndexData = pSimulationSpace->planeColliders.pFirst;
		for (int planeColliderIndex = 0; planeColliderIndex < pSimulationSpace->planeColliders.count; planeColliderIndex++)
		{
			ST_PlaneCollider* pPlaneCollider = (ST_PlaneCollider*)pOtherIndexData->value;
			if (sphereTraceColliderPlaneBoxCollisionTest(pBoxCollider, pPlaneCollider, &contactInfo))
			{
				//sphereTraceSimulationBoxContactResponse(pSimulationSpace, &contactInfo, dt);
				//sphereTraceColliderBoxUpdateTransform(pBoxCollider);
				sphereTraceSimulationResolveBoxPenetration(pBoxCollider, &contactInfo, &penetrationRestriction);
				sphereTraceColliderBoxSetPosition(pBoxCollider, pBoxCollider->rigidBody.position);
				ST_BoxContact* pabc = sphereTraceLinearAllocatorAllocateBoxContact();
				*pabc = contactInfo;
			}
			pOtherIndexData = pOtherIndexData->pNext;
		}
		float speed = sphereTraceRigidBodyGetSpeed(&pBoxCollider->rigidBody);
		float sdt = speed * dt;
		ST_Vector3 dp = sphereTraceVector3Subtract(pBoxCollider->rigidBody.position, pBoxCollider->rigidBody.prevPosition);
		float dpmag = sphereTraceVector3Length(dp);
		if (dpmag > sdt)
		{
			sphereTraceVector3ScaleByRef(&dp, 1.0f / dpmag);
			//pBoxCollider->rigidBody.position = sphereTraceVector3AddAndScale(pBoxCollider->rigidBody.prevPosition, dp, sdt);

		}
		sphereTraceSimulationBoxMultiContactResponse(pSimulationSpace, pBoxCollider, dt);
		pBoxIndexData = pBoxIndexData->pNext;
	}



	//execute all the callback
	//pOtherIndexData = pSimulationSpace->callbackColliders.pFirst;
	//for (int i = 0; i < pSimulationSpace->callbackColliders.count; i++)
	//{
	//	sphereTraceSimulationExecuteCallbacksOnCollider(pOtherIndexData->value);
	//	pOtherIndexData = pOtherIndexData->pNext;
	//}
}


void sphereTraceSimulationGlobalSolveImposedPosition(ST_SimulationSpace* const pSimulationSpace, float dt)
{
	dt = sphereTraceMin(dt, pSimulationSpace->minDeltaTime);
	if (dt < 0.0f)
		dt = pSimulationSpace->minDeltaTime;
	ST_IndexListData* pSphereIndexData;
	ST_IndexListData* pOtherSphereIndexData;
	ST_IndexListData* pPlaneIndexData;
	ST_IndexListData* pTerrainIndexData;
	ST_IndexListData* pTriangleIndexData;

	pSphereIndexData = pSimulationSpace->sphereColliders.pFirst;
	//update all sphere aabb's 
	for (int sphereColliderIndex = 0; sphereColliderIndex < pSimulationSpace->sphereColliders.count; sphereColliderIndex++)
	{
		ST_SphereCollider* pSphereCollider = (ST_SphereCollider*)pSphereIndexData->value;
		sphereTraceColliderSphereAABBSetTransformedVertices(pSphereCollider);
		pSphereIndexData = pSphereIndexData->pNext;
	}

	//check for all sphere plane collisions
	pSphereIndexData = pSimulationSpace->sphereColliders.pFirst;
	for (int sphereColliderIndex = 0; sphereColliderIndex < pSimulationSpace->sphereColliders.count; sphereColliderIndex++)
	{
		float accumulatedDt = 0.0f;
		ST_SphereCollider* pSphereCollider = (ST_SphereCollider*)pSphereIndexData->value;
		//apply all pre existing forces and torques
		sphereTraceSimulationApplyForcesAndTorques(pSimulationSpace, &pSphereCollider->rigidBody, dt, 0);
		while (accumulatedDt < dt)
		{
			//sphereTraceColliderSphereAABBSetTransformedVertices(pSphereCollider);

			ST_SphereCollider* pClosestSphere = NULL;
			ST_PlaneCollider* pClosestPlane = NULL;
			ST_UniformTerrainCollider* pClosestTerrain = NULL;
			float closestCollider = FLT_MAX;
			//float closestRay = FLT_MAX;
			ST_SphereTraceData sphereCastData;
			ST_SphereTraceData sphereCastDataClosestPlane;
			ST_SphereTraceData sphereCastDataClosestSphere;
			ST_SphereTraceData sphereCastDataClosestTerrain;
			//b32 overrideTerrainBehaviour = 0;

			ST_Vector3 imposedNextPosition = sphereTraceSimulationImposedStepPosition(pSimulationSpace, &pSphereCollider->rigidBody, dt - accumulatedDt);
			float imposedNextLength = sphereTraceVector3Length(sphereTraceVector3Subtract(imposedNextPosition, pSphereCollider->rigidBody.position));

			//check for the closest plane
			pPlaneIndexData = pSimulationSpace->planeColliders.pFirst;
			for (int planeColliderIndex = 0; planeColliderIndex < pSimulationSpace->planeColliders.count; planeColliderIndex++)
			{
				ST_PlaneCollider* pPlaneCollider = (ST_PlaneCollider*)pPlaneIndexData->value;
				if (sphereTraceColliderPlaneSphereTrace(pSphereCollider->rigidBody.position, sphereTraceDirectionConstruct(pSphereCollider->rigidBody.velocity, 0), pSphereCollider->radius, pPlaneCollider, &sphereCastData))
				{
					//float sphereCastDistance = sphereTraceVector3Length(sphereTraceVector3Subtract(sphereCastData.sphereCenter, pSphereCollider->pRigidBody->position));
					if (sphereCastData.traceDistance <= imposedNextLength)
					{
						if (sphereCastData.rayTraceData.distance < closestCollider)
						{
							//if (sphereCastData.traceDistance < closestCollider)
							//{
							pClosestPlane = pPlaneCollider;
							//closestRay = sphereCastData.rayTraceData.distance;
							closestCollider = sphereCastData.rayTraceData.distance;
							sphereCastDataClosestPlane = sphereCastData;
						}
						//}
					}
				}
				pPlaneIndexData = pPlaneIndexData->pNext;
			}

			//check for the closest terrain
			pTerrainIndexData = pSimulationSpace->uniformTerrainColliders.pFirst;
			for (int terrainColliderIndex = 0; terrainColliderIndex < pSimulationSpace->uniformTerrainColliders.count; terrainColliderIndex++)
			{
				ST_UniformTerrainCollider* pTerrainCollider = (ST_UniformTerrainCollider*)pTerrainIndexData->value;
				//if (sphereTraceColliderUniformTerrainSphereTrace(pTerrainCollider, pSphereCollider->pRigidBody->position, pSphereCollider->pRigidBody->velocity, pSphereCollider->radius, &sphereCastData))
				//ST_SphereTerrainTriangleContactInfo contactInfo;
				//if (sphereTraceColliderUniformTerrainSphereTraceByStartEndPoint(pTerrainCollider,pSphereCollider->pRigidBody->position, imposedNextPosition, pSphereCollider->radius, &sphereCastData))
				ST_TriangleCollider* pTriangleCollider = NULL;
				if (sphereTraceColliderUniformTerrainSphereTraceByStartEndPoint(pTerrainCollider, pSphereCollider->rigidBody.position, imposedNextPosition, pSphereCollider->radius, &sphereCastData, &pTriangleCollider))
				{
					if (sphereCastData.traceDistance <= imposedNextLength)
					{
						if (sphereCastData.rayTraceData.distance < closestCollider)
						{
							////if (sphereCastData.rayTraceData.distance <= closestRay)
							////{
							//	pClosestTerrain = pTerrainCollider;
							//	closestRay = sphereCastData.rayTraceData.distance;
							//	closestCollider = sphereCastData.traceDistance;
							//	sphereCastDataClosestTerrain = sphereCastData;
							//	pClosestPlane = NULL;
							//}
							//else 
							//if (sphereCastData.traceDistance == closestCollider)
							//{
							pClosestTerrain = pTerrainCollider;
							//closestRay = sphereCastData.rayTraceData.distance;
							closestCollider = sphereCastData.rayTraceData.distance;
							sphereCastDataClosestTerrain = sphereCastData;
							pClosestPlane = NULL;
							//overrideTerrainBehaviour = 1;
						}
						//}
					}
				}
				pTerrainIndexData = pTerrainIndexData->pNext;
			}
			//check for all sphere sphere collisions
			pOtherSphereIndexData = pSimulationSpace->sphereColliders.pFirst;
			for (int sphereColliderOtherIndex = sphereColliderIndex + 1; sphereColliderOtherIndex < pSimulationSpace->sphereColliders.count; sphereColliderOtherIndex++)
			{
				ST_SphereCollider* pSphereColliderOther = (ST_SphereCollider*)pOtherSphereIndexData->value;
				if (sphereTraceColliderSphereSphereTrace(pSphereCollider->rigidBody.position, sphereTraceDirectionConstructNormalized(pSphereCollider->rigidBody.velocity), pSphereCollider->radius, pSphereColliderOther, &sphereCastData))
				{
					//ST_Vector3 nextPos = 
					//float sphereCastDistance = sphereTraceVector3Length(sphereTraceVector3Subtract(sphereCastData.sphereCenter, pSphereCollider->pRigidBody->position));
					if (sphereCastData.traceDistance <= imposedNextLength)
					{
						if (sphereCastData.rayTraceData.distance < closestCollider)
						{
							//if (sphereCastData.traceDistance < closestCollider)
							//{
							pClosestSphere = pSphereColliderOther;
							pClosestPlane = NULL;
							pClosestTerrain = NULL;
							//closestRay = sphereCastData.rayTraceData.distance;
							closestCollider = sphereCastData.rayTraceData.distance;
							sphereCastDataClosestSphere = sphereCastData;
							//}
						}
					}
				}
				pOtherSphereIndexData = pOtherSphereIndexData->pNext;
			}
			//sphereTraceColliderUniformTerrainSphereTraceByStartEndPoint()
			if (closestCollider != FLT_MAX)
			{
				if (pClosestPlane != NULL)
				{
					//float sphereCastDistance = sphereTraceVector3Length(sphereTraceVector3Subtract(sphereCastDataClosestPlane.sphereCenter, pSphereCollider->pRigidBody->position));
					float sphereCastDistance = sphereCastDataClosestPlane.traceDistance;
					//float test = pSphereCollider->radius - sphereCastDataClosestPlane.rayTraceData.distance;
					//if(test>0)
					//printf("test: %f\n", test);
					float adjustedDt = sphereTraceMin((dt - accumulatedDt) * sphereCastDistance / imposedNextLength + ST_AUTO_DT_FACTOR * dt, dt - accumulatedDt);

					//step the simulation
					sphereTraceSimulationStepQuantity(pSimulationSpace, &pSphereCollider->rigidBody, adjustedDt);
					//sphereTraceVector3Print(pSphereCollider->pRigidBody->linearMomentum);

					//resolve the collisions if any
					ST_SphereContact contactInfo;
					if (sphereTraceColliderPlaneSphereCollisionTest(pClosestPlane, pSphereCollider, &contactInfo))
					{
						sphereTraceSimulationSpherePlaneResponse(pSimulationSpace, &contactInfo, adjustedDt);
					}
					//printf("after: ");
					//sphereTraceVector3Print(pSphereCollider->pRigidBody->velocity);
					accumulatedDt += adjustedDt;
				}
				else if (pClosestSphere != NULL)
				{
					//float sphereCastDistance = sphereTraceVector3Length(sphereTraceVector3Subtract(sphereCastDataClosestSphere.sphereCenter, pSphereCollider->pRigidBody->position));
					float sphereCastDistance = sphereCastDataClosestSphere.traceDistance;
					float adjustedDt = sphereTraceMin((dt - accumulatedDt) * sphereCastDistance / imposedNextLength + ST_AUTO_DT_FACTOR * dt, dt - accumulatedDt);

					//step the simulation
					sphereTraceSimulationStepQuantity(pSimulationSpace, &pSphereCollider->rigidBody, adjustedDt);

					ST_SphereContact contactInfo;
					if (sphereTraceColliderSphereSphereCollisionTest(pSphereCollider, pClosestSphere, &contactInfo))
					{
						sphereTraceSimulationSphereSphereResponse(pSimulationSpace, &contactInfo, adjustedDt);
					}
					accumulatedDt += adjustedDt;
				}
				else if (pClosestTerrain != NULL)
				{
					//float sphereCastDistance = sphereTraceVector3Length(sphereTraceVector3Subtract(sphereCastDataClosestTerrain.sphereCenter, pSphereCollider->pRigidBody->position));
					float sphereCastDistance = sphereCastDataClosestTerrain.traceDistance;
					float adjustedDt = sphereTraceMin((dt - accumulatedDt) * sphereCastDistance / imposedNextLength + ST_AUTO_DT_FACTOR * dt, dt - accumulatedDt);

					//step the simulation
					sphereTraceSimulationStepQuantity(pSimulationSpace, &pSphereCollider->rigidBody, adjustedDt);

					ST_SphereContact contactInfo;
					if (sphereTraceColliderUniformTerrainSphereFindMaxPenetratingTriangle(pClosestTerrain, pSphereCollider, &contactInfo))
					{
						//if (!overrideTerrainBehaviour)
						//	sphereTraceSimulationSphereTerrainTriangleResponse(pSimulationSpace, &contactInfo, adjustedDt);
						//else
						sphereTraceSimulationSphereTriangleResponse(pSimulationSpace, &contactInfo, adjustedDt);
					}
					accumulatedDt += adjustedDt;
				}
			}
			else
			{
				float adjustedDt = dt - accumulatedDt;
				sphereTraceSimulationStepQuantity(pSimulationSpace, &pSphereCollider->rigidBody, adjustedDt);
				accumulatedDt += adjustedDt;
			}
		}
		pSphereIndexData = pSphereIndexData->pNext;
	}
}


void sphereTraceSimulationSpherePairMultipleContactResponse(const ST_SimulationSpace* const pSimulationSpace, ST_SpherePair* pSpherePair, float dt)
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
		ST_Vector3 r = sphereTraceVector3Subtract(pContactInfo->point, pSpherePair->rigidBody.position);
		float vnMag = sphereTraceVector3Dot(pContactInfo->normal.v, pSpherePair->rigidBody.velocity);
		ST_Vector3 vn = sphereTraceVector3Scale(pContactInfo->normal.v, vnMag);
		ST_Vector3 vt = sphereTraceVector3Subtract(pSpherePair->rigidBody.velocity, vn);
		ST_Vector3 vprel = sphereTraceVector3Add(pSpherePair->rigidBody.velocity, sphereTraceVector3Cross(pSpherePair->rigidBody.angularVelocity, r));
		float j = -(1.0f + pSimulationSpace->defaultMaterial.restitution) * vnMag * (pSpherePair->rigidBody.mass / (float)numContacts);
		b32 restingContactCondition = (sphereTraceAbs(vnMag) < ST_VELOCITY_THRESHOLD && pContactInfo->normal.v.y > 0.0f);
		float accelNormal = sphereTraceAbs(vnMag);
		if (!restingContactCondition)
		{
			if (vnMag < 0.0f)
			{
				ST_Vector3 dp = sphereTraceVector3Scale(pContactInfo->normal.v, j);
				sphereTraceVector3ListAddFirst(&impulses, dp);
			}
			ST_Vector3 dl = sphereTraceVector3Cross(sphereTraceVector3Normalize(vprel), sphereTraceVector3Scale(pContactInfo->normal.v, -pSimulationSpace->defaultMaterial.kineticFriction * j));
			if (!sphereTraceVector3NanAny(dl))
				sphereTraceRigidBodyAddDeltaAngularMomentum(&pSpherePair->rigidBody, dl);

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
	}/*

	if (pRestingContactWithMaxForce)
	{
		ST_Vector3 r = sphereTraceVector3Subtract(pRestingContactWithMaxForce->point, pSpherePair->rigidBody.position);
		float vnMag = sphereTraceVector3Dot(pRestingContactWithMaxForce->normal.v, pSpherePair->rigidBody.velocity);
		ST_Vector3 vn = sphereTraceVector3Scale(pRestingContactWithMaxForce->normal.v, vnMag);
		ST_Vector3 vt = sphereTraceVector3Subtract(pSpherePair->rigidBody.velocity, vn);
		float j = -(1.0f + pSimulationSpace->defaultMaterial.restitution) * vnMag * pSpherePair->rigidBody.mass;
		float accelNormal = sphereTraceAbs(vnMag);
		pSpherePair->rigidBody.linearMomentum = sphereTraceVector3Scale(vt, pSpherePair->rigidBody.mass);
		ST_Vector3 actualVelocity = sphereTraceVector3Subtract(pSpherePair->rigidBody.position, pSpherePair->rigidBody.prevPosition);
		sphereTraceVector3ScaleByRef(&actualVelocity, 1.0f / dt);
		ST_Vector3 rollingWithoutSlipAngularVelocity = sphereTraceVector3Cross(pRestingContactWithMaxForce->normal.v, sphereTraceVector3Scale(vt, 1.0f / pSpherePair->radii));
		ST_Vector3 f = sphereTraceVector3Scale(sphereTraceVector3Normalize(pSpherePair->rigidBody.velocity),
			(-accelNormal + pSimulationSpace->gravitationalAcceleration.y) * pSpherePair->rigidBody.mass * pSimulationSpace->defaultMaterial.staticFriction * ST_FRICTION_MODIFIER);
		if (!sphereTraceVector3NanAny(f))
			sphereTraceRigidBodyAddForce(&pSpherePair->rigidBody, f);
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
		sphereTraceVector3AddByRef(&forceSum, modifiedForce);
		pvld = pvld->pNext;
	}
	sphereTraceRigidBodyAddDeltaMomentum(&pSpherePair->rigidBody, forceSum);*/
	sphereTraceVector3ListFree(&impulses);
	sphereTraceVector3ListFree(&restingContactNormals);
	sphereTraceVector3ListFree(&restingContactPoints);

}

void sphereTraceSimulationSpherePairSolver(ST_SimulationSpace* const pSimulationSpace, float dt)
{

	dt = sphereTraceMin(dt, pSimulationSpace->minDeltaTime);
	if (dt < 0.0f)
		dt = pSimulationSpace->minDeltaTime;

	//step all quantities
	sphereTraceSimulationStepQuantities(pSimulationSpace, dt);

	//update all sphere aabb's 
	ST_IndexListData* pSpherePairIndexData;
	ST_IndexListData* pOtherIndexData;
	pSpherePairIndexData = pSimulationSpace->spherePairColliders.pFirst;
	for (int sphereColliderIndex = 0; sphereColliderIndex < pSimulationSpace->spherePairColliders.count; sphereColliderIndex++)
	{
		ST_SpherePair* pSpherePairCollider = (ST_SpherePair*)pSpherePairIndexData->value;
		sphereTraceColliderSpherePairSetAABB(pSpherePairCollider);
		sphereTraceFrameUpdateWithRotationMatrix(&pSpherePairCollider->frame, pSpherePairCollider->rigidBody.rotationMatrix);
		pSpherePairIndexData = pSpherePairIndexData->pNext;
	}

	PenetrationRestriction penetrationRestriction;

	//ST_SphereContact contacts[ST_CONTACT_MAX];
	ST_Index contactsCount;

	////first do all sphere-sphere collisions
	//pSpherePairIndexData = pSimulationSpace->spherePairColliders.pFirst;
	//for (int sphereColliderIndex = 0; sphereColliderIndex < pSimulationSpace->spherePairColliders.count; sphereColliderIndex++)
	//{
	//	ST_SphereCollider* pSphereCollider = (ST_SphereCollider*)pSphereIndexData->value;

	//	//check for all sphere sphere collisions
	//	pOtherIndexData = pSimulationSpace->sphereColliders.pFirst;
	//	for (int sphereColliderBIndex = 0; sphereColliderBIndex < pSimulationSpace->sphereColliders.count; sphereColliderBIndex++)
	//	{
	//		ST_SphereCollider* pSphereColliderB = (ST_SphereCollider*)pOtherIndexData->value;

	//		if (pSphereCollider != pSphereColliderB)
	//		{
	//			ST_SphereContact contactInfo;
	//			if (sphereTraceColliderSphereSphereCollisionTest(pSphereCollider, pSphereColliderB, &contactInfo))
	//			{
	//				sphereTraceSimulationAddCurFrameContactEntry(&contactInfo);
	//				sphereTraceSimulationSphereSphereResponse(pSimulationSpace, &contactInfo, dt);
	//			}
	//		}
	//		pOtherIndexData = pOtherIndexData->pNext;
	//	}
	//	pSphereIndexData = pSphereIndexData->pNext;
	//}

	//check for all sphere plane collisions
	ST_SphereCollider ghostSphere;
	pSpherePairIndexData = pSimulationSpace->spherePairColliders.pFirst;
	for (int sphereColliderIndex = 0; sphereColliderIndex < pSimulationSpace->spherePairColliders.count; sphereColliderIndex++)
	{
		ST_SpherePair* pSpherePairCollider = (ST_SpherePair*)pSpherePairIndexData->value;
		ghostSphere.rigidBody = pSpherePairCollider->rigidBody;
		ghostSphere.radius = pSpherePairCollider->radii;
		ST_SphereContact contactInfo;
		//penetrationRestriction.penetrationRestrictionType = ST_PENETRATION_RESTRICTION_NONE;
		//penetrationRestriction.restrictionDirectionSet = 0;
		penetrationRestriction = sphereTracePenetrationRestrictionConstruct();
		contactsCount = 0;
		sphereTraceLinearAllocatorResetSphereContacts();
		ST_Vector3 spherePos1, spherePos2;
		sphereTraceColliderSpherePairGetSpherePositions(pSpherePairCollider, &spherePos1, &spherePos2);

		pOtherIndexData = pSimulationSpace->planeColliders.pFirst;
		for (int planeColliderIndex = 0; planeColliderIndex < pSimulationSpace->planeColliders.count; planeColliderIndex++)
		{
			ST_PlaneCollider* pPlaneCollider = (ST_PlaneCollider*)pOtherIndexData->value;
			if (sphereTraceColliderPlaneImposedSphereCollisionTest(pPlaneCollider, spherePos1, pSpherePairCollider->radii, &contactInfo))
			{
				ghostSphere.rigidBody.position = sphereTraceVector3Add(ghostSphere.rigidBody.position, sphereTraceVector3Scale(contactInfo.normal.v, contactInfo.penetrationDistance));
				ST_SphereContact* pAllocatedContact = sphereTraceLinearAllocatorAllocateSphereContact();
				*pAllocatedContact = contactInfo;
			}
			if (sphereTraceColliderPlaneImposedSphereCollisionTest(pPlaneCollider, spherePos2, pSpherePairCollider->radii, &contactInfo))
			{
				ghostSphere.rigidBody.position = sphereTraceVector3Add(ghostSphere.rigidBody.position, sphereTraceVector3Scale(contactInfo.normal.v, contactInfo.penetrationDistance));
				ST_SphereContact* pAllocatedContact = sphereTraceLinearAllocatorAllocateSphereContact();
				*pAllocatedContact = contactInfo;
			}
			pOtherIndexData = pOtherIndexData->pNext;
		}


		ST_Vector3 dp = sphereTraceVector3Subtract(ghostSphere.rigidBody.position, ghostSphere.rigidBody.prevPosition);
		float actualSpeed = sqrtf(sphereTraceVector3Dot(dp, dp)) / dt;
		float rbSpeed = sphereTraceRigidBodyGetSpeed(&ghostSphere.rigidBody);
		//if the actual position speed calculation is greater than the rigidbody speed
		//some sort of teleportation is taking place due to the penetration constraints
		//and we need to cap the teleportation
		if (actualSpeed > rbSpeed)
		{
			//printf("speed: %f, actual speed: %f\n", actualSpeed, rbSpeed);
			ST_Vector3 dir = sphereTraceVector3Normalize(dp);
			sphereTraceVector3AddAndScaleByRef(&ghostSphere.rigidBody.prevPosition, dir, rbSpeed * dt);
		}
		//printf("contacts count: %i\n", contactsCount);
		pSpherePairCollider->rigidBody = ghostSphere.rigidBody;
		sphereTraceSimulationSpherePairMultipleContactResponse(pSimulationSpace, pSpherePairCollider, dt);


		pSpherePairIndexData = pSpherePairIndexData->pNext;
	}

}