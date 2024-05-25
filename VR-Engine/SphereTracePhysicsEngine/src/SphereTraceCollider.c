#include "SphereTraceCollider.h"
#include "SphereTraceColliderPlane.h"
#include "SphereTraceGlobals.h"
#include "SphereTraceAllocator.h"

const ST_AABB gAABBOne = { {0.5f,0.5f,0.5f}, {0.0f,0.0f,0.0f}, {1.0f,1.0f,1.0f}, {0.5f,0.5f,0.5f} };

const ST_BoxFace gFaceRight = { ST_DIRECTION_RIGHT, {ST_RIGHT_DOWN_BACK, ST_RIGHT_DOWN_FORWARD, ST_RIGHT_UP_BACK, ST_RIGHT_UP_FORWARD}};
const ST_BoxFace gFaceLeft = { ST_DIRECTION_LEFT, {ST_LEFT_DOWN_BACK, ST_LEFT_DOWN_FORWARD, ST_LEFT_UP_BACK, ST_LEFT_UP_FORWARD}};
const ST_BoxFace gFaceUp = { ST_DIRECTION_UP, {ST_LEFT_UP_BACK, ST_RIGHT_UP_BACK, ST_LEFT_UP_FORWARD, ST_RIGHT_UP_FORWARD}};
const ST_BoxFace gFaceDown = { ST_DIRECTION_DOWN, {ST_LEFT_DOWN_BACK, ST_RIGHT_DOWN_BACK, ST_LEFT_DOWN_FORWARD, ST_RIGHT_DOWN_FORWARD}};
const ST_BoxFace gFaceForward = { ST_DIRECTION_FORWARD, {ST_LEFT_DOWN_FORWARD, ST_RIGHT_DOWN_FORWARD, ST_LEFT_UP_FORWARD, ST_RIGHT_UP_FORWARD}};
const ST_BoxFace gFaceBack = { ST_DIRECTION_BACK, {ST_LEFT_DOWN_BACK, ST_RIGHT_DOWN_BACK, ST_LEFT_UP_BACK, ST_RIGHT_UP_BACK}};

const ST_BoxEdgeConnection gEdgeConnections[8] = { 
	//LEFT_DOWN_BACK
	{{ST_DIRECTION_RIGHT, ST_DIRECTION_UP, ST_DIRECTION_FORWARD}, {ST_RIGHT_DOWN_BACK, ST_LEFT_UP_BACK, ST_LEFT_DOWN_FORWARD}},
	//RIGHT_DOWN_BACK
	{ {ST_DIRECTION_LEFT, ST_DIRECTION_UP, ST_DIRECTION_FORWARD}, {ST_LEFT_DOWN_BACK, ST_RIGHT_UP_BACK, ST_RIGHT_DOWN_FORWARD}},
	//LEFT_DOWN_FORWARD
	{ {ST_DIRECTION_RIGHT, ST_DIRECTION_UP, ST_DIRECTION_BACK}, {ST_RIGHT_DOWN_FORWARD, ST_LEFT_UP_FORWARD, ST_LEFT_DOWN_BACK} },
	//RIGHT_DOWN_FORWARD
	{ {ST_DIRECTION_LEFT, ST_DIRECTION_UP, ST_DIRECTION_BACK}, {ST_LEFT_DOWN_FORWARD, ST_RIGHT_UP_FORWARD, ST_RIGHT_DOWN_BACK} },
	//LEFT_UP_BACK
	{ {ST_DIRECTION_RIGHT, ST_DIRECTION_DOWN, ST_DIRECTION_FORWARD}, {ST_RIGHT_UP_BACK, ST_LEFT_DOWN_BACK, ST_LEFT_UP_FORWARD} },
	//RIGHT_UP_BACK
	{ {ST_DIRECTION_LEFT, ST_DIRECTION_DOWN, ST_DIRECTION_FORWARD}, {ST_LEFT_UP_BACK, ST_RIGHT_DOWN_BACK, ST_RIGHT_UP_FORWARD} },
	//LEFT_UP_FORWARD
	{ {ST_DIRECTION_RIGHT, ST_DIRECTION_DOWN, ST_DIRECTION_BACK}, {ST_RIGHT_UP_FORWARD, ST_LEFT_DOWN_FORWARD, ST_LEFT_UP_BACK} },
	//RIGHT_UP_FORWARD
	{ {ST_DIRECTION_LEFT, ST_DIRECTION_DOWN, ST_DIRECTION_BACK}, {ST_LEFT_UP_FORWARD, ST_RIGHT_DOWN_FORWARD, ST_RIGHT_UP_BACK} } };



const char* ST_ColliderStrings[] =
{
	"Sphere",
	"Plane",
	"Terrain",
	"Triangle",
	"Bowl",
	"Pipe"
};

ST_SubscriberList sphereTraceSubscriberListConstruct()
{
	ST_SubscriberList sl;
	sl.curFrameContactEntries = sphereTraceIndexListConstruct();
	sl.contactEntries = sphereTraceIndexListConstruct();
	sl.onCollisionStayCallbacks = sphereTraceIndexListConstruct();
	sl.onCollisionExitCallbacks = sphereTraceIndexListConstruct();
	sl.onCollisionEnterCallbacks = sphereTraceIndexListConstruct();
	sl.hasSubscriber = 0;
	return sl;
}

ST_Collider sphereTraceColliderConstruct(ST_ColliderType colliderType, float boundingRadius)
{
	ST_Collider collider;
	collider.colliderType = colliderType;
	if (colliderType == COLLIDER_SPHERE || colliderType == COLLIDER_BOX)
		collider.isDynamic = ST_TRUE;
	else
		collider.isDynamic = ST_FALSE;
	collider.bucketIndices = sphereTraceIndexListConstruct();
	collider.pLeafBucketLists = sphereTraceAllocatorAllocateIndexListArray();
	//default, will be set later when the derrived collider is created
	collider.colliderIndex = 0;
	collider.subscriberList = sphereTraceSubscriberListConstruct();
	collider.boundingRadius = boundingRadius;
	return collider;
}

ST_Collider sphereTraceColliderAABBConstruct(ST_AABB aabb)
{
	ST_Collider collider;
	collider.colliderType = COLLIDER_AABB;
	collider.isDynamic = ST_FALSE;
	collider.bucketIndices = sphereTraceIndexListConstruct();
	collider.pLeafBucketLists = sphereTraceAllocatorAllocateIndexListArray();
	//default, will be set later when the derrived collider is created
	collider.colliderIndex = 0;
	collider.subscriberList = sphereTraceSubscriberListConstruct();
	collider.boundingRadius = sphereTraceAABBGetBoundingRadius(&aabb);
	collider.aabb = aabb;
	return collider;
}

void sphereTraceColliderFree(ST_Collider* const pCollider)
{
	//ST_IndexListData* pild = pCollider->buc
	sphereTraceIndexListFree(&pCollider->bucketIndices);
	sphereTraceAllocatorFreeIndexListArray(pCollider->pLeafBucketLists);
}
//void sphereTraceColliderSetOctTreeEntry(ST_Collider* const pCollider)
//{
//	pCollider->octTreeEntry.leafNodes = sphereTraceIndexListConstruct();
//	pCollider->octTreeEntry.paabb = &pCollider->aabb;
//	pCollider->octTreeEntry.pObject = pCollider;
//	pCollider->octTreeEntry.objectIsCollider = ST_TRUE;
//}

ST_Frame sphereTraceFrameConstruct()
{
	ST_Frame frame;
	frame.right = gDirectionRight;
	frame.up = gDirectionUp;
	frame.forward = gDirectionForward;
	return frame;
}

void sphereTraceFrameUpdateWithRotationMatrix(ST_Frame* pFrame, ST_Matrix4 rotationMatrix)
{
	pFrame->right = sphereTraceDirectionGetLocalXAxisFromRotationMatrix(rotationMatrix);
	pFrame->up = sphereTraceDirectionGetLocalYAxisFromRotationMatrix(rotationMatrix);
	pFrame->forward = sphereTraceDirectionGetLocalZAxisFromRotationMatrix(rotationMatrix);
	sphereTraceDirectionNormalizeIfNotNormalizedByRef(&pFrame->right);
	sphereTraceDirectionNormalizeIfNotNormalizedByRef(&pFrame->up);
	sphereTraceDirectionNormalizeIfNotNormalizedByRef(&pFrame->up);
}

ST_Edge sphereTraceEdgeConstruct(ST_Vector3 p1, ST_Vector3 p2)
{
	ST_Edge edge;
	edge.point1 = p1;
	edge.point2 = p2;
	edge.dir = sphereTraceDirectionConstructNormalized(sphereTraceVector3Subtract(p2, p1));
	edge.dist = sphereTraceVector3Distance(p1, p2);
	return edge;
}

ST_Edge sphereTraceEdgeConstruct1(ST_Vector3 p1, ST_Vector3 p2, float dist, ST_Direction dir)
{
	ST_Edge edge;
	edge.point1 = p1;
	edge.point2 = p2;
	edge.dir = dir;
	edge.dist = dist;
	return edge;
}

ST_DynamicEdge sphereTraceDynamicEdgeConstruct(ST_Vector3* pp1, ST_Vector3* pp2, float dist, ST_Direction* pdir)
{
	ST_DynamicEdge edge;
	edge.pDir = pdir;
	edge.pPoint1 = pp1;
	edge.pPoint2 = pp2;
	edge.dist = dist;
	return edge;
}

ST_Edge sphereTraceEdgeFromDynamicEdge(const ST_DynamicEdge* const pde)
{
	ST_Edge edge;
	edge.dir = *pde->pDir;
	edge.dist = pde->dist;
	edge.point1 = *pde->pPoint1;
	edge.point2 = *pde->pPoint2;
	return edge;
}

//direction and distance stay the same
void sphereTraceEdgeTranslate(ST_Edge* pEdge, ST_Vector3 translation)
{
	sphereTraceVector3AddByRef(&pEdge->point1, translation);
	sphereTraceVector3AddByRef(&pEdge->point2, translation);
}

ST_Ring sphereTraceRingConstruct(ST_Vector3 centroid, ST_Direction normal, float radius)
{
	ST_Ring ring;
	ring.centroid = centroid;
	ring.normal = normal;
	float dot = sphereTraceVector3Dot(ring.normal.v, gVector3Up);
	if (sphereTraceEpsilonEqual(dot, 1.0f, ST_COLLIDER_TOLERANCE) || sphereTraceEpsilonEqual(dot, -1.0f, ST_COLLIDER_TOLERANCE))
	{
		ring.right = sphereTraceDirectionConstruct(gVector3Right, 1);
		ring.forward = sphereTraceDirectionConstruct(gVector3Forward, 1);
	}
	else
	{
		ring.right = sphereTraceDirectionConstructNormalized(sphereTraceVector3Cross(ring.normal.v, gVector3Up));
		ring.forward = sphereTraceDirectionConstructNormalized(sphereTraceVector3Cross(ring.normal.v, ring.right.v));
	}
	ring.radius = radius;
	return ring;
}

ST_AABB sphereTraceAABBConstruct1(ST_Vector3 lowExtent, ST_Vector3 highExtent)
{
	ST_AABB aabb;
	aabb.lowExtent = lowExtent;
	aabb.highExtent = highExtent;
	sphereTraceAABBSetCenterAndHalfExtents(&aabb);
	return aabb;
}

ST_AABB sphereTraceAABBConstruct2(ST_Vector3 position, ST_Vector3 halfExtents)
{
	ST_AABB aabb;
	aabb.center = position;
	aabb.halfExtents = halfExtents;
	sphereTraceAABBSetHighAndLowExtents(&aabb);
	return aabb;
}

void sphereTraceAABBTranslate(ST_AABB* paabb, ST_Vector3 translation)
{
	sphereTraceVector3AddByRef(&paabb->center, translation);
	sphereTraceVector3AddByRef(&paabb->highExtent, translation);
	sphereTraceVector3AddByRef(&paabb->lowExtent, translation);
}

b32 sphereTraceAABBAssert(const ST_AABB* const paabb)
{
	if (paabb->highExtent.x >+ paabb->lowExtent.x)
		if (paabb->highExtent.y >= paabb->lowExtent.y)
			if (paabb->highExtent.z >= paabb->lowExtent.z)
				return ST_TRUE;
	return ST_FALSE;
}

void sphereTraceAABBSetCenterAndHalfExtents(ST_AABB* paabb)
{
	paabb->center = sphereTraceVector3Average(paabb->lowExtent, paabb->highExtent);
	paabb->halfExtents = sphereTraceVector3Subtract(paabb->highExtent, paabb->center);
}

void sphereTraceAABBSetHighAndLowExtents(ST_AABB* paabb)
{
	paabb->highExtent = sphereTraceVector3Add(paabb->center, paabb->halfExtents);
	paabb->lowExtent = sphereTraceVector3Subtract(paabb->center, paabb->halfExtents);
}

float sphereTraceAABBGetVolume(const ST_AABB* const paabb)
{
	return 2.0f * paabb->halfExtents.x * 2.0f * paabb->halfExtents.y * 2.0f * paabb->halfExtents.z;
}

float sphereTraceAABBGetSizeMetric(const ST_AABB* const paabb)
{
	return (2.0f * paabb->halfExtents.x * 2.0f * paabb->halfExtents.y * 2.0f * paabb->halfExtents.z) +
		(2.0f * paabb->halfExtents.x * 2.0f * paabb->halfExtents.y) +
		(2.0f * paabb->halfExtents.y * 2.0f * paabb->halfExtents.z) +
		(2.0f * paabb->halfExtents.x * 2.0f * paabb->halfExtents.z) +
		(2.0f * paabb->halfExtents.x) +
		(2.0f * paabb->halfExtents.y) +
		(2.0f * paabb->halfExtents.z);
}

float sphereTraceAABBGetBoundingRadius(ST_AABB* paabb)
{
	return sqrtf(paabb->halfExtents.x * paabb->halfExtents.x + paabb->halfExtents.y * paabb->halfExtents.y + paabb->halfExtents.z * paabb->halfExtents.z);
}

float sphereTraceAABBGetBoundingRadiusSquared(ST_AABB* paabb)
{
	return paabb->halfExtents.x * paabb->halfExtents.x + paabb->halfExtents.y * paabb->halfExtents.y + paabb->halfExtents.z * paabb->halfExtents.z;
}

ST_ContactPoint sphereTraceContactPointFromSphereContact(const ST_SphereContact* const psc)
{
	ST_ContactPoint cp;
	cp.collisionType = psc->collisionType;
	cp.normal = psc->normal;
	cp.penetrationDistance = psc->penetrationDistance;
	cp.point = psc->point;
	return cp;
}


b32 sphereTraceColliderAABBContainsPoint(const ST_AABB* const aabb, ST_Vector3 point)
{
	if (point.x >= aabb->lowExtent.x && point.x <= aabb->highExtent.x)
	{
		if (point.y >= aabb->lowExtent.y && point.y <= aabb->highExtent.y)
		{
			if (point.z >= aabb->lowExtent.z && point.z <= aabb->highExtent.z)
			{
				return 1;
			}
		}
	}
	return 0;
}

void sphereTraceColliderAABBSetHalfExtents(ST_AABB* const aabb)
{
	aabb->halfExtents = sphereTraceVector3Subtract(aabb->highExtent, sphereTraceVector3Average(aabb->lowExtent, aabb->highExtent));
}

void sphereTraceColliderAABBResizeAABBToContainAnotherAABB(ST_AABB* const aabbToResize, const ST_AABB* const aabbToContain)
{
	b32 resizeDidHappen = 0;
	if (aabbToContain->lowExtent.x < aabbToResize->lowExtent.x)
	{
		aabbToResize->lowExtent.x = aabbToContain->lowExtent.x;
		resizeDidHappen = 1;
	}
	if (aabbToContain->lowExtent.y < aabbToResize->lowExtent.y)
	{
		aabbToResize->lowExtent.y = aabbToContain->lowExtent.y;
		resizeDidHappen = 1;
	}
	if (aabbToContain->lowExtent.z < aabbToResize->lowExtent.z)
	{
		aabbToResize->lowExtent.z = aabbToContain->lowExtent.z;
		resizeDidHappen = 1;
	}
	if (aabbToContain->highExtent.x > aabbToResize->highExtent.x)
	{
		aabbToResize->highExtent.x = aabbToContain->highExtent.x;
		resizeDidHappen = 1;
	}
	if (aabbToContain->highExtent.y > aabbToResize->highExtent.y)
	{
		aabbToResize->highExtent.y = aabbToContain->highExtent.y;
		resizeDidHappen = 1;
	}
	if (aabbToContain->highExtent.z > aabbToResize->highExtent.z)
	{
		aabbToResize->highExtent.z = aabbToContain->highExtent.z;
		resizeDidHappen = 1;
	}
	if (resizeDidHappen)
	{
		sphereTraceColliderAABBSetHalfExtents(aabbToResize);
		aabbToResize->center = sphereTraceVector3Average(aabbToResize->highExtent, aabbToResize->lowExtent);
	}
}

void sphereTraceColliderAABBResizeAABBToContainPoint(ST_AABB* const aabbToResize, const ST_Vector3 point)
{
	b32 resizeDidHappen = 0;
	if (point.x < aabbToResize->lowExtent.x)
	{
		aabbToResize->lowExtent.x = point.x;
		resizeDidHappen = 1;
	}
	if (point.y < aabbToResize->lowExtent.y)
	{
		aabbToResize->lowExtent.y = point.y;
		resizeDidHappen = 1;
	}
	if (point.z < aabbToResize->lowExtent.z)
	{
		aabbToResize->lowExtent.z = point.z;
		resizeDidHappen = 1;
	}
	if (point.x > aabbToResize->highExtent.x)
	{
		aabbToResize->highExtent.x = point.x;
		resizeDidHappen = 1;
	}
	if (point.y > aabbToResize->highExtent.y)
	{
		aabbToResize->highExtent.y = point.y;
		resizeDidHappen = 1;
	}
	if (point.z > aabbToResize->highExtent.z)
	{
		aabbToResize->highExtent.z = point.z;
		resizeDidHappen = 1;
	}
	if (resizeDidHappen)
	{
		sphereTraceColliderAABBSetHalfExtents(aabbToResize);
		aabbToResize->center = sphereTraceVector3Average(aabbToResize->highExtent, aabbToResize->lowExtent);
	}
}

b32 sphereTraceColliderAABBIntersectAABB(const ST_AABB* const aabb1, const ST_AABB* const aabb2)
{
	ST_Vector3 absdp = sphereTraceVector3SubtractAbsolute(aabb1->center, aabb2->center);
	if (absdp.x <= (aabb1->halfExtents.x + aabb2->halfExtents.x))
	{
		if (absdp.y <= (aabb1->halfExtents.y + aabb2->halfExtents.y))
		{
			if (absdp.z <= (aabb1->halfExtents.z + aabb2->halfExtents.z))
			{
				return 1;
			}
		}
	}
	return 0;
}

b32 sphereTraceColliderAABBIntersectAABBIntersectionRegion(const ST_AABB* const aabb1, const ST_AABB* const aabb2, ST_AABB* const intersectionRegion)
{
	intersectionRegion->lowExtent = sphereTraceVector3Construct(sphereTraceMax(aabb1->lowExtent.x, aabb2->lowExtent.x),
		sphereTraceMax(aabb1->lowExtent.y, aabb2->lowExtent.y),
		sphereTraceMax(aabb1->lowExtent.z, aabb2->lowExtent.z));

	intersectionRegion->highExtent = sphereTraceVector3Construct(sphereTraceMin(aabb1->highExtent.x, aabb2->highExtent.x),
		sphereTraceMin(aabb1->highExtent.y, aabb2->highExtent.y),
		sphereTraceMin(aabb1->highExtent.z, aabb2->highExtent.z));

	intersectionRegion->halfExtents = sphereTraceVector3Subtract(intersectionRegion->highExtent, intersectionRegion->lowExtent);
	if (intersectionRegion->halfExtents.x >= 0.0f && intersectionRegion->halfExtents.y >= 0.0f && intersectionRegion->halfExtents.z >= 0.0f)
	{
		sphereTraceVector3ScaleByRef(&intersectionRegion->halfExtents, 0.5f);
		intersectionRegion->center = sphereTraceVector3Add(intersectionRegion->lowExtent, intersectionRegion->halfExtents);
		return 1;
	}

	return 0;
}

b32 sphereTraceColliderAABBIntersectAABBHorizontally(const ST_AABB* const aabb1, const ST_AABB* const aabb2)
{
	if (sphereTraceAbs(aabb1->center.x - aabb2->center.x) <= (aabb1->halfExtents.x + aabb2->halfExtents.x))
	{
		if (sphereTraceAbs(aabb1->center.z - aabb2->center.z) <= (aabb1->halfExtents.z + aabb2->halfExtents.z))
		{
			return 1;
		}
	}
	return 0;
	//if ((aabb1->highExtent.x >= aabb2->highExtent.x && aabb1->lowExtent.x <= aabb2->highExtent.x)
	//	|| (aabb1->highExtent.x >= aabb2->lowExtent.x && aabb1->lowExtent.x <= aabb2->lowExtent.x)
	//	|| (aabb2->highExtent.x >= aabb1->highExtent.x && aabb2->lowExtent.x <= aabb1->highExtent.x)
	//	|| (aabb2->highExtent.x >= aabb1->lowExtent.x && aabb2->lowExtent.x <= aabb1->lowExtent.x))
	//{

	//	if ((aabb1->highExtent.z >= aabb2->highExtent.z && aabb1->lowExtent.z <= aabb2->highExtent.z)
	//		|| (aabb1->highExtent.z >= aabb2->lowExtent.z && aabb1->lowExtent.z <= aabb2->lowExtent.z)
	//		|| (aabb2->highExtent.z >= aabb1->highExtent.z && aabb2->lowExtent.z <= aabb1->highExtent.z)
	//		|| (aabb2->highExtent.z >= aabb1->lowExtent.z && aabb2->lowExtent.z <= aabb1->lowExtent.z))
	//	{
	//		return 1;
	//	}
	//}

	//return 0;
}

b32 sphereTraceColliderAABBIntersectAABBVertically(const ST_AABB* const aabb1, const ST_AABB* const aabb2)
{
	if (sphereTraceAbs(aabb1->center.y - aabb2->center.y) <= (aabb1->halfExtents.y + aabb2->halfExtents.y))
	{
		return 1;
	}
	return 0;
	//if ((aabb1->highExtent.y >= aabb2->highExtent.y && aabb1->lowExtent.y <= aabb2->highExtent.y)
	//	|| (aabb1->highExtent.y >= aabb2->lowExtent.y && aabb1->lowExtent.y <= aabb2->lowExtent.y)
	//	|| (aabb2->highExtent.y >= aabb1->highExtent.y && aabb2->lowExtent.y <= aabb1->highExtent.y)
	//	|| (aabb2->highExtent.y >= aabb1->lowExtent.y && aabb2->lowExtent.y <= aabb1->lowExtent.y))
	//{
	//	return 1;
	//}

	//return 0;
}

b32 sphereTraceColliderEmptyAABBRayTrace(ST_Vector3 from, ST_Direction dir, const ST_AABB* const paabb, ST_RayTraceData* const pRaycastData)
{
	ST_Vector3 dp = sphereTraceVector3Subtract(paabb->center, from);
	if (sphereTraceAbs(dp.x) <= paabb->halfExtents.x)
	{
		if (sphereTraceAbs(dp.y) <= paabb->halfExtents.y)
		{
			if (sphereTraceAbs(dp.z) <= paabb->halfExtents.z)
			{
				pRaycastData->distance = 0.0f;
				pRaycastData->startPoint = from;
				pRaycastData->contact.point = from;
				return 1;

			}
		}
	}
	ST_Vector3 dpDir = sphereTraceVector3Normalize(dp);
	float dot = sphereTraceVector3Dot(dpDir, dir.v);
	if (dot >= 0.0f)
	{
		if (dir.v.x>=0.0f)
		{
			//check the left face
			sphereTraceColliderInfiniteXPlaneRayTrace(from, dir, paabb->lowExtent, pRaycastData);
			if ((pRaycastData->contact.point.y >= paabb->lowExtent.y) && (pRaycastData->contact.point.z >= paabb->lowExtent.z))
			{
				if ((pRaycastData->contact.point.y <= paabb->highExtent.y) && (pRaycastData->contact.point.z <= paabb->highExtent.z))
				{
					return 1;
				}
			}
		}
		else
		{
			//check the right face
			sphereTraceColliderInfiniteXPlaneRayTrace(from, dir, paabb->highExtent, pRaycastData);
			if ((pRaycastData->contact.point.y >= paabb->lowExtent.y) && (pRaycastData->contact.point.z >= paabb->lowExtent.z))
			{
				if ((pRaycastData->contact.point.y <= paabb->highExtent.y) && (pRaycastData->contact.point.z <= paabb->highExtent.z))
				{
					return 1;
				}
			}
		}

		if (dir.v.y>=0.0f)
		{
			//check the left face
			sphereTraceColliderInfiniteYPlaneRayTrace(from, dir, paabb->lowExtent, pRaycastData);
			if ((pRaycastData->contact.point.x >= paabb->lowExtent.x) && (pRaycastData->contact.point.z >= paabb->lowExtent.z))
			{
				if ((pRaycastData->contact.point.x <= paabb->highExtent.x) && (pRaycastData->contact.point.z <= paabb->highExtent.z))
				{
					return 1;
				}
			}
		}
		else
		{
			//check the right face
			sphereTraceColliderInfiniteYPlaneRayTrace(from, dir, paabb->highExtent, pRaycastData);
			if ((pRaycastData->contact.point.x >= paabb->lowExtent.x) && (pRaycastData->contact.point.z >= paabb->lowExtent.z))
			{
				if ((pRaycastData->contact.point.x <= paabb->highExtent.x) && (pRaycastData->contact.point.z <= paabb->highExtent.z))
				{
					return 1;
				}
			}
		}

		if (dir.v.z>=0.0f)
		{
			//check the left face
			sphereTraceColliderInfiniteZPlaneRayTrace(from, dir, paabb->lowExtent, pRaycastData);
			if ((pRaycastData->contact.point.y >= paabb->lowExtent.y) && (pRaycastData->contact.point.x >= paabb->lowExtent.x))
			{
				if ((pRaycastData->contact.point.y <= paabb->highExtent.y) && (pRaycastData->contact.point.x <= paabb->highExtent.x))
				{
					return 1;
				}
			}
		}
		else
		{
			//check the right face
			sphereTraceColliderInfiniteZPlaneRayTrace(from, dir, paabb->highExtent, pRaycastData);
			if ((pRaycastData->contact.point.y >= paabb->lowExtent.y) && (pRaycastData->contact.point.x >= paabb->lowExtent.x))
			{
				if ((pRaycastData->contact.point.y <= paabb->highExtent.y) && (pRaycastData->contact.point.x <= paabb->highExtent.x))
				{
					return 1;
				}
			}
		}
	}

	return 0;
}

b32 sphereTraceColliderAABBRayTrace(ST_Vector3 from, ST_Direction dir, const ST_Collider* const pCollider, ST_RayTraceData* const pRaycastData)
{
	if (sphereTraceColliderEmptyAABBRayTrace(from, dir, &pCollider->aabb, pRaycastData))
	{
		pRaycastData->pOtherCollider = pCollider;
		return ST_TRUE;
	}
	return ST_FALSE;
}

ST_DirectionType sphereTraceDirectionTypeGetReverse(ST_DirectionType dirType)
{
	switch (dirType)
	{
	case ST_DIRECTION_RIGHT:
		return ST_DIRECTION_LEFT;
		break;
	case ST_DIRECTION_LEFT:
		return ST_DIRECTION_RIGHT;
		break;
	case ST_DIRECTION_UP:
		return ST_DIRECTION_DOWN;
		break;
	case ST_DIRECTION_DOWN:
		return ST_DIRECTION_UP;
		break;
	case ST_DIRECTION_FORWARD:
		return ST_DIRECTION_BACK;
		break;
	case ST_DIRECTION_BACK:
		return ST_DIRECTION_FORWARD;
		break;
	}
}

ST_DirectionType sphereTraceDirectionTypeCross(ST_DirectionType dir1, ST_DirectionType dir2)
{
	switch (dir1)
	{
	case ST_DIRECTION_RIGHT:
	{
		switch (dir2)
		{
		case ST_DIRECTION_RIGHT:
		{
			return ST_DIRECTION_NONE;
		}
		break;
		case ST_DIRECTION_LEFT:
		{
			return ST_DIRECTION_NONE;
		}
		break;
		case ST_DIRECTION_UP:
		{
			return ST_DIRECTION_FORWARD;
		}
		break;
		case ST_DIRECTION_DOWN:
		{
			return ST_DIRECTION_BACK;
		}
		break;
		case ST_DIRECTION_FORWARD:
		{
			return ST_DIRECTION_DOWN;
		}
		break;
		case ST_DIRECTION_BACK:
		{
			return ST_DIRECTION_UP;
		}
		break;
		}
	}
	break;
	case ST_DIRECTION_LEFT:
	{
		switch (dir2)
		{
		case ST_DIRECTION_RIGHT:
		{
			return ST_DIRECTION_NONE;
		}
		break;
		case ST_DIRECTION_LEFT:
		{
			return ST_DIRECTION_NONE;
		}
		break;
		case ST_DIRECTION_UP:
		{
			return ST_DIRECTION_BACK;
		}
		break;
		case ST_DIRECTION_DOWN:
		{
			return ST_DIRECTION_FORWARD;
		}
		break;
		case ST_DIRECTION_FORWARD:
		{
			return ST_DIRECTION_UP;
		}
		break;
		case ST_DIRECTION_BACK:
		{
			return ST_DIRECTION_DOWN;
		}
		break;
		}
	}
	break;
	case ST_DIRECTION_UP:
	{
		switch (dir2)
		{
		case ST_DIRECTION_RIGHT:
		{
			return ST_DIRECTION_BACK;
		}
		break;
		case ST_DIRECTION_LEFT:
		{
			return ST_DIRECTION_FORWARD;
		}
		break;
		case ST_DIRECTION_UP:
		{
			return ST_DIRECTION_NONE;
		}
		break;
		case ST_DIRECTION_DOWN:
		{
			return ST_DIRECTION_NONE;
		}
		break;
		case ST_DIRECTION_FORWARD:
		{
			return ST_DIRECTION_RIGHT;
		}
		break;
		case ST_DIRECTION_BACK:
		{
			return ST_DIRECTION_LEFT;
		}
		break;
		}
	}
	break;
	case ST_DIRECTION_DOWN:
	{
		switch (dir2)
		{
		case ST_DIRECTION_RIGHT:
		{
			return ST_DIRECTION_FORWARD;
		}
		break;
		case ST_DIRECTION_LEFT:
		{
			return ST_DIRECTION_BACK;
		}
		break;
		case ST_DIRECTION_UP:
		{
			return ST_DIRECTION_NONE;
		}
		break;
		case ST_DIRECTION_DOWN:
		{
			return ST_DIRECTION_NONE;
		}
		break;
		case ST_DIRECTION_FORWARD:
		{
			return ST_DIRECTION_LEFT;
		}
		break;
		case ST_DIRECTION_BACK:
		{
			return ST_DIRECTION_RIGHT;
		}
		break;
		}
	}
	break;
	case ST_DIRECTION_FORWARD:
	{
		switch (dir2)
		{
		case ST_DIRECTION_RIGHT:
		{
			return ST_DIRECTION_UP;
		}
		break;
		case ST_DIRECTION_LEFT:
		{
			return ST_DIRECTION_DOWN;
		}
		break;
		case ST_DIRECTION_UP:
		{
			return ST_DIRECTION_LEFT;
		}
		break;
		case ST_DIRECTION_DOWN:
		{
			return ST_DIRECTION_RIGHT;
		}
		break;
		case ST_DIRECTION_FORWARD:
		{
			return ST_DIRECTION_NONE;
		}
		break;
		case ST_DIRECTION_BACK:
		{
			return ST_DIRECTION_NONE;
		}
		break;
		}
	}
	break;
	case ST_DIRECTION_BACK:
	{
		switch (dir2)
		{
		case ST_DIRECTION_RIGHT:
		{
			return ST_DIRECTION_DOWN;
		}
		break;
		case ST_DIRECTION_LEFT:
		{
			return ST_DIRECTION_UP;
		}
		break;
		case ST_DIRECTION_UP:
		{
			return ST_DIRECTION_RIGHT;
		}
		break;
		case ST_DIRECTION_DOWN:
		{
			return ST_DIRECTION_LEFT;
		}
		break;
		case ST_DIRECTION_FORWARD:
		{
			return ST_DIRECTION_NONE;
		}
		break;
		case ST_DIRECTION_BACK:
		{
			return ST_DIRECTION_NONE;
		}
		break;
		}
	}
	break;
	}
}
//
//b32 sphereTraceColliderAABBRayTraceThrough(ST_Vector3 contactStart, ST_DirectionType contactDirection, ST_Direction dir, const ST_AABB* const paabb, ST_RayTraceData* const pRaycastData)
//{
//	float tMin;
//	float tentativeT;
//	ST_DirectionType tMinDir;
//	switch (contactDirection)
//	{
//	case ST_DIRECTION_RIGHT:
//		if (dir.v.x >= 0.0f)
//		{
//			return 0;
//		}
//		else
//		{
//			tMin = (2.0f * paabb->halfExtents.x) / -dir.v.x;
//			tMinDir = ST_DIRECTION_LEFT;
//			pRaycastData->contact.normal = gDirectionLeft;
//			if (dir.v.y > 0.0f)
//			{
//				tentativeT = (paabb->highExtent.y - contactStart.y) / dir.v.y;
//				if (tentativeT < tMin)
//				{
//					tMin = tentativeT;
//					tMinDir = ST_DIRECTION_UP;
//					pRaycastData->contact.normal = gDirectionUp;
//				}
//			} 
//			else if (dir.v.y < 0.0f)
//			{
//				tentativeT = (paabb->lowExtent.y - contactStart.y) / dir.v.y;
//				if (tentativeT < tMin)
//				{
//					tMin = tentativeT;
//					tMinDir = ST_DIRECTION_DOWN;
//					pRaycastData->contact.normal = gDirectionDown;
//				}
//			}
//			if (dir.v.z > 0.0f)
//			{
//				tentativeT = (paabb->highExtent.z - contactStart.z) / dir.v.z;
//				if (tentativeT < tMin)
//				{
//					tMin = tentativeT;
//					tMinDir = ST_DIRECTION_FORWARD;
//					pRaycastData->contact.normal = gDirectionForward;
//				}
//			}
//			else if (dir.v.z < 0.0f)
//			{
//				tentativeT = (paabb->lowExtent.z - contactStart.z) / dir.v.z;
//				if (tentativeT < tMin)
//				{
//					tMin = tentativeT;
//					tMinDir = ST_DIRECTION_BACK;
//					pRaycastData->contact.normal = gDirectionBack;
//				}
//			}		
//		}
//		pRaycastData->directionType = tMinDir;
//		pRaycastData->distance = tMin;
//		pRaycastData->startPoint = contactStart;
//		pRaycastData->contact.point = sphereTraceVector3AddAndScale(contactStart, dir.v, tMin);
//		break;
//	case ST_DIRECTION_LEFT:
//		if (dir.v.x <= 0.0f)
//		{
//			return 0;
//		}
//		else
//		{
//			tMin = (2.0f * paabb->halfExtents.x) / dir.v.x;
//			tMinDir = ST_DIRECTION_RIGHT;
//			pRaycastData->contact.normal = gDirectionRight;
//			if (dir.v.y > 0.0f)
//			{
//				tentativeT = (paabb->highExtent.y - contactStart.y) / dir.v.y;
//				if (tentativeT < tMin)
//				{
//					tMin = tentativeT;
//					tMinDir = ST_DIRECTION_UP;
//					pRaycastData->contact.normal = gDirectionUp;
//				}
//			}
//			else if (dir.v.y < 0.0f)
//			{
//				tentativeT = (paabb->lowExtent.y - contactStart.y) / dir.v.y;
//				if (tentativeT < tMin)
//				{
//					tMin = tentativeT;
//					tMinDir = ST_DIRECTION_DOWN;
//					pRaycastData->contact.normal = gDirectionDown;
//				}
//			}
//			if (dir.v.z > 0.0f)
//			{
//				tentativeT = (paabb->highExtent.z - contactStart.z) / dir.v.z;
//				if (tentativeT < tMin)
//				{
//					tMin = tentativeT;
//					tMinDir = ST_DIRECTION_FORWARD;
//					pRaycastData->contact.normal = gDirectionForward;
//				}
//			}
//			else if (dir.v.z < 0.0f)
//			{
//				tentativeT = (paabb->lowExtent.z - contactStart.z) / dir.v.z;
//				if (tentativeT < tMin)
//				{
//					tMin = tentativeT;
//					tMinDir = ST_DIRECTION_BACK;
//					pRaycastData->contact.normal = gDirectionBack;
//				}
//			}
//		}
//		pRaycastData->directionType = tMinDir;
//		pRaycastData->distance = tMin;
//		pRaycastData->startPoint = contactStart;
//		pRaycastData->contact.point = sphereTraceVector3AddAndScale(contactStart, dir.v, tMin);
//		break;
//	case ST_DIRECTION_UP:
//		if (dir.v.y >= 0.0f)
//		{
//			return 0;
//		}
//		else
//		{
//			tMin = (2.0f * paabb->halfExtents.y) / -dir.v.y;
//			tMinDir = ST_DIRECTION_DOWN;
//			pRaycastData->contact.normal = gDirectionDown;
//			if (dir.v.x > 0.0f)
//			{
//				tentativeT = (paabb->highExtent.x - contactStart.x) / dir.v.x;
//				if (tentativeT < tMin)
//				{
//					tMin = tentativeT;
//					tMinDir = ST_DIRECTION_RIGHT;
//					pRaycastData->contact.normal = gDirectionRight;
//				}
//			}
//			else if (dir.v.x < 0.0f)
//			{
//				tentativeT = (paabb->lowExtent.x - contactStart.x) / dir.v.x;
//				if (tentativeT < tMin)
//				{
//					tMin = tentativeT;
//					tMinDir = ST_DIRECTION_LEFT;
//					pRaycastData->contact.normal = gDirectionLeft;
//				}
//			}
//			if (dir.v.z > 0.0f)
//			{
//				tentativeT = (paabb->highExtent.z - contactStart.z) / dir.v.z;
//				if (tentativeT < tMin)
//				{
//					tMin = tentativeT;
//					tMinDir = ST_DIRECTION_FORWARD;
//					pRaycastData->contact.normal = gDirectionForward;
//				}
//			}
//			else if (dir.v.z < 0.0f)
//			{
//				tentativeT = (paabb->lowExtent.z - contactStart.z) / dir.v.z;
//				if (tentativeT < tMin)
//				{
//					tMin = tentativeT;
//					tMinDir = ST_DIRECTION_BACK;
//					pRaycastData->contact.normal = gDirectionBack;
//				}
//			}
//		}
//		pRaycastData->directionType = tMinDir;
//		pRaycastData->distance = tMin;
//		pRaycastData->startPoint = contactStart;
//		pRaycastData->contact.point = sphereTraceVector3AddAndScale(contactStart, dir.v, tMin);
//		break;
//	case ST_DIRECTION_DOWN:
//		if (dir.v.y <= 0.0f)
//		{
//			return 0;
//		}
//		else
//		{
//			tMin = (2.0f * paabb->halfExtents.y) / dir.v.y;
//			tMinDir = ST_DIRECTION_UP;
//			pRaycastData->contact.normal = gDirectionUp;
//			if (dir.v.x > 0.0f)
//			{
//				tentativeT = (paabb->highExtent.x - contactStart.x) / dir.v.x;
//				if (tentativeT < tMin)
//				{
//					tMin = tentativeT;
//					tMinDir = ST_DIRECTION_RIGHT;
//					pRaycastData->contact.normal = gDirectionRight;
//				}
//			}
//			else if (dir.v.x < 0.0f)
//			{
//				tentativeT = (paabb->lowExtent.x - contactStart.x) / dir.v.x;
//				if (tentativeT < tMin)
//				{
//					tMin = tentativeT;
//					tMinDir = ST_DIRECTION_LEFT;
//					pRaycastData->contact.normal = gDirectionLeft;
//				}
//			}
//			if (dir.v.z > 0.0f)
//			{
//				tentativeT = (paabb->highExtent.z - contactStart.z) / dir.v.z;
//				if (tentativeT < tMin)
//				{
//					tMin = tentativeT;
//					tMinDir = ST_DIRECTION_FORWARD;
//					pRaycastData->contact.normal = gDirectionForward;
//				}
//			}
//			else if (dir.v.z < 0.0f)
//			{
//				tentativeT = (paabb->lowExtent.z - contactStart.z) / dir.v.z;
//				if (tentativeT < tMin)
//				{
//					tMin = tentativeT;
//					tMinDir = ST_DIRECTION_BACK;
//					pRaycastData->contact.normal = gDirectionBack;
//				}
//			}
//		}
//		pRaycastData->directionType = tMinDir;
//		pRaycastData->distance = tMin;
//		pRaycastData->startPoint = contactStart;
//		pRaycastData->contact.point = sphereTraceVector3AddAndScale(contactStart, dir.v, tMin);
//		break;
//	case ST_DIRECTION_FORWARD:
//		if (dir.v.z >= 0.0f)
//		{
//			return 0;
//		}
//		else
//		{
//			tMin = (2.0f * paabb->halfExtents.z) / -dir.v.z;
//			tMinDir = ST_DIRECTION_BACK;
//			pRaycastData->contact.normal = gDirectionBack;
//			if (dir.v.x > 0.0f)
//			{
//				tentativeT = (paabb->highExtent.x - contactStart.x) / dir.v.x;
//				if (tentativeT < tMin)
//				{
//					tMin = tentativeT;
//					tMinDir = ST_DIRECTION_RIGHT;
//					pRaycastData->contact.normal = gDirectionRight;
//				}
//			}
//			else if (dir.v.x < 0.0f)
//			{
//				tentativeT = (paabb->lowExtent.x - contactStart.x) / dir.v.x;
//				if (tentativeT < tMin)
//				{
//					tMin = tentativeT;
//					tMinDir = ST_DIRECTION_LEFT;
//					pRaycastData->contact.normal = gDirectionLeft;
//				}
//			}
//			if (dir.v.y > 0.0f)
//			{
//				tentativeT = (paabb->highExtent.y - contactStart.y) / dir.v.y;
//				if (tentativeT < tMin)
//				{
//					tMin = tentativeT;
//					tMinDir = ST_DIRECTION_RIGHT;
//					pRaycastData->contact.normal = gDirectionRight;
//				}
//			}
//			else if (dir.v.y < 0.0f)
//			{
//				tentativeT = (paabb->lowExtent.y - contactStart.y) / dir.v.y;
//				if (tentativeT < tMin)
//				{
//					tMin = tentativeT;
//					tMinDir = ST_DIRECTION_LEFT;
//					pRaycastData->contact.normal = gDirectionLeft;
//				}
//			}
//		}
//		pRaycastData->directionType = tMinDir;
//		pRaycastData->distance = tMin;
//		pRaycastData->startPoint = contactStart;
//		pRaycastData->contact.point = sphereTraceVector3AddAndScale(contactStart, dir.v, tMin);
//		break;
//	case ST_DIRECTION_BACK:
//		if (dir.v.z <= 0.0f)
//		{
//			return 0;
//		}
//		else
//		{
//			tMin = (2.0f * paabb->halfExtents.z) / dir.v.z;
//			tMinDir = ST_DIRECTION_FORWARD;
//			pRaycastData->contact.normal = gDirectionForward;
//			if (dir.v.x > 0.0f)
//			{
//				tentativeT = (paabb->highExtent.x - contactStart.x) / dir.v.x;
//				if (tentativeT < tMin)
//				{
//					tMin = tentativeT;
//					tMinDir = ST_DIRECTION_RIGHT;
//					pRaycastData->contact.normal = gDirectionRight;
//				}
//			}
//			else if (dir.v.x < 0.0f)
//			{
//				tentativeT = (paabb->lowExtent.x - contactStart.x) / dir.v.x;
//				if (tentativeT < tMin)
//				{
//					tMin = tentativeT;
//					tMinDir = ST_DIRECTION_LEFT;
//					pRaycastData->contact.normal = gDirectionLeft;
//				}
//			}
//			if (dir.v.y > 0.0f)
//			{
//				tentativeT = (paabb->highExtent.y - contactStart.y) / dir.v.y;
//				if (tentativeT < tMin)
//				{
//					tMin = tentativeT;
//					tMinDir = ST_DIRECTION_RIGHT;
//					pRaycastData->contact.normal = gDirectionRight;
//				}
//			}
//			else if (dir.v.y < 0.0f)
//			{
//				tentativeT = (paabb->lowExtent.y - contactStart.y) / dir.v.y;
//				if (tentativeT < tMin)
//				{
//					tMin = tentativeT;
//					tMinDir = ST_DIRECTION_LEFT;
//					pRaycastData->contact.normal = gDirectionLeft;
//				}
//			}
//		}
//		pRaycastData->directionType = tMinDir;
//		pRaycastData->distance = tMin;
//		pRaycastData->startPoint = contactStart;
//		pRaycastData->contact.point = sphereTraceVector3AddAndScale(contactStart, dir.v, tMin);
//		break;
//	}
//	return 1;
//}

b32 sphereTraceColliderAABBRayTraceThrough(ST_Vector3 pointWithin, ST_Direction dir, const ST_AABB* const paabb, ST_RayTraceData* const pRaycastData)
{
	float t = FLT_MAX;
	float tentativeT = FLT_MAX;
	pRaycastData->startPoint = pointWithin;
	if (dir.v.x > 0.0f)
	{
		t = (paabb->highExtent.x - pointWithin.x) / dir.v.x;
		//pRaycastData->contact.point = sphereTraceVector3AddAndScale(pointWithin, dir.v, t);
		pRaycastData->directionType = ST_DIRECTION_RIGHT;
		//if (pRaycastData->contact.point.y >= paabb->lowExtent.y && pRaycastData->contact.point.y <= paabb->highExtent.y)
		//{
		//	if (pRaycastData->contact.point.z >= paabb->lowExtent.z && pRaycastData->contact.point.z <= paabb->highExtent.z)
		//	{
		//		pRaycastData->distance = t;
		//		return ST_TRUE;
		//	}
		//}
	}
	else if (dir.v.x < 0.0f)
	{
		t = (paabb->lowExtent.x - pointWithin.x) / dir.v.x;
		//pRaycastData->contact.point = sphereTraceVector3AddAndScale(pointWithin, dir.v, t);
		pRaycastData->directionType = ST_DIRECTION_LEFT;
		//if (pRaycastData->contact.point.y >= paabb->lowExtent.y && pRaycastData->contact.point.y <= paabb->highExtent.y)
		//{
		//	if (pRaycastData->contact.point.z >= paabb->lowExtent.z && pRaycastData->contact.point.z <= paabb->highExtent.z)
		//	{
		//		pRaycastData->distance = t;
		//		return ST_TRUE;
		//	}
		//}
	}
	if (dir.v.y > 0.0f)
	{
		tentativeT = (paabb->highExtent.y - pointWithin.y) / dir.v.y;
		//pRaycastData->contact.point = sphereTraceVector3AddAndScale(pointWithin, dir.v, t);
		if (tentativeT < t)
		{
			pRaycastData->directionType = ST_DIRECTION_UP;
			t = tentativeT;
		}
		//if (pRaycastData->contact.point.x >= paabb->lowExtent.x && pRaycastData->contact.point.x <= paabb->highExtent.x)
		//{
		//	if (pRaycastData->contact.point.z >= paabb->lowExtent.z && pRaycastData->contact.point.z <= paabb->highExtent.z)
		//	{
		//		pRaycastData->distance = t;
		//		return ST_TRUE;
		//	}
		//}
	}
	else if (dir.v.y < 0.0f)
	{
		tentativeT = (paabb->lowExtent.y - pointWithin.y) / dir.v.y;
		if (tentativeT < t)
		{
			pRaycastData->directionType = ST_DIRECTION_DOWN;
			t = tentativeT;
		}
		//pRaycastData->contact.point = sphereTraceVector3AddAndScale(pointWithin, dir.v, t);
		//if (pRaycastData->contact.point.x >= paabb->lowExtent.x && pRaycastData->contact.point.x <= paabb->highExtent.x)
		//{
		//	if (pRaycastData->contact.point.z >= paabb->lowExtent.z && pRaycastData->contact.point.z <= paabb->highExtent.z)
		//	{
		//		pRaycastData->distance = t;
		//		return ST_TRUE;
		//	}
		//}
	}
	if (dir.v.z > 0.0f)
	{
		tentativeT = (paabb->highExtent.z - pointWithin.z) / dir.v.z;
		if (tentativeT < t)
		{
			pRaycastData->directionType = ST_DIRECTION_FORWARD;
			t = tentativeT;
		}
		//pRaycastData->contact.point = sphereTraceVector3AddAndScale(pointWithin, dir.v, t);
		//if (pRaycastData->contact.point.x >= paabb->lowExtent.x && pRaycastData->contact.point.x <= paabb->highExtent.x)
		//{
		//	if (pRaycastData->contact.point.y >= paabb->lowExtent.y && pRaycastData->contact.point.y <= paabb->highExtent.y)
		//	{
		//		pRaycastData->distance = t;
		//		return ST_TRUE;
		//	}
		//}
	}
	else if (dir.v.z < 0.0f)
	{
		tentativeT = (paabb->lowExtent.z - pointWithin.z) / dir.v.z;
		if (tentativeT < t)
		{
			pRaycastData->directionType = ST_DIRECTION_BACK;
			t = tentativeT;
		}
		//pRaycastData->contact.point = sphereTraceVector3AddAndScale(pointWithin, dir.v, t);
		//if (pRaycastData->contact.point.x >= paabb->lowExtent.x && pRaycastData->contact.point.x <= paabb->highExtent.x)
		//{
		//	if (pRaycastData->contact.point.y >= paabb->lowExtent.y && pRaycastData->contact.point.y <= paabb->highExtent.y)
		//	{
		//		pRaycastData->directionType = ST_DIRECTION_BACK;
		//		pRaycastData->distance = t;
		//		return ST_TRUE;
		//	}
		//}
	}
	if (t < FLT_MAX)
	{
		pRaycastData->contact.point = sphereTraceVector3AddAndScale(pointWithin, dir.v, t);
		pRaycastData->distance = t;
		//test if the point is within bounds
		ST_Vector3 absdp = sphereTraceVector3SubtractAbsolute(pRaycastData->contact.point, paabb->center);
		if (absdp.x <= (paabb->halfExtents.x + 0.0005 ))
		{
			if (absdp.y <= (paabb->halfExtents.y + 0.0005))
			{
				if (absdp.z <= (paabb->halfExtents.z + 0.0005))
				{
					return ST_TRUE;
				}
			}
		}

		return ST_FALSE;
		
	}
}

b32 sphereTraceColliderAABBFrontFaceSphereTrace(ST_Vector3 from, ST_Direction dir, float radius, const ST_AABB* const paabb, ST_SphereTraceData* const pData)
{
	if (sphereTraceColliderInfiniteZPlaneSphereTrace(from, dir, radius, paabb->highExtent, pData))
	{
		ST_Vector3 contactPoint = pData->rayTraceData.contact.point;
		ST_Vector3 dp = sphereTraceVector3Subtract(contactPoint, paabb->center);
		b32 coordInYBounds = sphereTraceAbs(dp.y) <= paabb->halfExtents.y;
		b32 coordInXBounds = sphereTraceAbs(dp.x) <= paabb->halfExtents.x;
		if (coordInYBounds && coordInXBounds)
		{
			return 1;
		}
		else 
		{
			ST_Edge edge;
			ST_SphereTraceData std;
			float minDist = FLT_MAX;
			if (!coordInYBounds)
			{
				if (dp.y > 0.0f)
				{
					edge = sphereTraceEdgeConstruct1(sphereTraceColliderAABBGetExtentByOctant(paabb, ST_LEFT_UP_FORWARD),
						sphereTraceColliderAABBGetExtentByOctant(paabb, ST_RIGHT_UP_FORWARD), paabb->halfExtents.x * 2.0f, gDirectionRight);
				}
				else
				{
					edge = sphereTraceEdgeConstruct1(sphereTraceColliderAABBGetExtentByOctant(paabb, ST_LEFT_DOWN_FORWARD),
						sphereTraceColliderAABBGetExtentByOctant(paabb, ST_RIGHT_DOWN_FORWARD), paabb->halfExtents.x * 2.0f, gDirectionRight);
				}
				if (sphereTraceColliderEdgeSphereTrace(from, dir, radius, &edge, &std))
				{
					*pData = std;
					minDist = std.traceDistance;
				}
			}

			if (!coordInXBounds)
			{
				if (dp.x > 0.0f)
				{
					edge = sphereTraceEdgeConstruct1(sphereTraceColliderAABBGetExtentByOctant(paabb, ST_RIGHT_DOWN_FORWARD),
						sphereTraceColliderAABBGetExtentByOctant(paabb, ST_RIGHT_UP_FORWARD), paabb->halfExtents.y * 2.0f, gDirectionUp);
				}
				else
				{
					edge = sphereTraceEdgeConstruct1(sphereTraceColliderAABBGetExtentByOctant(paabb, ST_LEFT_DOWN_FORWARD),
						sphereTraceColliderAABBGetExtentByOctant(paabb, ST_LEFT_UP_FORWARD), paabb->halfExtents.y * 2.0f, gDirectionUp);
				}
				if (sphereTraceColliderEdgeSphereTrace(from, dir, radius, &edge, &std))
				{
					if (std.traceDistance < minDist)
					{
						*pData = std;
						minDist = std.traceDistance;
					}
				}
			}
			if (minDist != FLT_MAX)
			{
				return 1;
			}
		}
	}
	return 0;
}

b32 sphereTraceColliderAABBBackFaceSphereTrace(ST_Vector3 from, ST_Direction dir, float radius, const ST_AABB* const paabb, ST_SphereTraceData* const pData)
{
	if (sphereTraceColliderInfiniteZPlaneSphereTrace(from, dir, radius, paabb->lowExtent, pData))
	{
		ST_Vector3 contactPoint = pData->rayTraceData.contact.point;
		ST_Vector3 dp = sphereTraceVector3Subtract(contactPoint, paabb->center);
		b32 coordInYBounds = sphereTraceAbs(dp.y) <= paabb->halfExtents.y;
		b32 coordInXBounds = sphereTraceAbs(dp.x) <= paabb->halfExtents.x;
		if (coordInYBounds && coordInXBounds)
		{
			return 1;
		}
		else
		{
			ST_Edge edge;
			ST_SphereTraceData std;
			float minDist = FLT_MAX;
			if (!coordInYBounds)
			{
				if (dp.y > 0.0f)
				{
					edge = sphereTraceEdgeConstruct1(sphereTraceColliderAABBGetExtentByOctant(paabb, ST_LEFT_UP_BACK),
						sphereTraceColliderAABBGetExtentByOctant(paabb, ST_RIGHT_UP_BACK), paabb->halfExtents.x * 2.0f, gDirectionRight);
				}
				else
				{
					edge = sphereTraceEdgeConstruct1(sphereTraceColliderAABBGetExtentByOctant(paabb, ST_LEFT_DOWN_BACK),
						sphereTraceColliderAABBGetExtentByOctant(paabb, ST_RIGHT_DOWN_BACK), paabb->halfExtents.x * 2.0f, gDirectionRight);
				}
				if (sphereTraceColliderEdgeSphereTrace(from, dir, radius, &edge, &std))
				{
					*pData = std;
					minDist = std.traceDistance;
				}
			}

			if (!coordInXBounds)
			{
				if (dp.x > 0.0f)
				{
					edge = sphereTraceEdgeConstruct1(sphereTraceColliderAABBGetExtentByOctant(paabb, ST_RIGHT_DOWN_BACK),
						sphereTraceColliderAABBGetExtentByOctant(paabb, ST_RIGHT_UP_BACK), paabb->halfExtents.y * 2.0f, gDirectionUp);
				}
				else
				{
					edge = sphereTraceEdgeConstruct1(sphereTraceColliderAABBGetExtentByOctant(paabb, ST_LEFT_DOWN_BACK),
						sphereTraceColliderAABBGetExtentByOctant(paabb, ST_LEFT_UP_BACK), paabb->halfExtents.y * 2.0f, gDirectionUp);
				}
				if (sphereTraceColliderEdgeSphereTrace(from, dir, radius, &edge, &std))
				{
					if (std.traceDistance < minDist)
					{
						*pData = std;
						minDist = std.traceDistance;
					}
				}
			}
			if (minDist != FLT_MAX)
			{
				return 1;
			}
		}
	}
	return 0;
}

b32 sphereTraceColliderAABBRightFaceSphereTrace(ST_Vector3 from, ST_Direction dir, float radius, const ST_AABB* const paabb, ST_SphereTraceData* const pData)
{
	if (sphereTraceColliderInfiniteXPlaneSphereTrace(from, dir, radius, paabb->highExtent, pData))
	{
		ST_Vector3 contactPoint = pData->rayTraceData.contact.point;
		ST_Vector3 dp = sphereTraceVector3Subtract(contactPoint, paabb->center);
		b32 coordInYBounds = sphereTraceAbs(dp.y) <= paabb->halfExtents.y;
		b32 coordInZBounds = sphereTraceAbs(dp.z) <= paabb->halfExtents.z;
		if (coordInYBounds && coordInZBounds)
		{
			return 1;
		}
		else
		{
			ST_Edge edge;
			ST_SphereTraceData std;
			float minDist = FLT_MAX;
			if (!coordInYBounds)
			{
				if (dp.y > 0.0f)
				{
					edge = sphereTraceEdgeConstruct1(sphereTraceColliderAABBGetExtentByOctant(paabb, ST_RIGHT_UP_BACK),
						sphereTraceColliderAABBGetExtentByOctant(paabb, ST_RIGHT_UP_FORWARD), paabb->halfExtents.z * 2.0f, gDirectionForward);
				}
				else
				{
					edge = sphereTraceEdgeConstruct1(sphereTraceColliderAABBGetExtentByOctant(paabb, ST_RIGHT_DOWN_BACK),
						sphereTraceColliderAABBGetExtentByOctant(paabb, ST_RIGHT_DOWN_FORWARD), paabb->halfExtents.z * 2.0f, gDirectionForward);
				}
				if (sphereTraceColliderEdgeSphereTrace(from, dir, radius, &edge, &std))
				{
					*pData = std;
					minDist = std.traceDistance;
				}
			}

			if (!coordInZBounds)
			{
				if (dp.z > 0.0f)
				{
					edge = sphereTraceEdgeConstruct1(sphereTraceColliderAABBGetExtentByOctant(paabb, ST_RIGHT_DOWN_FORWARD),
						sphereTraceColliderAABBGetExtentByOctant(paabb, ST_RIGHT_UP_FORWARD), paabb->halfExtents.y * 2.0f, gDirectionUp);
				}
				else
				{
					edge = sphereTraceEdgeConstruct1(sphereTraceColliderAABBGetExtentByOctant(paabb, ST_RIGHT_DOWN_BACK),
						sphereTraceColliderAABBGetExtentByOctant(paabb, ST_RIGHT_UP_BACK), paabb->halfExtents.y * 2.0f, gDirectionUp);
				}
				if (sphereTraceColliderEdgeSphereTrace(from, dir, radius, &edge, &std))
				{
					if (std.traceDistance < minDist)
					{
						*pData = std;
						minDist = std.traceDistance;
					}
				}
			}
			if (minDist != FLT_MAX)
			{
				return 1;
			}
		}
	}
	return 0;
}

b32 sphereTraceColliderAABBLeftFaceSphereTrace(ST_Vector3 from, ST_Direction dir, float radius, const ST_AABB* const paabb, ST_SphereTraceData* const pData)
{
	if (sphereTraceColliderInfiniteXPlaneSphereTrace(from, dir, radius, paabb->lowExtent, pData))
	{
		ST_Vector3 contactPoint = pData->rayTraceData.contact.point;
		ST_Vector3 dp = sphereTraceVector3Subtract(contactPoint, paabb->center);
		b32 coordInYBounds = sphereTraceAbs(dp.y) <= paabb->halfExtents.y;
		b32 coordInZBounds = sphereTraceAbs(dp.z) <= paabb->halfExtents.z;
		if (coordInYBounds && coordInZBounds)
		{
			return 1;
		}
		else
		{
			ST_Edge edge;
			ST_SphereTraceData std;
			float minDist = FLT_MAX;
			if (!coordInYBounds)
			{
				if (dp.y > 0.0f)
				{
					edge = sphereTraceEdgeConstruct1(sphereTraceColliderAABBGetExtentByOctant(paabb, ST_LEFT_UP_BACK),
						sphereTraceColliderAABBGetExtentByOctant(paabb, ST_LEFT_UP_FORWARD), paabb->halfExtents.z * 2.0f, gDirectionForward);
				}
				else
				{
					edge = sphereTraceEdgeConstruct1(sphereTraceColliderAABBGetExtentByOctant(paabb, ST_LEFT_DOWN_BACK),
						sphereTraceColliderAABBGetExtentByOctant(paabb, ST_LEFT_DOWN_FORWARD), paabb->halfExtents.z * 2.0f, gDirectionForward);
				}
				if (sphereTraceColliderEdgeSphereTrace(from, dir, radius, &edge, &std))
				{
					*pData = std;
					minDist = std.traceDistance;
				}
			}

			if (!coordInZBounds)
			{
				if (dp.z > 0.0f)
				{
					edge = sphereTraceEdgeConstruct1(sphereTraceColliderAABBGetExtentByOctant(paabb, ST_LEFT_DOWN_FORWARD),
						sphereTraceColliderAABBGetExtentByOctant(paabb, ST_LEFT_UP_FORWARD), paabb->halfExtents.y * 2.0f, gDirectionUp);
				}
				else
				{
					edge = sphereTraceEdgeConstruct1(sphereTraceColliderAABBGetExtentByOctant(paabb, ST_LEFT_DOWN_BACK),
						sphereTraceColliderAABBGetExtentByOctant(paabb, ST_LEFT_UP_BACK), paabb->halfExtents.y * 2.0f, gDirectionUp);
				}
				if (sphereTraceColliderEdgeSphereTrace(from, dir, radius, &edge, &std))
				{
					if (std.traceDistance < minDist)
					{
						*pData = std;
						minDist = std.traceDistance;
					}
				}
			}
			if (minDist != FLT_MAX)
			{
				return 1;
			}
		}
	}
	return 0;
}

b32 sphereTraceColliderAABBUpFaceSphereTrace(ST_Vector3 from, ST_Direction dir, float radius, const ST_AABB* const paabb, ST_SphereTraceData* const pData)
{
	if (sphereTraceColliderInfiniteYPlaneSphereTrace(from, dir, radius, paabb->highExtent, pData))
	{
		ST_Vector3 contactPoint = pData->rayTraceData.contact.point;
		ST_Vector3 dp = sphereTraceVector3Subtract(contactPoint, paabb->center);
		b32 coordInXBounds = sphereTraceAbs(dp.x) <= paabb->halfExtents.x;
		b32 coordInZBounds = sphereTraceAbs(dp.z) <= paabb->halfExtents.z;
		if (coordInXBounds && coordInZBounds)
		{
			return 1;
		}
		else
		{
			ST_Edge edge;
			ST_SphereTraceData std;
			float minDist = FLT_MAX;
			if (!coordInXBounds)
			{
				if (dp.x > 0.0f)
				{
					edge = sphereTraceEdgeConstruct1(sphereTraceColliderAABBGetExtentByOctant(paabb, ST_RIGHT_UP_BACK),
						sphereTraceColliderAABBGetExtentByOctant(paabb, ST_RIGHT_UP_FORWARD), paabb->halfExtents.z * 2.0f, gDirectionForward);
				}
				else
				{
					edge = sphereTraceEdgeConstruct1(sphereTraceColliderAABBGetExtentByOctant(paabb, ST_LEFT_UP_BACK),
						sphereTraceColliderAABBGetExtentByOctant(paabb, ST_LEFT_UP_FORWARD), paabb->halfExtents.z * 2.0f, gDirectionForward);
				}
				if (sphereTraceColliderEdgeSphereTrace(from, dir, radius, &edge, &std))
				{
					*pData = std;
					minDist = std.traceDistance;
				}
			}

			if (!coordInZBounds)
			{
				if (dp.z > 0.0f)
				{
					edge = sphereTraceEdgeConstruct1(sphereTraceColliderAABBGetExtentByOctant(paabb, ST_LEFT_UP_FORWARD),
						sphereTraceColliderAABBGetExtentByOctant(paabb, ST_RIGHT_UP_FORWARD), paabb->halfExtents.x * 2.0f, gDirectionRight);
				}
				else
				{
					edge = sphereTraceEdgeConstruct1(sphereTraceColliderAABBGetExtentByOctant(paabb, ST_LEFT_UP_BACK),
						sphereTraceColliderAABBGetExtentByOctant(paabb, ST_RIGHT_UP_BACK), paabb->halfExtents.x * 2.0f, gDirectionRight);
				}
				if (sphereTraceColliderEdgeSphereTrace(from, dir, radius, &edge, &std))
				{
					if (std.traceDistance < minDist)
					{
						*pData = std;
						minDist = std.traceDistance;
					}
				}
			}
			if (minDist != FLT_MAX)
			{
				return 1;
			}
		}
	}
	return 0;
}

b32 sphereTraceColliderAABBDownFaceSphereTrace(ST_Vector3 from, ST_Direction dir, float radius, const ST_AABB* const paabb, ST_SphereTraceData* const pData)
{
	if (sphereTraceColliderInfiniteYPlaneSphereTrace(from, dir, radius, paabb->lowExtent, pData))
	{
		ST_Vector3 contactPoint = pData->rayTraceData.contact.point;
		ST_Vector3 dp = sphereTraceVector3Subtract(contactPoint, paabb->center);
		b32 coordInXBounds = sphereTraceAbs(dp.x) <= paabb->halfExtents.x;
		b32 coordInZBounds = sphereTraceAbs(dp.z) <= paabb->halfExtents.z;
		if (coordInXBounds && coordInZBounds)
		{
			return 1;
		}
		else
		{
			ST_Edge edge;
			ST_SphereTraceData std;
			float minDist = FLT_MAX;
			if (!coordInXBounds)
			{
				if (dp.x > 0.0f)
				{
					edge = sphereTraceEdgeConstruct1(sphereTraceColliderAABBGetExtentByOctant(paabb, ST_RIGHT_DOWN_BACK),
						sphereTraceColliderAABBGetExtentByOctant(paabb, ST_RIGHT_DOWN_FORWARD), paabb->halfExtents.z * 2.0f, gDirectionForward);
				}
				else
				{
					edge = sphereTraceEdgeConstruct1(sphereTraceColliderAABBGetExtentByOctant(paabb, ST_LEFT_DOWN_BACK),
						sphereTraceColliderAABBGetExtentByOctant(paabb, ST_LEFT_DOWN_FORWARD), paabb->halfExtents.z * 2.0f, gDirectionForward);
				}
				if (sphereTraceColliderEdgeSphereTrace(from, dir, radius, &edge, &std))
				{
					*pData = std;
					minDist = std.traceDistance;
				}
			}

			if (!coordInZBounds)
			{
				if (dp.z > 0.0f)
				{
					edge = sphereTraceEdgeConstruct1(sphereTraceColliderAABBGetExtentByOctant(paabb, ST_LEFT_DOWN_FORWARD),
						sphereTraceColliderAABBGetExtentByOctant(paabb, ST_RIGHT_DOWN_FORWARD), paabb->halfExtents.x * 2.0f, gDirectionRight);
				}
				else
				{
					edge = sphereTraceEdgeConstruct1(sphereTraceColliderAABBGetExtentByOctant(paabb, ST_LEFT_DOWN_BACK),
						sphereTraceColliderAABBGetExtentByOctant(paabb, ST_RIGHT_DOWN_BACK), paabb->halfExtents.x * 2.0f, gDirectionRight);
				}
				if (sphereTraceColliderEdgeSphereTrace(from, dir, radius, &edge, &std))
				{
					if (std.traceDistance < minDist)
					{
						*pData = std;
						minDist = std.traceDistance;
					}
				}
			}
			if (minDist != FLT_MAX)
			{
				return 1;
			}
		}
	}
	return 0;
}

b32 sphereTraceColliderEmptyAABBSphereTrace(ST_Vector3 from, ST_Direction dir, float radius, const ST_AABB* const paabb, ST_SphereTraceData* const pRaycastData)
{
	if (sphereTraceColliderAABBImposedSphereCollisionTest(paabb, from, radius, &pRaycastData->rayTraceData.contact))
	{
		pRaycastData->radius = radius;
		pRaycastData->sphereCenter = from;
		pRaycastData->traceDistance = 0.0f;
		pRaycastData->rayTraceData.distance = sphereTraceVector3Distance(pRaycastData->rayTraceData.contact.point, from);
		pRaycastData->rayTraceData.startPoint = from;
		return ST_TRUE;
	}
	sphereTraceDirectionNormalizeIfNotNormalizedByRef(&dir);
	float dot = sphereTraceVector3Dot(dir.v, gVector3Right);
	ST_SphereTraceData std;
	float minDist = FLT_MAX;
	if (dot <= -ST_COLLIDER_TOLERANCE)
	{
		if (sphereTraceColliderAABBRightFaceSphereTrace(from, dir, radius, paabb, &std))
		{
			minDist = std.traceDistance;
			*pRaycastData = std;
		}

	}
	else if (dot >= ST_COLLIDER_TOLERANCE)
	{
		if (sphereTraceColliderAABBLeftFaceSphereTrace(from, dir, radius, paabb, &std))
		{
			minDist = std.traceDistance;
			*pRaycastData = std;
		}
	}

	// check the vertical dots
	dot = sphereTraceVector3Dot(gVector3Up, dir.v);
	if (dot <= -ST_COLLIDER_TOLERANCE)
	{
		if (sphereTraceColliderAABBUpFaceSphereTrace(from, dir, radius, paabb, &std))
		{
			if (std.traceDistance < minDist)
			{
				minDist = std.traceDistance;
				*pRaycastData = std;
			}
		}
	}
	else if (dot >= ST_COLLIDER_TOLERANCE)
	{
		if (sphereTraceColliderAABBDownFaceSphereTrace(from, dir, radius, paabb, &std))
		{
			if (std.traceDistance < minDist)
			{
				minDist = std.traceDistance;
				*pRaycastData = std;
			}
		}
	}

	// check the vertical dots
	dot = sphereTraceVector3Dot(gVector3Forward, dir.v);
	if (dot <= -ST_COLLIDER_TOLERANCE)
	{
		if (sphereTraceColliderAABBFrontFaceSphereTrace(from, dir, radius, paabb, &std))
		{
			if (std.traceDistance < minDist)
			{
				minDist = std.traceDistance;
				*pRaycastData = std;
			}
		}
	}
	else if (dot >= ST_COLLIDER_TOLERANCE)
	{
		if (sphereTraceColliderAABBBackFaceSphereTrace(from, dir, radius, paabb, &std))
		{
			if (std.traceDistance < minDist)
			{
				minDist = std.traceDistance;
				*pRaycastData = std;
			}
		}
	}
	if (minDist != FLT_MAX)
		return ST_TRUE;
	return ST_FALSE;
}

b32 sphereTraceColliderAABBSphereTrace(ST_Vector3 from, ST_Direction dir, float radius, const ST_Collider* const pCollider, ST_SphereTraceData* const pRaycastData)
{
	if (sphereTraceColliderEmptyAABBSphereTrace(from, dir, radius, &pCollider->aabb, pRaycastData))
	{
		pRaycastData->rayTraceData.pOtherCollider = pCollider;
		return ST_TRUE;
	}

	return ST_FALSE;
}
b32 sphereTraceColliderAABBSphereTraceOut(ST_Vector3 spherePos, float sphereRadius, ST_Direction clipoutDir, ST_Collider* const pCollider, ST_SphereTraceData* const pSphereCastData)
{
	sphereTraceDirectionNormalizeIfNotNormalizedByRef(&clipoutDir);
	ST_Vector3 castPoint = sphereTraceVector3AddAndScale(spherePos, clipoutDir.v, 2 * pCollider->boundingRadius + 2 * sphereRadius);
	b32 ret = sphereTraceColliderAABBSphereTrace(castPoint, sphereTraceDirectionNegative(clipoutDir), sphereRadius, pCollider, pSphereCastData);
	//ST_Direction dir = sphereTraceDirectionConstructNormalized(sphereTraceVector3Subtract(castPoint, pSphereCastData->sphereCenter));
	return ret;
}

ST_Octant sphereTraceOctantGetNextFromDirection(ST_Octant curOctant, ST_DirectionType dir)
{
	switch (curOctant)
	{
	case ST_LEFT_DOWN_BACK:
	{
		switch (dir)
		{
		case ST_DIRECTION_RIGHT:
			return ST_RIGHT_DOWN_BACK;
			break;
		case ST_DIRECTION_LEFT:
			return ST_OCTANT_NONE;
			break;
		case ST_DIRECTION_UP:
			return ST_LEFT_UP_BACK;
			break;
		case ST_DIRECTION_DOWN:
			return ST_OCTANT_NONE;
			break;
		case ST_DIRECTION_FORWARD:
			return ST_LEFT_DOWN_FORWARD;
			break;
		case ST_DIRECTION_BACK:
			return ST_OCTANT_NONE;
			break;
		}
	}
	break;
	case ST_RIGHT_DOWN_BACK:
	{
		switch (dir)
		{
		case ST_DIRECTION_RIGHT:
			return ST_OCTANT_NONE;
			break;
		case ST_DIRECTION_LEFT:
			return ST_LEFT_DOWN_BACK;
			break;
		case ST_DIRECTION_UP:
			return ST_RIGHT_UP_BACK;
			break;
		case ST_DIRECTION_DOWN:
			return ST_OCTANT_NONE;
			break;
		case ST_DIRECTION_FORWARD:
			return ST_RIGHT_DOWN_FORWARD;
			break;
		case ST_DIRECTION_BACK:
			return ST_OCTANT_NONE;
			break;
		}
	}
	break;
	case ST_LEFT_DOWN_FORWARD:
	{
		switch (dir)
		{
		case ST_DIRECTION_RIGHT:
			return ST_RIGHT_DOWN_FORWARD;
			break;
		case ST_DIRECTION_LEFT:
			return ST_OCTANT_NONE;
			break;
		case ST_DIRECTION_UP:
			return ST_LEFT_UP_FORWARD;
			break;
		case ST_DIRECTION_DOWN:
			return ST_OCTANT_NONE;
			break;
		case ST_DIRECTION_FORWARD:
			return ST_OCTANT_NONE;
			break;
		case ST_DIRECTION_BACK:
			return ST_LEFT_DOWN_BACK;
			break;
		}
	}
	break;
	case ST_RIGHT_DOWN_FORWARD:
	{
		switch (dir)
		{
		case ST_DIRECTION_RIGHT:
			return ST_OCTANT_NONE;
			break;
		case ST_DIRECTION_LEFT:
			return ST_LEFT_DOWN_FORWARD;
			break;
		case ST_DIRECTION_UP:
			return ST_RIGHT_UP_FORWARD;
			break;
		case ST_DIRECTION_DOWN:
			return ST_OCTANT_NONE;
			break;
		case ST_DIRECTION_FORWARD:
			return ST_OCTANT_NONE;
			break;
		case ST_DIRECTION_BACK:
			return ST_RIGHT_DOWN_BACK;
			break;
		}
	}
	break;

	case ST_LEFT_UP_BACK:
	{
		switch (dir)
		{
		case ST_DIRECTION_RIGHT:
			return ST_RIGHT_UP_BACK;
			break;
		case ST_DIRECTION_LEFT:
			return ST_OCTANT_NONE;
			break;
		case ST_DIRECTION_UP:
			return ST_OCTANT_NONE;
			break;
		case ST_DIRECTION_DOWN:
			return ST_LEFT_DOWN_BACK;
			break;
		case ST_DIRECTION_FORWARD:
			return ST_LEFT_UP_FORWARD;
			break;
		case ST_DIRECTION_BACK:
			return ST_OCTANT_NONE;
			break;
		}
	}
	break;
	case ST_RIGHT_UP_BACK:
	{
		switch (dir)
		{
		case ST_DIRECTION_RIGHT:
			return ST_OCTANT_NONE;
			break;
		case ST_DIRECTION_LEFT:
			return ST_LEFT_UP_BACK;
			break;
		case ST_DIRECTION_UP:
			return ST_OCTANT_NONE;
			break;
		case ST_DIRECTION_DOWN:
			return ST_RIGHT_DOWN_BACK;
			break;
		case ST_DIRECTION_FORWARD:
			return ST_RIGHT_UP_FORWARD;
			break;
		case ST_DIRECTION_BACK:
			return ST_OCTANT_NONE;
			break;
		}
	}
	break;
	case ST_LEFT_UP_FORWARD:
	{
		switch (dir)
		{
		case ST_DIRECTION_RIGHT:
			return ST_RIGHT_UP_FORWARD;
			break;
		case ST_DIRECTION_LEFT:
			return ST_OCTANT_NONE;
			break;
		case ST_DIRECTION_UP:
			return ST_OCTANT_NONE;
			break;
		case ST_DIRECTION_DOWN:
			return ST_LEFT_DOWN_FORWARD;
			break;
		case ST_DIRECTION_FORWARD:
			return ST_OCTANT_NONE;
			break;
		case ST_DIRECTION_BACK:
			return ST_LEFT_UP_BACK;
			break;
		}
	}
	break;
	case ST_RIGHT_UP_FORWARD:
	{
		switch (dir)
		{
		case ST_DIRECTION_RIGHT:
			return ST_OCTANT_NONE;
			break;
		case ST_DIRECTION_LEFT:
			return ST_LEFT_UP_FORWARD;
			break;
		case ST_DIRECTION_UP:
			return ST_OCTANT_NONE;
			break;
		case ST_DIRECTION_DOWN:
			return ST_RIGHT_DOWN_FORWARD;
			break;
		case ST_DIRECTION_FORWARD:
			return ST_OCTANT_NONE;
			break;
		case ST_DIRECTION_BACK:
			return ST_RIGHT_UP_BACK;
			break;
		}
	}
	break;
	}
}

b32 sphereTraceOctantIsOnRightSide(ST_Octant curOctant)
{
	return (curOctant % 2 == 1);
}

b32 sphereTraceOctantIsOnUpSide(ST_Octant curOctant)
{
	return curOctant > 3;
}

b32 sphereTraceOctantIsOnForwardSide(ST_Octant curOctant)
{
	return (curOctant % 4 > 1);
}

ST_Vector3 sphereTraceColliderAABBMidPoint(const ST_AABB* const aabb)
{
	return sphereTraceVector3Average(aabb->lowExtent, aabb->highExtent);
}

ST_Vector3 sphereTraceColliderAABBGetExtentByOctant(const ST_AABB* const paabb, ST_Octant octant)
{
	switch (octant)
	{
	case ST_LEFT_DOWN_BACK:
		return paabb->lowExtent;
		break;
	case ST_RIGHT_DOWN_BACK:
		return sphereTraceVector3Construct(paabb->highExtent.x, paabb->lowExtent.y, paabb->lowExtent.z);
		break;
	case ST_LEFT_DOWN_FORWARD:
		return sphereTraceVector3Construct(paabb->lowExtent.x, paabb->lowExtent.y, paabb->highExtent.z);
		break;
	case ST_RIGHT_DOWN_FORWARD:
		return sphereTraceVector3Construct(paabb->highExtent.x, paabb->lowExtent.y, paabb->highExtent.z);
		break;
	case ST_LEFT_UP_BACK:
		return sphereTraceVector3Construct(paabb->lowExtent.x, paabb->highExtent.y, paabb->lowExtent.z);
		break;
	case ST_RIGHT_UP_BACK:
		return sphereTraceVector3Construct(paabb->highExtent.x, paabb->highExtent.y, paabb->lowExtent.z);
		break;
	case ST_LEFT_UP_FORWARD:
		return sphereTraceVector3Construct(paabb->lowExtent.x, paabb->highExtent.y, paabb->highExtent.z);
		break;
	case ST_RIGHT_UP_FORWARD:
		return paabb->highExtent;
		break;
	}
}

//ST_Vector3 sphereTraceColliderAABBGetRightBottomBackExtent(const ST_AABB* const paabb)
//{
//	return sphereTraceVector3Construct(paabb->highExtent.x, paabb->lowExtent.y, paabb->lowExtent.z);
//}
//
//ST_Vector3 sphereTraceColliderAABBGetLeftTopBackExtent(const ST_AABB* const paabb)
//{
//	return sphereTraceVector3Construct(paabb->lowExtent.x, paabb->highExtent.y, paabb->lowExtent.z);
//}
//
//ST_Vector3 sphereTraceColliderAABBGetLeftBottomForwardExtent(const ST_AABB* const paabb)
//{
//	return sphereTraceVector3Construct(paabb->lowExtent.x, paabb->lowExtent.y, paabb->highExtent.z);
//}
//
//ST_Vector3 sphereTraceColliderAABBGetRightTopBackExtent(const ST_AABB* const paabb)
//{
//	return sphereTraceVector3Construct(paabb->highExtent.x, paabb->highExtent.y, paabb->lowExtent.z);
//}
//
//ST_Vector3 sphereTraceColliderAABBGetRightBottomForwardExtent(const ST_AABB* const paabb)
//{
//	return sphereTraceVector3Construct(paabb->highExtent.x, paabb->lowExtent.y, paabb->highExtent.z);
//}
//
//ST_Vector3 sphereTraceColliderAABBGetLeftTopForwardExtent(const ST_AABB* const paabb)
//{
//	return sphereTraceVector3Construct(paabb->lowExtent.x, paabb->highExtent.y, paabb->highExtent.z);
//}


void sphereTraceColliderResizeAABBWithSpherecast(const ST_SphereTraceData* const pSphereCastData, ST_AABB* const aabb)
{
	ST_Vector3 dp = sphereTraceVector3Subtract(pSphereCastData->sphereCenter, pSphereCastData->rayTraceData.startPoint);

	if (dp.x >= 0.0f)
	{
		if (dp.y >= 0.0f)
		{
			if (dp.z >= 0.0f)
			{
				aabb->lowExtent = sphereTraceVector3AddAndScale(pSphereCastData->rayTraceData.startPoint, gVector3One, -pSphereCastData->radius);
				aabb->highExtent = sphereTraceVector3AddAndScale(pSphereCastData->sphereCenter, gVector3One, pSphereCastData->radius);
			}
			else
			{
				aabb->lowExtent = sphereTraceVector3AddAndScale(sphereTraceVector3Construct(pSphereCastData->rayTraceData.startPoint.x,
					pSphereCastData->rayTraceData.startPoint.y, pSphereCastData->sphereCenter.z), gVector3One, -pSphereCastData->radius);
				aabb->highExtent = sphereTraceVector3AddAndScale(sphereTraceVector3Construct(pSphereCastData->sphereCenter.x,
					pSphereCastData->sphereCenter.y, pSphereCastData->rayTraceData.startPoint.z), gVector3One, pSphereCastData->radius);
			}
		}
		else
		{
			if (dp.z >= 0.0f)
			{
				aabb->lowExtent = sphereTraceVector3AddAndScale(sphereTraceVector3Construct(pSphereCastData->rayTraceData.startPoint.x,
					pSphereCastData->sphereCenter.y, pSphereCastData->rayTraceData.startPoint.z), gVector3One, -pSphereCastData->radius);
				aabb->highExtent = sphereTraceVector3AddAndScale(sphereTraceVector3Construct(pSphereCastData->sphereCenter.x,
					pSphereCastData->rayTraceData.startPoint.y, pSphereCastData->sphereCenter.z), gVector3One, pSphereCastData->radius);
			}
			else
			{
				aabb->lowExtent = sphereTraceVector3AddAndScale(sphereTraceVector3Construct(pSphereCastData->rayTraceData.startPoint.x,
					pSphereCastData->sphereCenter.y, pSphereCastData->sphereCenter.z), gVector3One, -pSphereCastData->radius);
				aabb->highExtent = sphereTraceVector3AddAndScale(sphereTraceVector3Construct(pSphereCastData->sphereCenter.x,
					pSphereCastData->rayTraceData.startPoint.y, pSphereCastData->rayTraceData.startPoint.z), gVector3One, pSphereCastData->radius);

			}
		}
	}
	else
	{
		if (dp.y >= 0.0f)
		{
			if (dp.z >= 0.0f)
			{
				aabb->lowExtent = sphereTraceVector3AddAndScale(sphereTraceVector3Construct(pSphereCastData->sphereCenter.x,
					pSphereCastData->rayTraceData.startPoint.y, pSphereCastData->rayTraceData.startPoint.z), gVector3One, -pSphereCastData->radius);
				aabb->highExtent = sphereTraceVector3AddAndScale(sphereTraceVector3Construct(pSphereCastData->rayTraceData.startPoint.x,
					pSphereCastData->sphereCenter.y, pSphereCastData->sphereCenter.z), gVector3One, pSphereCastData->radius);
			}
			else
			{
				aabb->lowExtent = sphereTraceVector3AddAndScale(sphereTraceVector3Construct(pSphereCastData->sphereCenter.x,
					pSphereCastData->rayTraceData.startPoint.y, pSphereCastData->sphereCenter.z), gVector3One, -pSphereCastData->radius);
				aabb->highExtent = sphereTraceVector3AddAndScale(sphereTraceVector3Construct(pSphereCastData->rayTraceData.startPoint.x,
					pSphereCastData->sphereCenter.y, pSphereCastData->rayTraceData.startPoint.z), gVector3One, pSphereCastData->radius);
			}
		}
		else
		{
			if (dp.z >= 0.0f)
			{
				aabb->lowExtent = sphereTraceVector3AddAndScale(sphereTraceVector3Construct(pSphereCastData->sphereCenter.x,
					pSphereCastData->sphereCenter.y, pSphereCastData->rayTraceData.startPoint.z), gVector3One, -pSphereCastData->radius);
				aabb->highExtent = sphereTraceVector3AddAndScale(sphereTraceVector3Construct(pSphereCastData->rayTraceData.startPoint.x,
					pSphereCastData->rayTraceData.startPoint.y, pSphereCastData->sphereCenter.z), gVector3One, pSphereCastData->radius);
			}
			else
			{
				aabb->lowExtent = sphereTraceVector3AddAndScale(pSphereCastData->sphereCenter, gVector3One, -pSphereCastData->radius);
				aabb->highExtent = sphereTraceVector3AddAndScale(pSphereCastData->rayTraceData.startPoint, gVector3One, pSphereCastData->radius);
			}
		}
	}

	//aabb->halfExtents = sphereTraceVector3Subtract(aabb->highExtent, sphereTraceVector3Average(aabb->highExtent, aabb->lowExtent));
	*aabb = sphereTraceAABBConstruct1(aabb->lowExtent, aabb->highExtent);
}



b32 sphereTraceColliderPointSphereTrace(ST_Vector3 from, ST_Direction dir, float radius, ST_Vector3 point, ST_SphereTraceData* const pSphereTraceData)
{
	sphereTraceDirectionNormalizeIfNotNormalizedByRef(&dir);
	ST_Vector3 dp = sphereTraceVector3Subtract(point, from);
	float fwdDist = sphereTraceVector3Dot(dp, dir.v);
	ST_Vector3 dpFwd = sphereTraceVector3Scale(dir.v, fwdDist);
	ST_Vector3 dpRight = sphereTraceVector3Subtract(dp, dpFwd);
	float rightDist = sphereTraceVector3Length(dpRight);
	if (fpclassify(rightDist) == FP_NAN)
	{
		pSphereTraceData->rayTraceData.startPoint = from;
		pSphereTraceData->rayTraceData.contact.point = point;
		pSphereTraceData->rayTraceData.distance = sphereTraceVector3Length(sphereTraceVector3Subtract(pSphereTraceData->rayTraceData.startPoint, pSphereTraceData->rayTraceData.contact.point));
		pSphereTraceData->radius = radius;
		pSphereTraceData->sphereCenter = sphereTraceVector3AddAndScale(point, dir.v, -radius);
		pSphereTraceData->rayTraceData.contact.normal = sphereTraceDirectionConstructNormalized(sphereTraceVector3Subtract(pSphereTraceData->sphereCenter, pSphereTraceData->rayTraceData.contact.point));
		pSphereTraceData->traceDistance = sphereTraceVector3Distance(pSphereTraceData->sphereCenter, pSphereTraceData->rayTraceData.startPoint);
		return 1;
	}
	else if (rightDist <= radius)
	{
		pSphereTraceData->rayTraceData.startPoint = from;
		pSphereTraceData->rayTraceData.contact.point = point;
		pSphereTraceData->rayTraceData.distance = sphereTraceVector3Length(sphereTraceVector3Subtract(pSphereTraceData->rayTraceData.startPoint, pSphereTraceData->rayTraceData.contact.point));
		pSphereTraceData->radius = radius;
		float beta = acosf(rightDist / radius);
		float opposite = sinf(beta) * radius;
		pSphereTraceData->sphereCenter = sphereTraceVector3AddAndScale(sphereTraceVector3AddAndScale(point, dpRight, -1.0f), dir.v, -opposite);
		pSphereTraceData->rayTraceData.contact.normal = sphereTraceDirectionConstructNormalized(sphereTraceVector3Subtract(pSphereTraceData->sphereCenter, pSphereTraceData->rayTraceData.contact.point));
		pSphereTraceData->traceDistance = sphereTraceVector3Distance(pSphereTraceData->sphereCenter, pSphereTraceData->rayTraceData.startPoint);
		return 1;
	}
	return 0;
}

b32 sphereTraceColliderEdgeSphereTrace(ST_Vector3 from, ST_Direction dir, float radius, ST_Edge* const pEdge, ST_SphereTraceData* const pSphereTraceData);
//b32 sphereTraceColliderEdgeSphereTrace(ST_Vector3 from, ST_Direction dir, float radius, ST_Edge* const pEdge, ST_SphereTraceData* const pSphereTraceData)
//{
//	float closestPointToStart = 0;
//	float t;
//	ST_Vector3 edgeTowardDir1;
//	ST_Vector3 point;
//	sphereTraceMathClosestPointOnLineNearestToPointExtractData(pEdge->point1, pEdge->dir.v, from, &point, &edgeTowardDir1, &t, &closestPointToStart);
//	sceneDrawImposedSphere(point, 0.2f, gVector4ColorRed);
//	rendererDrawLineFromTo(pEdge->point1, point, gVector4ColorMagenta);
//	if (closestPointToStart <= radius)
//	{
//		return 1;
//	}
//	float distOnToward, distOnEdge;
//	//ST_Vector3 towardEdge = sphereTraceVector3Cross(sphereTraceVector3Cross(dir.v, pEdge->dir.v), dir.v);
//	//sphereTraceMathClosestPointOnLineNearestToPointExtractData(pEdge->point1, pEdge->dir.v, )
//	float yIntersept = sphereTraceVector3Distance(from, point);
//	ST_Vector3 edgeTowardDir2;
//	float x1 = 0, y1 = 0;
//	ST_Vector3 point1;
//	float x2 = 0, y2 = 0;
//	ST_Vector3 point2;
//	sphereTraceMathClosestPointOnLineNearestToPointExtractData(from, dir.v, pEdge->point1, &point1,&edgeTowardDir1, &x1, &y1);
//	sphereTraceMathClosestPointOnLineNearestToPointExtractData(from, dir.v, pEdge->point2, &point2,&edgeTowardDir2, &x2, &y2);
//	sceneDrawArrow(point1, edgeTowardDir1, 1, 0.1f, gVector4ColorWhite);
//	sceneDrawArrow(point2, edgeTowardDir2, 1, 0.1f, gVector4ColorWhite);
//	sceneDrawImposedSphere(point1, 0.3f, gVector4ColorRed);
//	sceneDrawImposedSphere(point2, 0.3f, gVector4ColorBlue);
//	rendererDrawLineFromTo(from, point2, gVector4ColorRed);
//	float edgeDistClosest = 0, yMin = 0;
//	float xClosest = 0;
//	sphereTraceVector3ClosestPointOnLineBetweenTwoLinesDistancesOnLines(from, dir.v, pEdge->point1, pEdge->dir.v, &xClosest, &edgeDistClosest);
//	yMin = sphereTraceVector3ClosestPointOnLineBetweenTwoLinesDistance(from, dir.v, pEdge->point1, pEdge->dir.v);
//	float slope;
//	float ctheta = sphereTraceVector3Dot(dir.v, pEdge->dir.v);
//	float dist = 0;
//
//	if (x1 < x2)
//	{
//		if (xClosest<x2 && xClosest>x1)
//		{
//			dist =sphereTraceMathCircleIntersectLineWithPoints(radius, yIntersept, x1, y1, xClosest, yMin);
//		}
//	}
//
//	
//	pSphereTraceData->rayTraceData.startPoint = from;
//	//pSphereTraceData->rayTraceData.contact.point = intersection;
//	//pSphereTraceData->rayTraceData.distance = sphereTraceVector3Length(sphereTraceVector3Subtract(pSphereTraceData->rayTraceData.startPoint, pSphereTraceData->rayTraceData.contact.point));
//	pSphereTraceData->radius = radius;
//	pSphereTraceData->sphereCenter = sphereTraceVector3AddAndScale(from, dir.v, dist);
//	//pSphereTraceData->rayTraceData.contact.normal = sphereTraceDirectionConstructNormalized(sphereTraceVector3Subtract(pSphereTraceData->sphereCenter, intersection));
//	pSphereTraceData->traceDistance = dist;
//	return 1;
//}


b32 sphereTraceColliderEdgeSphereTrace_(ST_Vector3 from, ST_Direction dir, float radius, ST_Edge* const pEdge, ST_SphereTraceData* const pSphereTraceData)
{
	ST_SphereContact contact;
	//dir = sphereTraceVector3Normalize(dir);
	sphereTraceDirectionNormalizeIfNotNormalizedByRef(&dir);
	if (sphereTraceColliderEdgeImposedSphereCollisionTest(pEdge, from, radius, &contact))
	{
		pSphereTraceData->radius = radius;
		pSphereTraceData->rayTraceData.startPoint = from;
		pSphereTraceData->sphereCenter = from;
		pSphereTraceData->rayTraceData.contact.point = contact.point;
		pSphereTraceData->rayTraceData.contact.normal = contact.normal;
		pSphereTraceData->rayTraceData.distance = sphereTraceVector3Length(sphereTraceVector3Subtract(from, contact.point));
		pSphereTraceData->traceDistance = 0.0f;
		return 1;
	}
	ST_Vector3 right = sphereTraceVector3Normalize(sphereTraceVector3Cross(dir.v, pEdge->dir.v));
	//ST_Vector3 sphereDir = sphereTraceVector3Normalize(sphereTraceVector3Cross(right, edgeDir));
	ST_Vector3 sphereDir = sphereTraceVector3Negative(dir.v);
	float d = sphereTraceAbs(sphereTraceVector3Dot(pEdge->dir.v, dir.v));
	ST_Vector3 fwd = sphereTraceVector3Normalize(sphereTraceVector3Cross(right, dir.v));
	if (d > ST_COLLIDER_TOLERANCE)
		sphereDir = sphereTraceVector3Normalize(sphereTraceVector3Cross(right, pEdge->dir.v));
	//if (d < ST_COLLIDER_TOLERANCE)
	//{
	//	sphereDir = sphereTraceVector3Negative(dir);
	//	fwd = sphereDir;
	//}
	float theta = acosf(sphereTraceVector3Dot(pEdge->dir.v, dir.v));
	float rightDist;
	rightDist = sphereTraceVector3Dot(sphereTraceVector3Subtract(from, pEdge->point1), right);
	if (sphereTraceAbs(rightDist) < radius)
	{
		float beta = acosf(rightDist / radius);
		float fwdDist = sinf(beta) * radius;
		ST_Vector3 pointOnEdgeClosestToCenterRaycast = sphereTraceClosestPointOnLineBetweenTwoLines(pEdge->point1, pEdge->dir.v, from, dir.v);
		if (sphereTraceVector3Dot(dir.v, sphereTraceVector3Subtract(pointOnEdgeClosestToCenterRaycast, from)) < 0.0f)
			return 0;
		ST_Vector3 pointOnPlaneNearOriginClosestToEdge = sphereTraceClosestPointOnLineBetweenTwoLines(from, right, pEdge->point1, pEdge->dir.v);
		pointOnPlaneNearOriginClosestToEdge = sphereTraceVector3AddAndScale(pointOnPlaneNearOriginClosestToEdge, fwd, -fwdDist);
		ST_Vector3 pointOnEdgeTracedFrompointOnPlaneNearOriginClosestToEdge = sphereTraceClosestPointOnLineBetweenTwoLines(pEdge->point1, pEdge->dir.v, pointOnPlaneNearOriginClosestToEdge, dir.v);
		float distToPointOnEdgeOrthogonalWithSphereDirectionAndEdge = cosf(theta) * sphereTraceVector3Length(sphereTraceVector3Subtract(pointOnEdgeTracedFrompointOnPlaneNearOriginClosestToEdge, pointOnEdgeClosestToCenterRaycast));
		ST_Vector3 intersection = sphereTraceVector3AddAndScale(pointOnEdgeClosestToCenterRaycast, pEdge->dir.v, -distToPointOnEdgeOrthogonalWithSphereDirectionAndEdge);

		fwdDist = sphereTraceAbs(tanf(theta) * sphereTraceVector3Length(sphereTraceVector3Subtract(pointOnEdgeClosestToCenterRaycast, intersection)));
		ST_Vector3 testPoint2 = sphereTraceClosestPointOnLineBetweenTwoLines(intersection, sphereDir, pointOnEdgeClosestToCenterRaycast, dir.v);
		if (sphereTraceVector3Nan(testPoint2))
		{
			fwdDist = sinf(beta) * radius;
			testPoint2 = sphereTraceVector3AddAndScale(intersection, sphereDir, fwdDist);
		}
		float edgeDist = sphereTraceVector3Dot(sphereTraceVector3Subtract(intersection, pEdge->point1), pEdge->dir.v);
		if (edgeDist < 0)
		{
			return sphereTraceColliderPointSphereTrace(from, dir, radius, pEdge->point1, pSphereTraceData);
		}
		else if (edgeDist > pEdge->dist)
		{
			return sphereTraceColliderPointSphereTrace(from, dir, radius, pEdge->point2, pSphereTraceData);
		}
		else
		{

			pSphereTraceData->rayTraceData.startPoint = from;
			pSphereTraceData->rayTraceData.contact.point = intersection;
			pSphereTraceData->rayTraceData.distance = sphereTraceVector3Length(sphereTraceVector3Subtract(pSphereTraceData->rayTraceData.startPoint, pSphereTraceData->rayTraceData.contact.point));
			pSphereTraceData->radius = radius;
			pSphereTraceData->sphereCenter = sphereTraceVector3Add(sphereTraceVector3AddAndScale(pointOnPlaneNearOriginClosestToEdge, right, rightDist), sphereTraceVector3Subtract(testPoint2, pointOnPlaneNearOriginClosestToEdge));
			pSphereTraceData->rayTraceData.contact.normal = sphereTraceDirectionConstructNormalized(sphereTraceVector3Subtract(pSphereTraceData->sphereCenter, intersection));
			pSphereTraceData->traceDistance = sphereTraceVector3Distance(pSphereTraceData->sphereCenter, pSphereTraceData->rayTraceData.startPoint);
			return 1;
		}
	}
	return 0;
}

b32 sphereTraceColliderEdgeSphereTrace1(ST_Vector3 from, ST_Direction dir, float radius, ST_Edge* const pEdge, ST_SphereTraceData* const pSphereTraceData)
{
	pSphereTraceData->radius = radius;
	//dir = sphereTraceVector3Normalize(dir);
	sphereTraceDirectionNormalizeIfNotNormalizedByRef(&dir);
	if (sphereTraceColliderEdgeImposedSphereCollisionTest(pEdge, from, radius, &pSphereTraceData->rayTraceData.contact))
	{
		pSphereTraceData->radius = radius;
		pSphereTraceData->rayTraceData.startPoint = from;
		pSphereTraceData->sphereCenter = from;
		pSphereTraceData->rayTraceData.distance = sphereTraceVector3Length(sphereTraceVector3Subtract(from, pSphereTraceData->rayTraceData.contact.point));
		pSphereTraceData->traceDistance = 0.0f;
		return 1;
	}
	ST_Vector3 cross = sphereTraceVector3Normalize(sphereTraceVector3Cross(dir.v, pEdge->dir.v));
	ST_Vector3 wall = sphereTraceVector3Cross(pEdge->dir.v, cross);
	sphereTraceColliderInfinitePlaneSphereTrace(from, dir, radius,
		pEdge->point1, sphereTraceDirectionConstructNormalized(wall), pSphereTraceData);
	float dist = sphereTraceVector3Dot(sphereTraceVector3Subtract(pSphereTraceData->rayTraceData.contact.point, pEdge->point1), cross);
	if (sphereTraceAbs(dist) <= radius)
	{
		ST_Vector3 dirRight = sphereTraceVector3Cross(dir.v, cross);
		float theta = acosf(dist, radius);
		float cbeta = sphereTraceVector3Dot(dir.v, pEdge->dir.v);
		float ringRad = sinf(theta) * cbeta * radius;
		ST_Vector3 lineStart = sphereTraceVector3AddAndScale(sphereTraceVector3AddAndScale(from, cross, -dist),
			dirRight, ringRad);
		ST_Vector3 lineEnd = sphereTraceClosestPointOnLineBetweenTwoLines(pEdge->point1, pEdge->dir.v,
			lineStart, dir.v);
		float lineDist = sphereTraceVector3Dot(sphereTraceVector3Subtract(lineEnd, pEdge->point1), pEdge->dir.v);
		if (lineDist < 0.0f)
		{
			return sphereTraceColliderPointSphereTrace(from, dir, radius, pEdge->point1, pSphereTraceData);
		}
		else if (lineDist > pEdge->dist)
		{
			return sphereTraceColliderPointSphereTrace(from, dir, radius, pEdge->point2, pSphereTraceData);
		}
		else
		{
			ST_RayTraceData rtd;
			if (sphereTraceColliderImposedSphereRayTrace(lineEnd, sphereTraceDirectionNegative(dir), pSphereTraceData->sphereCenter, pSphereTraceData->radius, &rtd))
			{
				//sceneDrawSphereCast(lineEnd, rtd.contact.point, 0.01f, gVector4ColorBlue);
				sphereTraceVector3AddAndScaleByRef(&pSphereTraceData->sphereCenter, dir.v, rtd.distance);
			}
			pSphereTraceData->traceDistance = sphereTraceVector3Distance(pSphereTraceData->sphereCenter, pSphereTraceData->rayTraceData.startPoint);
			pSphereTraceData->rayTraceData.contact.point = lineEnd;
			pSphereTraceData->rayTraceData.contact.normal = sphereTraceDirectionNegative(rtd.contact.normal);
			pSphereTraceData->rayTraceData.contact.collisionType = ST_COLLISION_EDGE;
			pSphereTraceData->rayTraceData.distance = sphereTraceVector3Distance(pSphereTraceData->rayTraceData.contact.point, pSphereTraceData->rayTraceData.startPoint);
			return 1;
		}
	}
	return 0;
}

b32 sphereTraceColliderEdgeSphereTrace(ST_Vector3 from, ST_Direction dir, float radius, ST_Edge* const pEdge, ST_SphereTraceData* const pSphereTraceData)
{
	pSphereTraceData->radius = radius;
	sphereTraceDirectionNormalizeIfNotNormalizedByRef(&dir);
	if (sphereTraceColliderEdgeImposedSphereCollisionTest(pEdge, from, radius, &pSphereTraceData->rayTraceData.contact))
	{
		pSphereTraceData->radius = radius;
		pSphereTraceData->rayTraceData.startPoint = from;
		pSphereTraceData->sphereCenter = from;
		pSphereTraceData->rayTraceData.distance = sphereTraceVector3Length(sphereTraceVector3Subtract(from, pSphereTraceData->rayTraceData.contact.point));
		pSphereTraceData->traceDistance = 0.0f;
		return 1;
	}
	ST_Vector3 cross = sphereTraceVector3Normalize(sphereTraceVector3Cross(dir.v, pEdge->dir.v));
	ST_Vector3 dp = sphereTraceVector3Subtract(pEdge->point1, from);
	float crossDist = sphereTraceVector3Dot(cross, dp);
	if (sphereTraceAbs(crossDist) <= radius)
	{
		float theta = asinf(crossDist / radius);
		float sliceRadius = cosf(theta) * radius;
		ST_Direction normal = sphereTraceDirectionConstruct(sphereTraceVector3Cross(cross, pEdge->dir.v), 1);
		ST_Vector3 temp = sphereTraceVector3Subtract(pEdge->point1, from);
		if (sphereTraceVector3IsVectorPositiveInDirection(temp, normal))
			temp = sphereTraceVector3AddAndScale(pEdge->point1, normal.v, -sliceRadius);
		else
			temp = sphereTraceVector3AddAndScale(pEdge->point1, normal.v, sliceRadius);
		sphereTraceColliderInfinitePlaneRayTrace(sphereTraceVector3AddAndScale(from, cross, crossDist), dir, normal, temp, &pSphereTraceData->rayTraceData);
		if (pSphereTraceData->rayTraceData.distance < 0.0f || isinf(pSphereTraceData->rayTraceData.distance))
			return 0;
		float lineDist = sphereTraceVector3Dot(sphereTraceVector3Subtract(pSphereTraceData->rayTraceData.contact.point, pEdge->point1), pEdge->dir.v);
		if (lineDist < 0.0f)
		{
			return sphereTraceColliderPointSphereTrace(from, dir, radius, pEdge->point1, pSphereTraceData);
		}
		else if (lineDist > pEdge->dist)
		{
			return sphereTraceColliderPointSphereTrace(from, dir, radius, pEdge->point2, pSphereTraceData);
		}
		else
		{

			temp = pSphereTraceData->rayTraceData.contact.point;
			pSphereTraceData->sphereCenter = sphereTraceVector3AddAndScale(temp, cross, -crossDist);
			pSphereTraceData->rayTraceData.contact.point = sphereTraceVector3AddAndScale(temp, pSphereTraceData->rayTraceData.contact.normal.v, -sliceRadius);
			pSphereTraceData->rayTraceData.contact.normal = sphereTraceDirectionConstructNormalized(sphereTraceVector3Subtract(pSphereTraceData->sphereCenter,
				pSphereTraceData->rayTraceData.contact.point));
			pSphereTraceData->rayTraceData.startPoint = from;
			pSphereTraceData->rayTraceData.distance = sphereTraceVector3Length(sphereTraceVector3Subtract(pSphereTraceData->rayTraceData.contact.point, from));
			pSphereTraceData->radius = radius;
			pSphereTraceData->traceDistance = sphereTraceVector3Distance(pSphereTraceData->sphereCenter, pSphereTraceData->rayTraceData.startPoint);
			//pSphereTraceData->rayTraceData.normal = sphereTraceDirectionConstructNormalized(sphereTraceVector3Subtract(pSphereTraceData->sphereCenter, pSphereTraceData->rayTraceData.hitPoint));
			return 1;
		}
	}
	return 0;
}


ST_EdgeTraceData sphereTraceEdgeTraceDataFrom1RayTraceData(const ST_RayTraceData* const prtd)
{
	ST_EdgeTraceData etd;
	etd.contact1 = prtd->contact;
	etd.distance = prtd->distance;
	etd.pOtherCollider = prtd->pOtherCollider;
	etd.usesBothContacts = ST_FALSE;
	return etd;
}

ST_EdgeTraceData sphereTraceEdgeTraceDataFrom2RayTraceData(const ST_RayTraceData* const prtd, const ST_RayTraceData* const prtd1)
{
	ST_EdgeTraceData etd;
	etd.contact1 = prtd->contact;
	etd.contact2 = prtd1->contact;
	etd.distance = prtd->distance;
	etd.pOtherCollider = prtd->pOtherCollider;
	etd.usesBothContacts = ST_TRUE;
	return etd;
}

b32 sphereTraceColliderEdgeEdgeTrace(ST_Edge* const pEdgeTrace, ST_Direction dir, ST_Edge* const pEdge, ST_EdgeTraceData* const pEdgeTraceData)
{
	ST_Vector3 crossVec = sphereTraceVector3Cross(pEdgeTrace->dir.v, pEdge->dir.v);
	ST_Direction crossDir = sphereTraceDirectionConstructNormalized(crossVec);
	ST_RayTraceData rtd;
	sphereTraceColliderInfinitePlaneRayTrace(pEdgeTrace->point1, dir, crossDir, pEdge->point1, &rtd);
	float crossSquared = sphereTraceVector3Length2(crossVec);
	float s = sphereTraceVector3Dot(sphereTraceVector3Cross(sphereTraceVector3Subtract(pEdge->point1, rtd.contact.point), pEdge->dir.v), crossVec) / crossSquared;
	float t = sphereTraceVector3Dot(sphereTraceVector3Cross(sphereTraceVector3Subtract(pEdge->point1, rtd.contact.point), pEdgeTrace->dir.v), crossVec) / crossSquared;
	rtd.contact.point = sphereTraceVector3AddAndScale(pEdge->point1, pEdge->dir.v, t);
	*pEdgeTraceData = sphereTraceEdgeTraceDataFrom1RayTraceData(&rtd);
	if (s >= 0.0f && s <= pEdgeTrace->dist)
	{
		if (t >= 0.0f && t <= pEdge->dist)
		{
			pEdgeTraceData->contact1.collisionType = ST_COLLISION_EDGE;
			return ST_TRUE;

		}
	}
	return ST_FALSE;
}

b32 sphereTraceColliderRingSphereTrace(ST_Vector3 from, ST_Direction dir, float radius, ST_Ring* const pRing, ST_SphereTraceData* const pSphereTraceData);

ST_SphereCollider* sphereTraceColliderSphereGetFromContact(const ST_SphereContact* const pContact)
{
	return pContact->pSphereCollider;
}
ST_PlaneCollider* sphereTraceColliderPlaneGetFromContact(const ST_SphereContact* const pContact)
{
	if (pContact->otherColliderType == COLLIDER_PLANE)
	{
		return pContact->pOtherCollider;
	}
}
ST_TriangleCollider* sphereTraceColliderTriangleGetFromContact(const ST_SphereContact* const pContact)
{
	if (pContact->otherColliderType == COLLIDER_TRIANGLE)
	{
		return pContact->pOtherCollider;
	}
}

const char* sphereTraceColliderGetColliderString(const ST_Collider* const pCollider)
{
	return ST_ColliderStrings[pCollider->colliderType];
}

b32 sphereTraceColliderRayTrace(ST_Vector3 from, ST_Direction dir, const ST_Collider* const pCollider, ST_RayTraceData* const pRayTraceData)
{
	switch (pCollider->colliderType)
	{
	case COLLIDER_PLANE:
		return sphereTraceColliderPlaneRayTrace(from, dir, pCollider, pRayTraceData);
		break;
	case COLLIDER_SPHERE:
		return sphereTraceColliderSphereRayTrace(from, dir, pCollider, pRayTraceData);
		break;
	case COLLIDER_TRIANGLE:
		return sphereTraceColliderTriangleRayTrace(from, dir, pCollider, pRayTraceData);
		break;
	case COLLIDER_AABB:
		return sphereTraceColliderAABBRayTrace(from, dir, pCollider, pRayTraceData);
		break;
	}
}

b32 sphereTraceColliderSphereTrace(ST_Vector3 from, float radius, ST_Direction dir, const ST_Collider* const pCollider, ST_SphereTraceData* const pSphereTraceData)
{
	switch (pCollider->colliderType)
	{
	case COLLIDER_PLANE:
		return sphereTraceColliderPlaneSphereTrace(from, dir, radius, pCollider, pSphereTraceData);
		break;
	case COLLIDER_SPHERE:
		return sphereTraceColliderSphereSphereTrace(from, dir, radius, pCollider, pSphereTraceData);
		break;
	case COLLIDER_TRIANGLE:
		return sphereTraceColliderTriangleSphereTrace(from, dir, radius, pCollider, pSphereTraceData);
		break;
	case COLLIDER_AABB:
		return sphereTraceColliderAABBSphereTrace(from, dir, radius, pCollider, pSphereTraceData);
		break;
	case COLLIDER_BOX:
		return 0;
		break;
	}
}


b32 sphereTraceColliderListRayTrace(ST_Vector3 from, ST_Direction dir, ST_IndexList* const pColliderList, ST_RayTraceData* const pRayTraceData)
{
	float minDist = FLT_MAX;
	ST_RayTraceData rtd;
	ST_Collider* pCollider;
	ST_IndexListData* pild = pColliderList->pFirst;
	for (ST_Index i = 0; i < pColliderList->count; i++)
	{
		pCollider = pild->value;
		if (sphereTraceColliderRayTrace(from, dir, pCollider, &rtd))
		{
			if (rtd.distance < minDist)
			{
				*pRayTraceData = rtd;
				minDist = rtd.distance;
			}
		}
		pild = pild->pNext;
	}
	if (minDist != FLT_MAX)
		return ST_TRUE;
	return ST_FALSE;
}

b32 sphereTraceColliderListSphereTrace(ST_Vector3 from,  ST_Direction dir, float radius, ST_IndexList* const pColliderList, ST_SphereTraceData* const pSphereTraceData)
{
	float minDist = FLT_MAX;
	ST_SphereTraceData std;
	ST_Collider* pCollider;
	ST_IndexListData* pild = pColliderList->pFirst;
	for (ST_Index i = 0; i < pColliderList->count; i++)
	{
		pCollider = pild->value;
		if (sphereTraceColliderSphereTrace(from,radius, dir, pCollider, &std))
		{
			if (std.traceDistance < minDist)
			{
				*pSphereTraceData = std;
				minDist = std.traceDistance;
			}
		}
		pild = pild->pNext;
	}
	if (minDist != FLT_MAX)
		return ST_TRUE;
	return ST_FALSE;
}
//ST_UniformTerrainCollider* sphereTraceColliderTerrainGetFromContact(const ST_SphereContact* const pContact);

ST_BoxContact sphereTraceBoxContactInit()
{
	ST_BoxContact bc;
	bc.numContacts = 0;
	bc.maxPenetrationIndex = 0;
	bc.numLerpPoints = 0;
	return bc;
}