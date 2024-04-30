#include "SphereTraceColliderTriangle.h"

ST_TriangleCollider sphereTraceColliderTriangleConstruct(ST_Vector3 v1, ST_Vector3 v2, ST_Vector3 v3)
{
	ST_TriangleCollider triangleCollider;
	sphereTraceColliderTriangleSetVertexAndEdgeData(&triangleCollider, v1, v2, v3);
	triangleCollider.ignoreCollisions = 0;
	triangleCollider.collider = sphereTraceColliderConstruct(COLLIDER_TRIANGLE, sphereTraceMax(sphereTraceMax(triangleCollider.vertexDists[0], triangleCollider.vertexDists[1]), triangleCollider.vertexDists[2]));
	sphereTraceColliderTriangleSetAABB(&triangleCollider);
	
	return triangleCollider;
}

int sphereTraceColliderTriangleGetClosestTransformedEdgeIndexToPoint(const ST_TriangleCollider* const pTriangleCollider, ST_Vector3 point)
{
	ST_Vector3 dp1 = sphereTraceVector3Subtract(point, pTriangleCollider->transformedEdges[0].point1);
	ST_Vector3 dp2 = sphereTraceVector3Subtract(point, pTriangleCollider->transformedEdges[1].point1);
	ST_Vector3 dp3 = sphereTraceVector3Subtract(point, pTriangleCollider->transformedEdges[2].point1);
	float normalizedDir0 = sphereTraceVector3Dot(pTriangleCollider->edgeOrthogonalDirs[0].v, dp1);
	float normalizedDir1 = sphereTraceVector3Dot(pTriangleCollider->edgeOrthogonalDirs[1].v, dp2);
	float normalizedDir2 = sphereTraceVector3Dot(pTriangleCollider->edgeOrthogonalDirs[2].v, dp3);
	if ((normalizedDir0 >= normalizedDir1) && (normalizedDir0 >= normalizedDir2))
	{
		return 0;
	}
	else if (normalizedDir1 >= normalizedDir2)
	{
		return 1;
	}
	else
	{
		return 2;
	}

}

int sphereTraceColliderTriangleGetClosestTransformedVertexIndexToPoint(const ST_TriangleCollider* const pTriangleCollider, ST_Vector3 point)
{
	ST_Vector3 dp = sphereTraceVector3Subtract(point, pTriangleCollider->centroid);
	float normalizedDir0 = sphereTraceVector3Dot(pTriangleCollider->vertexDirs[0].v, dp) / pTriangleCollider->vertexDists[0];
	float normalizedDir1 = sphereTraceVector3Dot(pTriangleCollider->vertexDirs[1].v, dp) / pTriangleCollider->vertexDists[1];
	float normalizedDir2 = sphereTraceVector3Dot(pTriangleCollider->vertexDirs[2].v, dp) / pTriangleCollider->vertexDists[2];
	if ((normalizedDir0 >= normalizedDir1) && (normalizedDir0 >= normalizedDir2))
	{
		return 0;
	}
	else if (normalizedDir1 >= normalizedDir2)
	{
		return 1;
	}
	else
	{
		return 2;
	}

}

void sphereTraceColliderTriangleSetAABB(ST_TriangleCollider* const pTriangleCollider)
{
	pTriangleCollider->collider.aabb.highExtent = sphereTraceVector3Construct(sphereTraceMax(pTriangleCollider->transformedVertices[0].x, sphereTraceMax(pTriangleCollider->transformedVertices[1].x, pTriangleCollider->transformedVertices[2].x)),
		sphereTraceMax(pTriangleCollider->transformedVertices[0].y, sphereTraceMax(pTriangleCollider->transformedVertices[1].y, pTriangleCollider->transformedVertices[2].y)),
		sphereTraceMax(pTriangleCollider->transformedVertices[0].z, sphereTraceMax(pTriangleCollider->transformedVertices[1].z, pTriangleCollider->transformedVertices[2].z)));

	pTriangleCollider->collider.aabb.lowExtent = sphereTraceVector3Construct(sphereTraceMin(pTriangleCollider->transformedVertices[0].x, sphereTraceMin(pTriangleCollider->transformedVertices[1].x, pTriangleCollider->transformedVertices[2].x)),
		sphereTraceMin(pTriangleCollider->transformedVertices[0].y, sphereTraceMin(pTriangleCollider->transformedVertices[1].y, pTriangleCollider->transformedVertices[2].y)),
		sphereTraceMin(pTriangleCollider->transformedVertices[0].z, sphereTraceMin(pTriangleCollider->transformedVertices[1].z, pTriangleCollider->transformedVertices[2].z)));

	pTriangleCollider->collider.aabb.halfExtents = sphereTraceVector3Subtract(pTriangleCollider->collider.aabb.highExtent,
		sphereTraceVector3Average(pTriangleCollider->collider.aabb.highExtent, pTriangleCollider->collider.aabb.lowExtent));

	pTriangleCollider->collider.aabb.center = sphereTraceVector3Average(pTriangleCollider->collider.aabb.lowExtent, pTriangleCollider->collider.aabb.highExtent);

	//pTriangleCollider->collider.boundingRadius = sphereTraceMax(sphereTraceMax(pTriangleCollider->vertexDists[0], pTriangleCollider->vertexDists[1]), pTriangleCollider->vertexDists[2]);
}

void sphereTraceColliderTriangleSetVertexAndEdgeData(ST_TriangleCollider* const pTriangleCollider, ST_Vector3 v1, ST_Vector3 v2, ST_Vector3 v3)
{
	pTriangleCollider->transformedVertices[0] = v1;
	pTriangleCollider->transformedVertices[1] = v2;
	pTriangleCollider->transformedVertices[2] = v3;
	pTriangleCollider->centroid = sphereTraceVector3Construct((v1.x + v2.x + v3.x) / 3.0f, (v1.y + v2.y + v3.y) / 3.0f, (v1.z + v2.z + v3.z) / 3.0f);
	pTriangleCollider->normal = sphereTraceDirectionConstructNormalized(sphereTraceVector3Cross(sphereTraceVector3Subtract(v3, v2), sphereTraceVector3Subtract(v1, v2)));
	pTriangleCollider->transformedEdges[0] = sphereTraceEdgeConstruct(v3, v1);
	pTriangleCollider->transformedEdges[1] = sphereTraceEdgeConstruct(v1, v2);
	pTriangleCollider->transformedEdges[2] = sphereTraceEdgeConstruct(v2, v3);
	pTriangleCollider->vertexDirs[0] = sphereTraceDirectionConstructNormalized(sphereTraceVector3Subtract(v1, pTriangleCollider->centroid));
	pTriangleCollider->vertexDirs[1] = sphereTraceDirectionConstructNormalized(sphereTraceVector3Subtract(v2, pTriangleCollider->centroid));
	pTriangleCollider->vertexDirs[2] = sphereTraceDirectionConstructNormalized(sphereTraceVector3Subtract(v3, pTriangleCollider->centroid));
	pTriangleCollider->vertexDists[0] = sphereTraceVector3Length(sphereTraceVector3Subtract(v1, pTriangleCollider->centroid));
	pTriangleCollider->vertexDists[1] = sphereTraceVector3Length(sphereTraceVector3Subtract(v2, pTriangleCollider->centroid));
	pTriangleCollider->vertexDists[2] = sphereTraceVector3Length(sphereTraceVector3Subtract(v3, pTriangleCollider->centroid));
	pTriangleCollider->edgeOrthogonalDirs[0] = sphereTraceDirectionConstructNormalized(sphereTraceVector3Cross(pTriangleCollider->transformedEdges[0].dir.v, pTriangleCollider->normal.v));
	pTriangleCollider->edgeOrthogonalDirs[1] = sphereTraceDirectionConstructNormalized(sphereTraceVector3Cross(pTriangleCollider->transformedEdges[1].dir.v, pTriangleCollider->normal.v));
	pTriangleCollider->edgeOrthogonalDirs[2] = sphereTraceDirectionConstructNormalized(sphereTraceVector3Cross(pTriangleCollider->transformedEdges[2].dir.v, pTriangleCollider->normal.v));
	if (v1.y <= v2.y)
	{
		if (v1.y <= v3.y)
		{
			pTriangleCollider->lowestVertIndex = 0;
			if(v2.y>=v3.y)
				pTriangleCollider->highestVertIndex = 1;
			else
				pTriangleCollider->highestVertIndex = 2;
		}
		else
		{
			pTriangleCollider->lowestVertIndex = 2;
			if (v1.y >= v2.y)
				pTriangleCollider->highestVertIndex = 0;
			else
				pTriangleCollider->highestVertIndex = 1;
		}
	}
	else
	{
		if (v2.y <= v3.y)
		{
			pTriangleCollider->lowestVertIndex = 1;
			if (v1.y >= v3.y)
				pTriangleCollider->highestVertIndex = 0;
			else
				pTriangleCollider->highestVertIndex = 2;
		}
		else
		{
			pTriangleCollider->lowestVertIndex = 2;
			if (v1.y >= v2.y)
				pTriangleCollider->highestVertIndex = 0;
			else
				pTriangleCollider->highestVertIndex = 1;
		}
	}
	pTriangleCollider->circularRadius = sphereTraceVector3HorizontalDistance(v1, pTriangleCollider->centroid);
	pTriangleCollider->circularRadius = sphereTraceMax(pTriangleCollider->circularRadius, sphereTraceVector3HorizontalDistance(v2, pTriangleCollider->centroid));
	pTriangleCollider->circularRadius = sphereTraceMax(pTriangleCollider->circularRadius, sphereTraceVector3HorizontalDistance(v3, pTriangleCollider->centroid));
}

void sphereTraceColliderTriangleUpdateVertexData(ST_TriangleCollider* const pTriangleCollider, ST_Vector3 v1, ST_Vector3 v2, ST_Vector3 v3)
{
	sphereTraceColliderTriangleSetVertexAndEdgeData(pTriangleCollider, v1, v2, v3);
	pTriangleCollider->collider.boundingRadius = sphereTraceMax(sphereTraceMax(pTriangleCollider->vertexDists[0], 
		pTriangleCollider->vertexDists[1]), pTriangleCollider->vertexDists[2]);
	sphereTraceColliderTriangleSetAABB(pTriangleCollider);
}

void sphereTraceColliderTriangleTranslate(ST_TriangleCollider* const pTriangleCollider, ST_Vector3 translation)
{
	sphereTraceVector3AddByRef(&pTriangleCollider->centroid, translation);
	for (int i = 0; i < 3; i++)
	{
		sphereTraceVector3AddByRef(&pTriangleCollider->transformedVertices[i], translation);
		sphereTraceEdgeTranslate(&pTriangleCollider->transformedEdges[i], translation);
	}
	//ST_Vector3 centroidTranslation = sphereTraceVector3Subtract(pTriangleCollider)
	sphereTraceAABBTranslate(&pTriangleCollider->collider.aabb, translation);
	//pTriangleCollider->collider.aabb = sphereTraceAABBConstruct2(pTriangleCollider->centroid, pTriangleCollider->collider.aabb.halfExtents);
}

void sphereTraceColliderTriangleSetPosition(ST_TriangleCollider* const pTriangleCollider, ST_Vector3 position)
{
	ST_Vector3 translation = sphereTraceVector3Subtract(position, pTriangleCollider->centroid);
	sphereTraceColliderTriangleTranslate(pTriangleCollider, translation);
}


b32 sphereTraceColliderTriangleIsProjectedPointContained(ST_Vector3 projectedPoint, const ST_TriangleCollider* const pTriangleCollider)
{
	float dot1 = sphereTraceVector3Dot(pTriangleCollider->normal.v, sphereTraceVector3Cross(sphereTraceVector3Subtract(pTriangleCollider->transformedVertices[0], pTriangleCollider->transformedVertices[2]),
		sphereTraceVector3Subtract(projectedPoint, pTriangleCollider->transformedVertices[2])));
	float dot2 = sphereTraceVector3Dot(pTriangleCollider->normal.v, sphereTraceVector3Cross(sphereTraceVector3Subtract(pTriangleCollider->transformedVertices[1], pTriangleCollider->transformedVertices[0]),
		sphereTraceVector3Subtract(projectedPoint, pTriangleCollider->transformedVertices[0])));
	float dot3 = sphereTraceVector3Dot(pTriangleCollider->normal.v, sphereTraceVector3Cross(sphereTraceVector3Subtract(pTriangleCollider->transformedVertices[2], pTriangleCollider->transformedVertices[1]),
		sphereTraceVector3Subtract(projectedPoint, pTriangleCollider->transformedVertices[1])));
	dot1 = copysignf(1.0f, dot1);
	dot2 = copysignf(1.0f, dot2);
	dot3 = copysignf(1.0f, dot3);
	if ((dot1 == dot2) && (dot1 == dot3))
	{
		return 1;
	}
	return 0;
}

b32 sphereTraceColliderTriangleRayTrace(ST_Vector3 from, ST_Direction dir, const ST_TriangleCollider* const pTriangleCollider, ST_RayTraceData* const pRaycastData)
{
	sphereTraceDirectionNormalizeIfNotNormalizedByRef(&dir);
	sphereTraceColliderInfinitePlaneRayTrace(from, dir, pTriangleCollider->normal, pTriangleCollider->centroid, pRaycastData);
	if (pRaycastData->distance >= 0.0f)
	{
		if (fpclassify(pRaycastData->distance) == FP_INFINITE)
			return 0;
		pRaycastData->startPoint = from;
		pRaycastData->contact.point = sphereTraceVector3Add(from, sphereTraceVector3Scale(dir.v, pRaycastData->distance));
		pRaycastData->pOtherCollider = pTriangleCollider;

		return sphereTraceColliderTriangleIsProjectedPointContained(pRaycastData->contact.point, pTriangleCollider);

	}
	return 0;
}

b32 sphereTraceColliderTriangleSphereTrace(ST_Vector3 from, ST_Direction dir, float radius, ST_TriangleCollider* const pTriangleCollider, ST_SphereTraceData* const pSphereTraceData)
{
	sphereTraceDirectionNormalizeIfNotNormalizedByRef(&dir);
	if (sphereTraceColliderInfinitePlaneSphereTrace(from, dir, radius, pTriangleCollider->centroid, pTriangleCollider->normal, pSphereTraceData))
	{
		if (sphereTraceColliderTriangleIsProjectedPointContained(pSphereTraceData->rayTraceData.contact.point, pTriangleCollider))
		{
			pSphereTraceData->rayTraceData.pOtherCollider = pTriangleCollider;
			//pSphereTraceData->rayTraceData.otherColliderType = COLLIDER_TRIANGLE;
			return 1;
		}
	}
	ST_SphereTraceData datTest;
	float dot = sphereTraceVector3Dot(dir.v, pTriangleCollider->normal.v);
	if (dot<ST_COLLIDER_TOLERANCE)
	{
		int edgeInd = sphereTraceColliderTriangleGetClosestTransformedEdgeIndexToPoint(pTriangleCollider, pSphereTraceData->rayTraceData.contact.point);
		if (sphereTraceColliderEdgeSphereTrace(from, dir, radius, &pTriangleCollider->transformedEdges[edgeInd], &datTest))
		{
			*pSphereTraceData = datTest;
			pSphereTraceData->rayTraceData.pOtherCollider = pTriangleCollider;
			//pSphereTraceData->rayTraceData.otherColliderType = COLLIDER_TRIANGLE;
			return 1;
		}
		return 0;
	}
	else
	{
		float closestEdgePointDist = FLT_MAX;
		if (sphereTraceColliderEdgeSphereTrace(from, dir, radius, &pTriangleCollider->transformedEdges[0], &datTest))
		{
			closestEdgePointDist = datTest.traceDistance;
			*pSphereTraceData = datTest;
		}
		if (sphereTraceColliderEdgeSphereTrace(from, dir, radius, &pTriangleCollider->transformedEdges[1], &datTest))
		{
			if (datTest.traceDistance < closestEdgePointDist)
			{
				closestEdgePointDist = datTest.traceDistance;
				*pSphereTraceData = datTest;
			}
		}
		if (sphereTraceColliderEdgeSphereTrace(from, dir, radius, &pTriangleCollider->transformedEdges[2], &datTest))
		{
			if (datTest.traceDistance < closestEdgePointDist)
			{
				closestEdgePointDist = datTest.traceDistance;
				*pSphereTraceData = datTest;
			}
		}
		if (closestEdgePointDist < FLT_MAX)
		{
			pSphereTraceData->rayTraceData.pOtherCollider = pTriangleCollider;
			//pSphereTraceData->rayTraceData.otherColliderType = COLLIDER_TRIANGLE;
			return 1;
		}
	}
	return 0;
}

b32 sphereTraceColliderTriangleSphereTraceOut(ST_Vector3 spherePos, float sphereRadius, ST_Direction clipoutDir, ST_TriangleCollider* const pTriangleCollider, ST_SphereTraceData* const pSphereCastData)
{
	sphereTraceDirectionNormalizeIfNotNormalizedByRef(&clipoutDir);
	ST_Vector3 castPoint = sphereTraceVector3AddAndScale(spherePos, clipoutDir.v, 2 * pTriangleCollider->collider.boundingRadius + 2 * sphereRadius);
	return sphereTraceColliderTriangleSphereTrace(castPoint, sphereTraceDirectionNegative(clipoutDir), sphereRadius, pTriangleCollider, pSphereCastData);
}