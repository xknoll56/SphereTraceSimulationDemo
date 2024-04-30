#include "SphereTraceColliderPlane.h"
#include "SphereTraceGlobals.h"

ST_PlaneCollider sphereTraceColliderPlaneConstruct(ST_Vector3 normalDir, float angle, float xHalfExtent, float zHalfExtent, ST_Vector3 position)
{
	ST_PlaneCollider planeCollider;
	planeCollider.collider = sphereTraceColliderConstruct(COLLIDER_PLANE, sqrtf(xHalfExtent * xHalfExtent + zHalfExtent * zHalfExtent));
	planeCollider.position = position;
	planeCollider.normal = sphereTraceDirectionConstructNormalized(normalDir);
	float dot = sphereTraceVector3Dot(planeCollider.normal.v, gVector3Forward);
	if (dot == 1.0f)
	{
		planeCollider.right = sphereTraceDirectionConstructNormalized(gVector3Right);
	}
	else if (dot == -1.0f)
	{
		planeCollider.right = sphereTraceDirectionConstructNormalized(sphereTraceVector3Negative(gVector3Right));
	}
	else
		planeCollider.right = sphereTraceDirectionConstructNormalized(sphereTraceVector3Cross(planeCollider.normal.v, gVector3Forward));
	ST_Quaternion rotation = sphereTraceQuaternionFromAngleAxis(planeCollider.normal.v, angle);
	planeCollider.right = sphereTraceDirectionConstruct(sphereTraceVector3RotatePoint(planeCollider.right.v, rotation), 1);
	planeCollider.forward = sphereTraceDirectionConstruct(sphereTraceVector3Cross(planeCollider.right.v, planeCollider.normal.v), 1);
	planeCollider.xHalfExtent = xHalfExtent;
	planeCollider.zHalfExtent = zHalfExtent;
	ST_Matrix4 rotMat = sphereTraceMatrixConstructFromRightForwardUp(planeCollider.right.v, planeCollider.normal.v, planeCollider.forward.v);
	planeCollider.rotation = sphereTraceQuaternionNormalize(sphereTraceMatrixQuaternionFromRotationMatrix(rotMat));

	//retreive the adjusted rotation matrix, matrix from quaternion is not perfect so the directional vectors change slightly
	rotMat = sphereTraceMatrixFromQuaternion(planeCollider.rotation);
	planeCollider.right = sphereTraceDirectionConstruct(sphereTraceVector3GetLocalXAxisFromRotationMatrix(rotMat), 1);
	planeCollider.normal = sphereTraceDirectionConstruct(sphereTraceVector3GetLocalYAxisFromRotationMatrix(rotMat), 1);
	planeCollider.forward = sphereTraceDirectionConstruct(sphereTraceVector3GetLocalZAxisFromRotationMatrix(rotMat), 1);
	sphereTraceColliderPlaneSetTransformedVerticesAndEdges(&planeCollider);
	sphereTraceColliderPlaneSetAABB(&planeCollider);
	//sphereTraceColliderPlaneSetAABBExtents(&planeCollider);
	//sphereTraceColliderPlaneAABBSetTransformedVertices(&planeCollider);
	//planeCollider.collider.aabb.center = planeCollider.position;
	//planeCollider.collider.colliderType = COLLIDER_PLANE;
	return planeCollider;
}

void sphereTraceColliderPlaneSetTransformWithRotationMatrix(ST_PlaneCollider* const pPlaneCollider, ST_Matrix4 rotMat, float xHalfExtent, float zHalfExtent, ST_Vector3 position)
{
	pPlaneCollider->position = position;
	pPlaneCollider->xHalfExtent = xHalfExtent;
	pPlaneCollider->zHalfExtent = zHalfExtent;
	pPlaneCollider->right = sphereTraceDirectionConstruct(sphereTraceVector3GetLocalXAxisFromRotationMatrix(rotMat), 0);
	pPlaneCollider->normal = sphereTraceDirectionConstruct(sphereTraceVector3GetLocalYAxisFromRotationMatrix(rotMat), 0);
	pPlaneCollider->forward = sphereTraceDirectionConstruct(sphereTraceVector3GetLocalZAxisFromRotationMatrix(rotMat), 0);
	pPlaneCollider->rotation = sphereTraceMatrixQuaternionFromRotationMatrix(rotMat);
	sphereTraceColliderPlaneSetTransformedVerticesAndEdges(pPlaneCollider);
	sphereTraceColliderPlaneSetAABB(pPlaneCollider);
	pPlaneCollider->collider.boundingRadius = sqrtf(xHalfExtent * xHalfExtent + zHalfExtent * zHalfExtent);
}

ST_PlaneCollider sphereTraceColliderPlaneConstructWithRotationMatrix(ST_Matrix4 rotMat, float xHalfExtent, float zHalfExtent, ST_Vector3 position)
{
	ST_PlaneCollider planeCollider;
	planeCollider.collider = sphereTraceColliderConstruct(COLLIDER_PLANE, sqrtf(xHalfExtent * xHalfExtent + zHalfExtent * zHalfExtent));
	planeCollider.position = position;
	planeCollider.xHalfExtent = xHalfExtent;
	planeCollider.zHalfExtent = zHalfExtent;
	planeCollider.right = sphereTraceDirectionConstruct(sphereTraceVector3GetLocalXAxisFromRotationMatrix(rotMat), 0);
	planeCollider.normal = sphereTraceDirectionConstruct(sphereTraceVector3GetLocalYAxisFromRotationMatrix(rotMat), 0);
	planeCollider.forward = sphereTraceDirectionConstruct(sphereTraceVector3GetLocalZAxisFromRotationMatrix(rotMat), 0);
	planeCollider.rotation = sphereTraceMatrixQuaternionFromRotationMatrix(rotMat);
	sphereTraceColliderPlaneSetTransformedVerticesAndEdges(&planeCollider);
	sphereTraceColliderPlaneSetAABB(&planeCollider);
	//sphereTraceColliderPlaneSetAABBExtents(&planeCollider);
	//sphereTraceColliderPlaneAABBSetTransformedVertices(&planeCollider);
	//planeCollider.collider.colliderType = COLLIDER_PLANE;
	return planeCollider;
}


void sphereTraceColliderPlaneSetTransformedVerticesAndEdges(ST_PlaneCollider* const pPlaneCollider)
{
	//forward right
	pPlaneCollider->transformedVertices[PLANE_VEREX_FORWARD_RIGHT] = sphereTraceVector3Add(sphereTraceVector3Add(pPlaneCollider->position, sphereTraceVector3Scale(pPlaneCollider->right.v, pPlaneCollider->xHalfExtent)), sphereTraceVector3Scale(pPlaneCollider->forward.v, pPlaneCollider->zHalfExtent));
	//forward left
	pPlaneCollider->transformedVertices[PLANE_VEREX_FORWARD_LEFT] = sphereTraceVector3Add(sphereTraceVector3Add(pPlaneCollider->position, sphereTraceVector3Scale(pPlaneCollider->right.v, -pPlaneCollider->xHalfExtent)), sphereTraceVector3Scale(pPlaneCollider->forward.v, pPlaneCollider->zHalfExtent));
	//back left
	pPlaneCollider->transformedVertices[PLANE_VEREX_BACK_LEFT] = sphereTraceVector3Add(sphereTraceVector3Add(pPlaneCollider->position, sphereTraceVector3Scale(pPlaneCollider->right.v, -pPlaneCollider->xHalfExtent)), sphereTraceVector3Scale(pPlaneCollider->forward.v, -pPlaneCollider->zHalfExtent));
	//back right
	pPlaneCollider->transformedVertices[PLANE_VEREX_BACK_RIGHT] = sphereTraceVector3Add(sphereTraceVector3Add(pPlaneCollider->position, sphereTraceVector3Scale(pPlaneCollider->right.v, pPlaneCollider->xHalfExtent)), sphereTraceVector3Scale(pPlaneCollider->forward.v, -pPlaneCollider->zHalfExtent));

	//right edge
	pPlaneCollider->transformedEdges[PLANE_EDGE_RIGHT] = sphereTraceEdgeConstruct(pPlaneCollider->transformedVertices[3], pPlaneCollider->transformedVertices[0]);
	//forward edge
	pPlaneCollider->transformedEdges[PLANE_EDGE_FORWARD] = sphereTraceEdgeConstruct(pPlaneCollider->transformedVertices[0], pPlaneCollider->transformedVertices[1]);
	//left edge
	pPlaneCollider->transformedEdges[PLANE_EDGE_LEFT] = sphereTraceEdgeConstruct(pPlaneCollider->transformedVertices[1], pPlaneCollider->transformedVertices[2]);
	//back edge
	pPlaneCollider->transformedEdges[PLANE_EDGE_BACK] = sphereTraceEdgeConstruct(pPlaneCollider->transformedVertices[2], pPlaneCollider->transformedVertices[3]);

}

//ST_PlaneEdgeDirection sphereTraceColliderPlaneGetClosestTransformedEdgeToPoint(const ST_PlaneCollider* const pPlaneCollider, ST_Vector3 point)
//{
//	ST_Vector3 distVec = sphereTraceVector3Subtract(point, pPlaneCollider->position);
//	float xDist = sphereTraceVector3Dot(distVec, pPlaneCollider->right.v) / pPlaneCollider->xHalfExtent;
//	float zDist = sphereTraceVector3Dot(distVec, pPlaneCollider->forward.v) / pPlaneCollider->zHalfExtent;
//
//	if (xDist > 0.0f)
//	{
//		if (sphereTraceAbs(zDist) > xDist)
//		{
//			if (zDist > 0.0f)
//			{
//				//return forward edge
//				return PLANE_EDGE_FORWARD;
//			}
//			else
//			{
//				//return back edge
//				return PLANE_EDGE_BACK;
//			}
//		}
//		else
//		{
//			//return right edge
//			return PLANE_EDGE_RIGHT;
//		}
//	}
//	else
//	{
//		if (sphereTraceAbs(zDist) > -xDist)
//		{
//			if (zDist > 0.0f)
//			{
//				//return forward edge
//				return PLANE_EDGE_FORWARD;
//			}
//			else
//			{
//				//return back edge
//				return PLANE_EDGE_BACK;
//			}
//		}
//		else
//		{
//			//return left edge
//			return PLANE_EDGE_LEFT;
//		}
//	}
//}

ST_PlaneEdgeDirection sphereTraceColliderPlaneGetClosestTransformedEdgeToPoint(const ST_PlaneCollider* const pPlaneCollider, ST_Vector3 point)
{
	ST_Vector3 distVec = sphereTraceVector3Subtract(point, pPlaneCollider->position);
	float xDist = sphereTraceVector3Dot(distVec, pPlaneCollider->right.v) / pPlaneCollider->xHalfExtent;
	float zDist = sphereTraceVector3Dot(distVec, pPlaneCollider->forward.v) / pPlaneCollider->zHalfExtent;

	if (xDist > 0.0f)
	{
		if (sphereTraceAbs(zDist) > xDist)
		{
			if (zDist > 0.0f)
			{
				//return forward edge
				return PLANE_EDGE_FORWARD;
			}
			else
			{
				//return back edge
				return PLANE_EDGE_BACK;
			}
		}
		else
		{
			//return right edge
			return PLANE_EDGE_RIGHT;
		}
	}
	else
	{
		if (sphereTraceAbs(zDist) > -xDist)
		{
			if (zDist > 0.0f)
			{
				//return forward edge
				return PLANE_EDGE_FORWARD;
			}
			else
			{
				//return back edge
				return PLANE_EDGE_BACK;
			}
		}
		else
		{
			//return left edge
			return PLANE_EDGE_LEFT;
		}
	}
}

ST_Direction sphereTraceColliderPlaneEdgeDirectionToDirection(const ST_PlaneCollider* const pPlaneCollider, ST_PlaneEdgeDirection dir)
{
	switch (dir)
	{
	case PLANE_EDGE_FORWARD:
		return pPlaneCollider->forward;
	case PLANE_EDGE_RIGHT:
		return pPlaneCollider->right;
	case PLANE_EDGE_BACK:
		return sphereTraceDirectionNegative(pPlaneCollider->forward);
	case PLANE_EDGE_LEFT:
		return sphereTraceDirectionNegative(pPlaneCollider->right);
	}
}

ST_Direction sphereTraceColliderImposedPlaneEdgeDirectionToDirection(const ST_Direction planeRight, const ST_Direction planeForward, ST_PlaneEdgeDirection dir)
{
	switch (dir)
	{
	case PLANE_EDGE_FORWARD:
		return planeForward;
	case PLANE_EDGE_RIGHT:
		return planeRight;
	case PLANE_EDGE_BACK:
		return sphereTraceDirectionNegative(planeForward);
	case PLANE_EDGE_LEFT:
		return sphereTraceDirectionNegative(planeRight);
	}
}

ST_PlaneEdgeDirection sphereTraceColliderPlaneGetClosestTransformedEdgeToSphereTrace(const ST_PlaneCollider* const pPlaneCollider, ST_Vector3 point, ST_Vector3 dir, float radius, ST_Vector3* closestPoint)
{
	float sForward = sphereTraceVector3Dot(dir, pPlaneCollider->forward.v);
	float sRight = sphereTraceVector3Dot(dir, pPlaneCollider->right.v);
	float s1, t1, s2, t2;
	float u1, v1, u2, v2;
	float dt1, dt2;
	ST_Vector3 right, fwd;
	ST_Vector3 start;
	ST_Vector3 closestPoint1, closestPoint2;
	ST_PlaneEdgeDirection e1, e2;
	if (sForward < 0.0f)
	{
		right = sphereTraceVector3Normalize(sphereTraceVector3Cross(dir, pPlaneCollider->forward.v));
		fwd = sphereTraceVector3Cross(dir, right);
		float rightDist = sphereTraceVector3Dot(right, pPlaneCollider->transformedEdges[PLANE_EDGE_FORWARD].point1);
		float theta = acosf(sphereTraceAbs(rightDist) / radius);
		start = sphereTraceVector3AddAndScale(point, fwd, radius * sinf(theta));
		ST_Vector3 edgeDir = sphereTraceVector3Normalize(sphereTraceVector3Subtract(pPlaneCollider->transformedEdges[PLANE_EDGE_FORWARD].point2, pPlaneCollider->transformedEdges[PLANE_EDGE_FORWARD].point1));
		sphereTraceVector3ClosestPointOnLineBetweenTwoLinesDistancesOnLines(start, dir, pPlaneCollider->transformedEdges[PLANE_EDGE_FORWARD].point1, edgeDir, &s1, &t1);
		sphereTraceVector3ClosestPointOnLineBetweenTwoLinesDistancesOnLines(point, dir, pPlaneCollider->transformedEdges[PLANE_EDGE_FORWARD].point1, edgeDir, &u1, &v1);
		float eMin, eMax;
		if (t1 > v1)
		{
			eMin = v1;
			eMax = t1;
		}
		else
		{
			eMin = t1;
			eMax = v1;
		}
		if (t1 >= pPlaneCollider->zHalfExtent)
		{
			if (eMin > pPlaneCollider->zHalfExtent * 2.0f)
				dt1 = FLT_MAX;
			else
				dt1 = pPlaneCollider->zHalfExtent * 2.0f - t1;
			closestPoint1 = pPlaneCollider->transformedEdges[PLANE_EDGE_FORWARD].point2;
		}
		else
		{
			if (eMax < 0.0f)
				dt1 = FLT_MAX;
			else
				dt1 = t1;
			closestPoint1 = pPlaneCollider->transformedEdges[PLANE_EDGE_FORWARD].point1;
		}
		e1 = PLANE_EDGE_FORWARD;
	}
	else
	{
		right = sphereTraceVector3Normalize(sphereTraceVector3Cross(dir, sphereTraceVector3Negative(pPlaneCollider->forward.v)));
		fwd = sphereTraceVector3Cross(dir, right);
		float rightDist = sphereTraceVector3Dot(right, pPlaneCollider->transformedEdges[PLANE_EDGE_BACK].point1);
		float theta = acosf(sphereTraceAbs(rightDist) / radius);
		start = sphereTraceVector3AddAndScale(point, fwd, radius * sinf(theta));
		ST_Vector3 edgeDir = sphereTraceVector3Normalize(sphereTraceVector3Subtract(pPlaneCollider->transformedEdges[PLANE_EDGE_BACK].point2, pPlaneCollider->transformedEdges[PLANE_EDGE_BACK].point1));
		sphereTraceVector3ClosestPointOnLineBetweenTwoLinesDistancesOnLines(start, dir, pPlaneCollider->transformedEdges[PLANE_EDGE_BACK].point1, edgeDir, &s1, &t1);
		sphereTraceVector3ClosestPointOnLineBetweenTwoLinesDistancesOnLines(point, dir, pPlaneCollider->transformedEdges[PLANE_EDGE_BACK].point1, edgeDir, &u1, &v1);
		float eMin, eMax;
		if (t1 > v1)
		{
			eMin = v1;
			eMax = t1;
		}
		else
		{
			eMin = t1;
			eMax = v1;
		}
		if (t1 >= pPlaneCollider->zHalfExtent)
		{
			if (eMin > pPlaneCollider->zHalfExtent * 2.0f)
				dt1 = FLT_MAX;
			else
				dt1 = pPlaneCollider->zHalfExtent * 2.0f - t1;
			closestPoint1 = pPlaneCollider->transformedEdges[PLANE_EDGE_BACK].point2;
		}
		else
		{
			if (eMax < 0.0f)
				dt1 = FLT_MAX;
			else
				dt1 = t1;
			closestPoint1 = pPlaneCollider->transformedEdges[PLANE_EDGE_BACK].point1;
		}
		e1 = PLANE_EDGE_BACK;
	}
	if (sRight < 0.0f)
	{
		right = sphereTraceVector3Normalize(sphereTraceVector3Cross(dir, pPlaneCollider->right.v));
		fwd = sphereTraceVector3Cross(dir, right);
		float rightDist = sphereTraceVector3Dot(right, pPlaneCollider->transformedEdges[PLANE_EDGE_RIGHT].point1);
		float theta = acosf(sphereTraceAbs(rightDist) / radius);
		start = sphereTraceVector3AddAndScale(point, fwd, radius * sinf(theta));
		ST_Vector3 edgeDir = sphereTraceVector3Normalize(sphereTraceVector3Subtract(pPlaneCollider->transformedEdges[PLANE_EDGE_RIGHT].point2, pPlaneCollider->transformedEdges[PLANE_EDGE_RIGHT].point1));
		sphereTraceVector3ClosestPointOnLineBetweenTwoLinesDistancesOnLines(start, dir, pPlaneCollider->transformedEdges[PLANE_EDGE_RIGHT].point1, edgeDir, &s2, &t2);
		sphereTraceVector3ClosestPointOnLineBetweenTwoLinesDistancesOnLines(point, dir, pPlaneCollider->transformedEdges[PLANE_EDGE_RIGHT].point1, edgeDir, &u2, &v2);
		float eMin, eMax;
		if (t2 > v2)
		{
			eMin = v2;
			eMax = t2;
		}
		else
		{
			eMin = t2;
			eMax = v2;
		}
		if (t2 >= pPlaneCollider->xHalfExtent)
		{
			if (eMin > pPlaneCollider->xHalfExtent * 2.0f)
				dt2 = FLT_MAX;
			else
				dt2 = pPlaneCollider->xHalfExtent * 2.0f - t2;
			closestPoint2 = pPlaneCollider->transformedEdges[PLANE_EDGE_RIGHT].point2;
		}
		else
		{
			if (eMax < 0.0f)
				dt2 = FLT_MAX;
			else
				dt2 = t2;
			closestPoint2 = pPlaneCollider->transformedEdges[PLANE_EDGE_RIGHT].point1;
		}
		e2 = PLANE_EDGE_RIGHT;
	}
	else
	{
		right = sphereTraceVector3Normalize(sphereTraceVector3Cross(dir, sphereTraceVector3Negative(pPlaneCollider->right.v)));
		fwd = sphereTraceVector3Cross(dir, right);
		float rightDist = sphereTraceVector3Dot(right, pPlaneCollider->transformedEdges[PLANE_EDGE_LEFT].point1);
		float theta = acosf(sphereTraceAbs(rightDist) / radius);
		start = sphereTraceVector3AddAndScale(point, fwd, radius * sinf(theta));
		ST_Vector3 edgeDir = sphereTraceVector3Normalize(sphereTraceVector3Subtract(pPlaneCollider->transformedEdges[PLANE_EDGE_LEFT].point2, pPlaneCollider->transformedEdges[PLANE_EDGE_LEFT].point1));
		sphereTraceVector3ClosestPointOnLineBetweenTwoLinesDistancesOnLines(start, dir, pPlaneCollider->transformedEdges[PLANE_EDGE_LEFT].point1, edgeDir, &s2, &t2);
		sphereTraceVector3ClosestPointOnLineBetweenTwoLinesDistancesOnLines(point, dir, pPlaneCollider->transformedEdges[PLANE_EDGE_LEFT].point1, edgeDir, &u2, &v2);
		float eMin, eMax;
		if (t2 > v2)
		{
			eMin = v2;
			eMax = t2;
		}
		else
		{
			eMin = t2;
			eMax = v2;
		}
		if (t2 >= pPlaneCollider->xHalfExtent)
		{
			if (eMin > pPlaneCollider->xHalfExtent * 2.0f)
				dt2 = FLT_MAX;
			else
				dt2 = pPlaneCollider->xHalfExtent * 2.0f - t2;
			closestPoint2 = pPlaneCollider->transformedEdges[PLANE_EDGE_LEFT].point2;
		}
		else
		{
			if (eMax < 0.0f)
				dt2 = FLT_MAX;
			else
				dt2 = t2;
			closestPoint2 = pPlaneCollider->transformedEdges[PLANE_EDGE_LEFT].point1;
		}
		e2 = PLANE_EDGE_LEFT;
	}

	if (dt1 <= dt2)
	{
		*closestPoint = closestPoint1;
		return e1;
	}
	else
	{
		*closestPoint = closestPoint2;
		return e2;
	}
}

ST_PlaneVertexDirection sphereTraceColliderPlaneGetClosestTransformedVertexToPoint(const ST_PlaneCollider* const pPlaneCollider, ST_Vector3 point)
{
	ST_Vector3 distVec = sphereTraceVector3Subtract(point, pPlaneCollider->position);
	float xDist = sphereTraceVector3Dot(distVec, pPlaneCollider->right.v);
	float zDist = sphereTraceVector3Dot(distVec, pPlaneCollider->forward.v);
	if (xDist > 0.0f)
	{
		if (zDist > 0.0f)
		{
			//return forward right
			return PLANE_VEREX_FORWARD_RIGHT;
		}
		else
		{
			//return back right
			return  PLANE_VEREX_BACK_RIGHT;
		}
	}
	else
	{
		if (zDist > 0.0f)
		{
			//return forward left
			return  PLANE_VEREX_FORWARD_LEFT;
		}
		else
		{
			//return back left
			return  PLANE_VEREX_BACK_LEFT;
		}
	}
}

void sphereTraceColliderPlaneTranslate(ST_PlaneCollider* const pPlaneCollider, ST_Vector3 translation)
{
	sphereTraceVector3AddByRef(&pPlaneCollider->position, translation);
	sphereTraceAABBTranslate(&pPlaneCollider->collider.aabb, translation);
	for (int i = 0; i < 4; i++)
	{
		sphereTraceVector3AddByRef(&pPlaneCollider->transformedVertices[i], translation);
		sphereTraceEdgeTranslate(&pPlaneCollider->transformedEdges[i], translation);
	}
}

//not the most efficient, not meant to be run every frame, pretty much same as calling the constructor
void sphereTraceColliderPlaneRotateAround(ST_PlaneCollider* const pPlaneCollider, ST_Vector3 point, ST_Quaternion rotation)
{
	pPlaneCollider->right = sphereTraceDirectionRotateDir(pPlaneCollider->right, rotation);
	pPlaneCollider->normal = sphereTraceDirectionRotateDir(pPlaneCollider->normal, rotation);
	pPlaneCollider->forward = sphereTraceDirectionRotateDir(pPlaneCollider->forward, rotation);
	ST_Vector3 relative = sphereTraceVector3Subtract(point, pPlaneCollider->position);
	pPlaneCollider->position = sphereTraceVector3Add(pPlaneCollider->position, sphereTraceVector3RotatePoint(relative, rotation));
	pPlaneCollider->rotation = sphereTraceQuaternionMultiply(rotation, pPlaneCollider->rotation);
	sphereTraceColliderPlaneSetTransformedVerticesAndEdges(pPlaneCollider);
	sphereTraceColliderPlaneSetAABB(pPlaneCollider);
}

void sphereTraceColliderPlaneSetPosition(ST_PlaneCollider* const pPlaneCollider, ST_Vector3 position)
{
	ST_Vector3 translation = sphereTraceVector3Subtract(position, pPlaneCollider->position);
	sphereTraceColliderPlaneTranslate(pPlaneCollider, translation);
}

void sphereTraceColliderPlaneSetRotation(ST_PlaneCollider* const pPlaneCollider, ST_Quaternion rotation)
{
	ST_Quaternion toRotation = sphereTraceQuaternionMultiply(pPlaneCollider->rotation, sphereTraceQuaternionConjugate(rotation));
	sphereTraceColliderPlaneRotateAround(pPlaneCollider, pPlaneCollider->position, toRotation);
}

void sphereTraceColliderPlaneSetRotationWithMatrix(ST_PlaneCollider* const pPlaneCollider, ST_Matrix4 rotMat)
{
	pPlaneCollider->right = sphereTraceDirectionConstruct(sphereTraceVector3GetLocalXAxisFromRotationMatrix(rotMat), 0);
	pPlaneCollider->normal = sphereTraceDirectionConstruct(sphereTraceVector3GetLocalYAxisFromRotationMatrix(rotMat), 0);
	pPlaneCollider->forward = sphereTraceDirectionConstruct(sphereTraceVector3GetLocalZAxisFromRotationMatrix(rotMat), 0);
	pPlaneCollider->rotation = sphereTraceMatrixQuaternionFromRotationMatrix(rotMat);
	sphereTraceColliderPlaneSetTransformedVerticesAndEdges(pPlaneCollider);
	sphereTraceColliderPlaneSetAABB(pPlaneCollider);
}


void sphereTraceColliderPlaneSetAABB(ST_PlaneCollider* const pPlaneCollider)
{
	sphereTraceColliderPlaneSetAABBExtents(pPlaneCollider);
	sphereTraceColliderPlaneAABBSetTransformedVertices(pPlaneCollider);
	pPlaneCollider->collider.aabb.center = pPlaneCollider->position;
}


void sphereTraceColliderPlaneSetAABBExtents(ST_PlaneCollider* const pPlaneCollider)
{
	//set aabb extents
	pPlaneCollider->collider.aabb.halfExtents.x = sphereTraceVector3Subtract(pPlaneCollider->transformedVertices[sphereTraceColliderPlaneGetClosestTransformedVertexToPoint(pPlaneCollider, sphereTraceVector3Add(pPlaneCollider->position, gVector3Right))], pPlaneCollider->position).x;
	pPlaneCollider->collider.aabb.halfExtents.y = sphereTraceVector3Subtract(pPlaneCollider->transformedVertices[sphereTraceColliderPlaneGetClosestTransformedVertexToPoint(pPlaneCollider, sphereTraceVector3Add(pPlaneCollider->position, gVector3Up))], pPlaneCollider->position).y;
	pPlaneCollider->collider.aabb.halfExtents.z = sphereTraceVector3Subtract(pPlaneCollider->transformedVertices[sphereTraceColliderPlaneGetClosestTransformedVertexToPoint(pPlaneCollider, sphereTraceVector3Add(pPlaneCollider->position, gVector3Forward))], pPlaneCollider->position).z;
}

void sphereTraceColliderPlaneAABBSetTransformedVertices(ST_PlaneCollider* const pPlaneCollider)
{
	pPlaneCollider->collider.aabb.highExtent = sphereTraceVector3Add(pPlaneCollider->position, pPlaneCollider->collider.aabb.halfExtents);
	pPlaneCollider->collider.aabb.lowExtent = sphereTraceVector3Subtract(pPlaneCollider->position, pPlaneCollider->collider.aabb.halfExtents);
}


void sphereTraceColliderInfinitePlaneRayTrace(ST_Vector3 from, ST_Direction dir, ST_Direction planeNormal, ST_Vector3 pointOnPlane, ST_RayTraceData* const pRaycastData)
{
	pRaycastData->startPoint = from;
	sphereTraceDirectionNormalizeIfNotNormalizedByRef(&dir);
	ST_Vector3 dp = sphereTraceVector3Subtract(pointOnPlane, from);
	pRaycastData->contact.normal = planeNormal;
	float dpDotNormal = sphereTraceVector3Dot(dp, pRaycastData->contact.normal.v);
	if (sphereTraceAbs(dpDotNormal)<ST_COLLIDER_TOLERANCE)
	{
		pRaycastData->contact.normal = planeNormal;
		pRaycastData->distance = 0.0f;
		pRaycastData->contact.point = pointOnPlane;
		return;
	}
	float dirDotNormal = sphereTraceVector3Dot(dir.v, planeNormal.v);
	pRaycastData->distance = dpDotNormal / dirDotNormal;
	pRaycastData->contact.point = sphereTraceVector3Add(from, sphereTraceVector3Scale(dir.v, pRaycastData->distance));
	//sphereTraceVector3NormalizeByRef(&dp);
	//if (sphereTraceVector3Dot(sphereTraceVector3Subtract(pRaycastData->contact.point, from), pRaycastData->contact.normal.v) > 0.0f)
	//{
	//	pRaycastData->contact.normal = sphereTraceDirectionNegative(planeNormal);
	//}
	if (dpDotNormal > 0.0f)
	{
		pRaycastData->contact.normal = sphereTraceDirectionNegative(planeNormal);
	}
}

b32 sphereTraceColliderInfinitePlaneEdgeTrace(ST_Edge* pEdge, ST_Direction dir, ST_Direction planeNormal, ST_Vector3 pointOnPlane, ST_EdgeTraceData* const pEdgeTraceData)
{
	sphereTraceDirectionNormalizeIfNotNormalizedByRef(&dir);
	ST_RayTraceData rtd1, rtd2;
	sphereTraceColliderInfinitePlaneRayTrace(pEdge->point1, dir, planeNormal, pointOnPlane, &rtd1);
	sphereTraceColliderInfinitePlaneRayTrace(pEdge->point2, dir, planeNormal, pointOnPlane, &rtd2);
	if (rtd1.distance > 0.0f && rtd2.distance > 0.0f)
	{
		float dot = sphereTraceVector3Dot(planeNormal.v, pEdge->dir.v);
		if (sphereTraceAbs(dot) < ST_COLLIDER_TOLERANCE)
		{
			*pEdgeTraceData = sphereTraceEdgeTraceDataFrom2RayTraceData(&rtd1, &rtd2);
			return ST_TRUE;
		}
		else
		{
			if (rtd1.distance < rtd2.distance)
			{
				*pEdgeTraceData = sphereTraceEdgeTraceDataFrom1RayTraceData(&rtd1);
				pEdgeTraceData->hitIndex = 0;
				return ST_TRUE;
			}
			else
			{
				*pEdgeTraceData = sphereTraceEdgeTraceDataFrom1RayTraceData(&rtd2);
				pEdgeTraceData->hitIndex = 1;
				return ST_TRUE;
			}
		}
	}
	else if (rtd1.distance > 0.0f)
	{
		sphereTraceColliderInfinitePlaneRayTrace(pEdge->point1, pEdge->dir, planeNormal, pointOnPlane, &rtd2);
		rtd2.distance = 0.0f;
		*pEdgeTraceData = sphereTraceEdgeTraceDataFrom1RayTraceData(&rtd2);
		return ST_TRUE;
	}
	else if (rtd2.distance > 0.0f)
	{
		sphereTraceColliderInfinitePlaneRayTrace(pEdge->point2, sphereTraceDirectionNegative(pEdge->dir), planeNormal, pointOnPlane, &rtd1);
		rtd1.distance = 0.0f;
		*pEdgeTraceData = sphereTraceEdgeTraceDataFrom1RayTraceData(&rtd1);
		return ST_TRUE;
	}
	return ST_FALSE;
}

void sphereTraceColliderInfiniteZPlaneRayTrace(ST_Vector3 from, ST_Direction dir, ST_Vector3 pointOnPlane, ST_RayTraceData* const pRaycastData)
{
	pRaycastData->startPoint = from;
	float dz = pointOnPlane.z - from.z;
	if (dz==0.0f)
	{
		pRaycastData->contact.normal = gDirectionForward;
		pRaycastData->distance = 0.0f;
		pRaycastData->contact.point = from;
		return;
	}
	else if (dir.v.z == 0.0f)
	{
		pRaycastData->contact.normal = gDirectionForward;
		pRaycastData->distance = FLT_MAX;
		pRaycastData->contact.point = gVector3Max;
		return;
	}
	sphereTraceDirectionNormalizeIfNotNormalizedByRef(&dir);
	pRaycastData->contact.normal = gDirectionForward;
	pRaycastData->directionType = ST_DIRECTION_FORWARD;
	//float dirDotNormal = sphereTraceVector3Dot(dir.v, gDirectionForward.v);
	pRaycastData->distance = dz / dir.v.z;
	pRaycastData->contact.point = sphereTraceVector3Add(from, sphereTraceVector3Scale(dir.v, pRaycastData->distance));
	if (dz>0.0f)
	{
		pRaycastData->contact.normal = gDirectionBack;
		pRaycastData->directionType = ST_DIRECTION_BACK;
	}
}
void sphereTraceColliderInfiniteYPlaneRayTrace(ST_Vector3 from, ST_Direction dir, ST_Vector3 pointOnPlane, ST_RayTraceData* const pRaycastData)
{
	pRaycastData->startPoint = from;
	float dy = pointOnPlane.y - from.y;
	if (dy == 0.0f)
	{
		pRaycastData->contact.normal = gDirectionUp;
		pRaycastData->distance = 0.0f;
		pRaycastData->contact.point = from;
		return;
	}
	else if (dir.v.y == 0.0f)
	{
		pRaycastData->contact.normal = gDirectionUp;
		pRaycastData->distance = FLT_MAX;
		pRaycastData->contact.point = gVector3Max;
		return;
	}
	sphereTraceDirectionNormalizeIfNotNormalizedByRef(&dir);
	pRaycastData->contact.normal = gDirectionUp;
	pRaycastData->directionType = ST_DIRECTION_UP;
	//float dirDotNormal = sphereTraceVector3Dot(dir.v, gDirectionUp.v);
	pRaycastData->distance = dy / dir.v.y;
	pRaycastData->contact.point = sphereTraceVector3Add(from, sphereTraceVector3Scale(dir.v, pRaycastData->distance));
	if (dy > 0.0f)
	{
		pRaycastData->contact.normal = gDirectionDown;
		pRaycastData->directionType = ST_DIRECTION_DOWN;
	}
}
void sphereTraceColliderInfiniteXPlaneRayTrace(ST_Vector3 from, ST_Direction dir, ST_Vector3 pointOnPlane, ST_RayTraceData* const pRaycastData)
{
	pRaycastData->startPoint = from;
	float dx = pointOnPlane.x - from.x;
	if (dx == 0.0f)
	{
		pRaycastData->contact.normal = gDirectionRight;
		pRaycastData->distance = 0.0f;
		pRaycastData->contact.point = from;
		return;
	}
	else if (dir.v.x == 0.0f)
	{
		pRaycastData->contact.normal = gDirectionRight;
		pRaycastData->distance = FLT_MAX;
		pRaycastData->contact.point = gVector3Max;
		return;
	}
	sphereTraceDirectionNormalizeIfNotNormalizedByRef(&dir);
	pRaycastData->contact.normal = gDirectionRight;
	pRaycastData->directionType = ST_DIRECTION_RIGHT;
	pRaycastData->distance = dx / dir.v.x;
	pRaycastData->contact.point = sphereTraceVector3Add(from, sphereTraceVector3Scale(dir.v, pRaycastData->distance));
	if (dx > 0.0f)
	{
		pRaycastData->contact.normal = gDirectionLeft;
		pRaycastData->directionType = ST_DIRECTION_LEFT;
	}
}

b32 sphereTraceColliderInfiniteZPlaneSphereTrace(ST_Vector3 from, ST_Direction dir, float radius, ST_Vector3 pointOnPlane, ST_SphereTraceData* const pData)
{
	pData->rayTraceData.startPoint = from;
	pData->radius = radius;
	float dz = pointOnPlane.z - from.z;
	if (sphereTraceAbs(dz) <= radius)
	{
		pData->traceDistance = 0.0f;
		pData->sphereCenter = from;
		pData->rayTraceData.distance = sphereTraceAbs(dz);
		pData->rayTraceData.contact.point = sphereTraceVector3Construct(from.x, from.y, pointOnPlane.z);
		if(dz<0.0f)
			pData->rayTraceData.contact.normal = gDirectionForward;
		else
			pData->rayTraceData.contact.normal = gDirectionBack;
		return 1;
	}
	else if (dir.v.z == 0.0f)
	{
		return 0;
	}
	if (dz > 0.0f)
	{
		dz -= radius;
	}
	else
	{
		dz += radius;
	}
	sphereTraceDirectionNormalizeIfNotNormalizedByRef(&dir);
	pData->rayTraceData.contact.normal = gDirectionForward;
	pData->rayTraceData.directionType = ST_DIRECTION_FORWARD;
	//float dirDotNormal = sphereTraceVector3Dot(dir.v, gDirectionForward.v);
	float dist = dz / dir.v.z;
	if (dist > 0.0f)
	{
		pData->sphereCenter = sphereTraceVector3Add(from, sphereTraceVector3Scale(dir.v, dist));
		pData->traceDistance = sphereTraceAbs(dist);
		if (dz > 0.0f)
		{
			pData->rayTraceData.contact.normal = gDirectionBack;
			pData->rayTraceData.directionType = ST_DIRECTION_BACK;
			pData->rayTraceData.contact.point = sphereTraceVector3Construct(pData->sphereCenter.x, pData->sphereCenter.y, pData->sphereCenter.z + radius);
		}
		else
		{
			pData->rayTraceData.contact.point = sphereTraceVector3Construct(pData->sphereCenter.x, pData->sphereCenter.y, pData->sphereCenter.z - radius);
		}
		return 1;
	}
	else
	{

		return 0;
	}
}
b32 sphereTraceColliderInfiniteYPlaneSphereTrace(ST_Vector3 from, ST_Direction dir, float radius, ST_Vector3 pointOnPlane, ST_SphereTraceData* const pData)
{
	pData->rayTraceData.startPoint = from;
	pData->radius = radius;
	float dy = pointOnPlane.y - from.y;
	if (sphereTraceAbs(dy) <= radius)
	{
		pData->traceDistance = 0.0f;
		pData->sphereCenter = from;
		pData->rayTraceData.distance = sphereTraceAbs(dy);
		pData->rayTraceData.contact.point = sphereTraceVector3Construct(from.x, pointOnPlane.y, from.z);
		if(dy<0.0f)
			pData->rayTraceData.contact.normal = gDirectionUp;
		else
			pData->rayTraceData.contact.normal = gDirectionDown;

		return 1;
	}
	else if (dir.v.y == 0.0f)
	{
		return 0;
	}
	if (dy > 0.0f)
	{
		dy -= radius;
	}
	else
	{
		dy += radius;
	}
	sphereTraceDirectionNormalizeIfNotNormalizedByRef(&dir);
	pData->rayTraceData.contact.normal = gDirectionUp;
	pData->rayTraceData.directionType = ST_DIRECTION_UP;
	float dist = dy / dir.v.y;
	if (dist > 0.0f)
	{
		pData->sphereCenter = sphereTraceVector3Add(from, sphereTraceVector3Scale(dir.v, dist));
		pData->traceDistance = sphereTraceAbs(dist);
		if (dy > 0.0f)
		{
			pData->rayTraceData.contact.normal = gDirectionDown;
			pData->rayTraceData.directionType = ST_DIRECTION_DOWN;
			pData->rayTraceData.contact.point = sphereTraceVector3Construct(pData->sphereCenter.x, pData->sphereCenter.y + radius, pData->sphereCenter.z);
		}
		else
		{
			pData->rayTraceData.contact.point = sphereTraceVector3Construct(pData->sphereCenter.x, pData->sphereCenter.y- radius, pData->sphereCenter.z);
		}
		return 1;
	}
	else
	{
		return 0;
	}
}
b32 sphereTraceColliderInfiniteXPlaneSphereTrace(ST_Vector3 from, ST_Direction dir, float radius, ST_Vector3 pointOnPlane, ST_SphereTraceData* const pData)
{
	pData->rayTraceData.startPoint = from;
	pData->radius = radius;
	float dx = pointOnPlane.x - from.x;
	if (sphereTraceAbs(dx) <= radius)
	{
		pData->traceDistance = 0.0f;
		pData->sphereCenter = from;
		pData->rayTraceData.distance = sphereTraceAbs(dx);
		pData->rayTraceData.contact.point = sphereTraceVector3Construct(pointOnPlane.x, from.y, from.z);
		if (dx < 0.0f)
			pData->rayTraceData.contact.normal = gDirectionRight;
		else
			pData->rayTraceData.contact.normal = gDirectionLeft;

		return 1;
	}
	else if (dir.v.x == 0.0f)
	{
		return 0;
	}
	if (dx > 0.0f)
	{
		dx -= radius;
	}
	else
	{
		dx += radius;
	}
	sphereTraceDirectionNormalizeIfNotNormalizedByRef(&dir);
	pData->rayTraceData.contact.normal = gDirectionRight;
	pData->rayTraceData.directionType = ST_DIRECTION_RIGHT;
	float dist = dx / dir.v.x;
	if (dist > 0.0f)
	{
		pData->sphereCenter = sphereTraceVector3Add(from, sphereTraceVector3Scale(dir.v, dist));
		pData->traceDistance = sphereTraceAbs(dist);
		if (dx > 0.0f)
		{
			pData->rayTraceData.contact.normal = gDirectionLeft;
			pData->rayTraceData.directionType = ST_DIRECTION_LEFT;
			pData->rayTraceData.contact.point = sphereTraceVector3Construct(pData->sphereCenter.x + radius, pData->sphereCenter.y, pData->sphereCenter.z);
		}
		else
		{
			pData->rayTraceData.contact.point = sphereTraceVector3Construct(pData->sphereCenter.x - radius, pData->sphereCenter.y, pData->sphereCenter.z);
		}
		return 1;
	}
	else
	{
		return 0;
	}
}

b32 sphereTraceColliderPlaneRayTrace(ST_Vector3 from, ST_Direction dir, const ST_PlaneCollider* const pPlaneCollider, ST_RayTraceData* const pRaycastData)
{
	sphereTraceDirectionNormalizeIfNotNormalizedByRef(&dir);
	sphereTraceColliderInfinitePlaneRayTrace(from, dir, pPlaneCollider->normal, pPlaneCollider->position, pRaycastData);
	if (pRaycastData->distance > 0.0f)
	{
		if (fpclassify(pRaycastData->distance) == FP_INFINITE)
			return 0;
		pRaycastData->startPoint = from;
		ST_Vector3 vectorFromCenter = sphereTraceVector3Subtract(pRaycastData->contact.point, pPlaneCollider->position);
		float xDist = sphereTraceVector3Dot(vectorFromCenter, pPlaneCollider->right.v);
		if (sphereTraceAbs(xDist) > pPlaneCollider->xHalfExtent)
			return 0;
		float zDist = sphereTraceVector3Dot(vectorFromCenter, pPlaneCollider->forward.v);
		if (sphereTraceAbs(zDist) > pPlaneCollider->zHalfExtent)
			return 0;
		pRaycastData->pOtherCollider = pPlaneCollider;
		return 1;
	}
	return 0;
}

b32 sphereTraceColliderPlaneEdgeTrace(ST_Edge* const pEdge, ST_Direction dir, const ST_PlaneCollider* const pPlaneCollider, ST_EdgeTraceData* const pEdgeTraceData)
{
	sphereTraceDirectionNormalizeIfNotNormalizedByRef(&dir);
	if (sphereTraceColliderInfinitePlaneEdgeTrace(pEdge, dir, pPlaneCollider->normal, pPlaneCollider->position, pEdgeTraceData))
	{
		
		if (!pEdgeTraceData->usesBothContacts)
		{
			if (sphereTraceColliderPlaneIsProjectedPointContained(pEdgeTraceData->contact1.point, pPlaneCollider))
			{
				pEdgeTraceData->contact1.collisionType = ST_COLLISION_FACE;
				return ST_TRUE;
			}
			else
			{
				ST_PlaneEdgeDirection edgeDir = sphereTraceColliderPlaneGetClosestTransformedEdgeToPoint(pPlaneCollider, pEdgeTraceData->contact1.point);
				if (sphereTraceColliderEdgeEdgeTrace(pEdge, dir, &pPlaneCollider->transformedEdges[edgeDir], pEdgeTraceData))
				{
					return ST_TRUE;
				}
				else
				{
					ST_PlaneEdgeDirection otherEdgeDir = sphereTraceColliderPlaneGetClosestTransformedEdgeToPoint(pPlaneCollider, pEdgeTraceData->contact1.point);
					if (otherEdgeDir != edgeDir)
					{
						if (sphereTraceColliderEdgeEdgeTrace(pEdge, dir, &pPlaneCollider->transformedEdges[otherEdgeDir], pEdgeTraceData))
						{
							return ST_TRUE;
						}
					}
				}
			}
		}
		else
		{
			ST_EdgeTraceData etd = *pEdgeTraceData;
			b32 hits = ST_FALSE;
			
			//check the first point and correct the first contact
			if (!sphereTraceColliderPlaneIsProjectedPointContained(etd.contact1.point, pPlaneCollider))
			{
				ST_PlaneEdgeDirection edgeDir = sphereTraceColliderPlaneGetClosestTransformedEdgeToPoint(pPlaneCollider, etd.contact1.point);
				if (sphereTraceColliderEdgeEdgeTrace(pEdge, dir, &pPlaneCollider->transformedEdges[edgeDir], pEdgeTraceData))
				{
					etd.contact1 = pEdgeTraceData->contact1;
					etd.contact1.collisionType = ST_COLLISION_EDGE;
					hits = ST_TRUE;
				}
				else
				{
					ST_PlaneEdgeDirection otherEdgeDir = sphereTraceColliderPlaneGetClosestTransformedEdgeToPoint(pPlaneCollider, pEdgeTraceData->contact1.point);
					if (otherEdgeDir != edgeDir)
					{
						if (sphereTraceColliderEdgeEdgeTrace(pEdge, dir, &pPlaneCollider->transformedEdges[otherEdgeDir], pEdgeTraceData))
						{
							etd.contact1 = pEdgeTraceData->contact1;
							etd.contact1.collisionType = ST_COLLISION_EDGE;
							hits = ST_TRUE;
						}
					}
				}
			}
			else
			{
				etd.contact1.collisionType = ST_COLLISION_FACE;
				hits = ST_TRUE;
			}

			//check the second point and correct the second contact
			if (!sphereTraceColliderPlaneIsProjectedPointContained(etd.contact2.point, pPlaneCollider))
			{
				ST_PlaneEdgeDirection edgeDir = sphereTraceColliderPlaneGetClosestTransformedEdgeToPoint(pPlaneCollider, etd.contact2.point);
				if (sphereTraceColliderEdgeEdgeTrace(pEdge, dir, &pPlaneCollider->transformedEdges[edgeDir], pEdgeTraceData))
				{
					etd.contact2 = pEdgeTraceData->contact1;
					etd.contact2.collisionType = ST_COLLISION_EDGE;
					hits = ST_TRUE;
				}
				else
				{
					ST_PlaneEdgeDirection otherEdgeDir = sphereTraceColliderPlaneGetClosestTransformedEdgeToPoint(pPlaneCollider, pEdgeTraceData->contact1.point);
					if (otherEdgeDir != edgeDir)
					{
						if (sphereTraceColliderEdgeEdgeTrace(pEdge, dir, &pPlaneCollider->transformedEdges[otherEdgeDir], pEdgeTraceData))
						{
							etd.contact2 = pEdgeTraceData->contact1;
							etd.contact2.collisionType = ST_COLLISION_EDGE;
							hits = ST_TRUE;
						}
					}
				}
			}
			else
			{
				etd.contact2.collisionType = ST_COLLISION_FACE;
				hits = ST_TRUE;
			}

			if (hits)
			{
				*pEdgeTraceData = etd;
				return ST_TRUE;
			}
		}
	}
	return ST_FALSE;
}

b32 sphereTraceColliderPlaneIsProjectedPointContained(ST_Vector3 projectedPoint, const ST_PlaneCollider* const pPlaneCollider)
{
	sphereTraceVector3SubtractByRef(&projectedPoint, pPlaneCollider->position);
	float dist = sphereTraceVector3Dot(projectedPoint, pPlaneCollider->right.v);
	if (sphereTraceAbs(dist) > pPlaneCollider->xHalfExtent)
	{
		return 0;
	}
	dist = sphereTraceVector3Dot(projectedPoint, pPlaneCollider->forward.v);
	if (sphereTraceAbs(dist) > pPlaneCollider->zHalfExtent)
	{
		return 0;
	}
	return 1;
}

b32 sphereTraceColliderInfinitePlaneSphereTrace(ST_Vector3 from, ST_Direction dir, float radius, ST_Vector3 pointOnPlane, ST_Direction planeNormal, ST_SphereTraceData* const pSphereTraceData)
{
	ST_SphereContact contact;
	if (sphereTraceColliderInfinitePlaneImposedSphereCollisionTest(from, radius, planeNormal, pointOnPlane, &contact))
	{
		pSphereTraceData->radius = radius;
		pSphereTraceData->sphereCenter = from;
		pSphereTraceData->traceDistance = 0.0f;
		pSphereTraceData->rayTraceData.distance = sphereTraceVector3Distance(from, contact.point);
		pSphereTraceData->rayTraceData.contact.point = contact.point;
		pSphereTraceData->rayTraceData.contact.normal = contact.normal;
		pSphereTraceData->rayTraceData.startPoint = from;
		return 1;
	}
	sphereTraceDirectionNormalizeIfNotNormalizedByRef(&dir);
	ST_Vector3 temp = sphereTraceVector3Subtract(pointOnPlane, from);
	if(sphereTraceVector3IsVectorPositiveInDirection(temp, planeNormal))
		temp = sphereTraceVector3AddAndScale(pointOnPlane, sphereTraceVector3Negative(planeNormal.v), radius);
	else
		temp = sphereTraceVector3AddAndScale(pointOnPlane, planeNormal.v, radius);

	sphereTraceColliderInfinitePlaneRayTrace(from, dir, planeNormal, temp, &pSphereTraceData->rayTraceData);
	if (pSphereTraceData->rayTraceData.distance < 0.0f || isinf(pSphereTraceData->rayTraceData.distance))
		return 0;

	pSphereTraceData->sphereCenter = pSphereTraceData->rayTraceData.contact.point;
	pSphereTraceData->rayTraceData.contact.point = sphereTraceVector3AddAndScale(pSphereTraceData->sphereCenter, sphereTraceVector3Negative(pSphereTraceData->rayTraceData.contact.normal.v), radius);
	pSphereTraceData->rayTraceData.startPoint = from;
	pSphereTraceData->rayTraceData.distance = sphereTraceVector3Length(sphereTraceVector3Subtract(pSphereTraceData->rayTraceData.contact.point, from));
	pSphereTraceData->radius = radius;
	pSphereTraceData->traceDistance = sphereTraceVector3Distance(pSphereTraceData->sphereCenter, pSphereTraceData->rayTraceData.startPoint);
	//pSphereTraceData->rayTraceData.normal = sphereTraceDirectionConstructNormalized(sphereTraceVector3Subtract(pSphereTraceData->sphereCenter, pSphereTraceData->rayTraceData.hitPoint));
	return 1;


}

//b32 sphereTraceColliderInfinitePlaneSphereTrace(ST_Vector3 from, ST_Direction dir, float radius, ST_Vector3 pointOnPlane, ST_Direction planeNormal, ST_SphereTraceData* const pSphereTraceData)
//{
//	ST_SphereContact contact;
//	if (sphereTraceColliderInfinitePlaneImposedSphereCollisionTest(from, radius, planeNormal, pointOnPlane, &contact))
//	{
//		pSphereTraceData->radius = radius;
//		pSphereTraceData->sphereCenter = from;
//		pSphereTraceData->traceDistance = 0.0f;
//		pSphereTraceData->rayTraceData.distance = sphereTraceVector3Distance(from, contact.point);
//		pSphereTraceData->rayTraceData.contact.point = contact.point;
//		pSphereTraceData->rayTraceData.contact.normal = contact.normal;
//		pSphereTraceData->rayTraceData.startPoint = from;
//		return 1;
//	}
//	sphereTraceDirectionNormalizeIfNotNormalizedByRef(&dir);
//	sphereTraceColliderInfinitePlaneRayTrace(from, dir, planeNormal, pointOnPlane, &pSphereTraceData->rayTraceData);
//	if (pSphereTraceData->rayTraceData.distance < 0.0f || isinf(pSphereTraceData->rayTraceData.distance))
//		return 0;
//	float alpha;
//	float dot = sphereTraceVector3Dot(dir.v, pSphereTraceData->rayTraceData.contact.normal.v);
//	float hypotinus;
//	ST_Vector3 orthogonal = pSphereTraceData->rayTraceData.contact.normal.v;
//	if (sphereTraceAbs(sphereTraceAbs(dot) - 1) < ST_COLLIDER_TOLERANCE)
//	{
//		alpha = 0.0f;
//		hypotinus = radius;
//	}
//	else
//	{
//		orthogonal = sphereTraceVector3Cross(pSphereTraceData->rayTraceData.contact.normal.v, dir.v);
//		orthogonal = sphereTraceVector3Cross(pSphereTraceData->rayTraceData.contact.normal.v, orthogonal);
//		sphereTraceVector3NormalizeByRef(&orthogonal);
//		alpha = acosf(sphereTraceVector3Dot(sphereTraceVector3Negative(dir.v), orthogonal));
//		hypotinus = radius / sinf(alpha);
//	}
//
//	//if (sphereTraceVector3EpsilonEquals(dir, sphereTraceVector3Negative(orthogonal), ST_COLLIDER_TOLERANCE))
//	//{
//
//	//}
//	//
//
//	pSphereTraceData->sphereCenter = sphereTraceVector3AddAndScale(from, dir.v, pSphereTraceData->rayTraceData.distance - hypotinus);
//
//	pSphereTraceData->rayTraceData.contact.point = sphereTraceVector3AddAndScale(pSphereTraceData->sphereCenter, sphereTraceVector3Negative(pSphereTraceData->rayTraceData.contact.normal.v), radius);
//	pSphereTraceData->rayTraceData.startPoint = from;
//	pSphereTraceData->rayTraceData.distance = sphereTraceVector3Length(sphereTraceVector3Subtract(pSphereTraceData->rayTraceData.contact.point, from));
//	pSphereTraceData->radius = radius;
//	pSphereTraceData->traceDistance = sphereTraceVector3Distance(pSphereTraceData->sphereCenter, pSphereTraceData->rayTraceData.startPoint);
//	//pSphereTraceData->rayTraceData.normal = sphereTraceDirectionConstructNormalized(sphereTraceVector3Subtract(pSphereTraceData->sphereCenter, pSphereTraceData->rayTraceData.hitPoint));
//	return 1;
//
//
//}

b32 sphereTraceColliderPlaneSphereTrace(ST_Vector3 from, ST_Direction dir, float radius, ST_PlaneCollider* const pPlaneCollider, ST_SphereTraceData* const pSphereTraceData)
{
	sphereTraceDirectionNormalizeIfNotNormalizedByRef(&dir);
	if (sphereTraceColliderInfinitePlaneSphereTrace(from, dir, radius, pPlaneCollider->position, pPlaneCollider->normal, pSphereTraceData))
	{
		if (sphereTraceColliderPlaneIsProjectedPointContained(pSphereTraceData->rayTraceData.contact.point, pPlaneCollider))
		{
			return 1;
		}
	}
	ST_SphereTraceData datTest;
	float dot = sphereTraceVector3Dot(dir.v, pPlaneCollider->normal.v);
	if (dot < ST_COLLIDER_TOLERANCE)
	{
		ST_PlaneEdgeDirection edgeDir = sphereTraceColliderPlaneGetClosestTransformedEdgeToPoint(pPlaneCollider, pSphereTraceData->rayTraceData.contact.point);
		if (sphereTraceColliderEdgeSphereTrace(from, dir, radius, &pPlaneCollider->transformedEdges[edgeDir], &datTest))
		{
			*pSphereTraceData = datTest;
			pSphereTraceData->rayTraceData.pOtherCollider = pPlaneCollider;
			//pSphereTraceData->rayTraceData.otherColliderType = COLLIDER_PLANE;
			return 1;
		}
		return 0;
	}
	else
	{
		float closestEdgePointDist = FLT_MAX;	
		if (sphereTraceColliderEdgeSphereTrace(from, dir, radius, &pPlaneCollider->transformedEdges[0], &datTest))
		{
			closestEdgePointDist = datTest.traceDistance;
			*pSphereTraceData = datTest;
		}
		if (sphereTraceColliderEdgeSphereTrace(from, dir, radius, &pPlaneCollider->transformedEdges[1], &datTest))
		{
			if (datTest.traceDistance < closestEdgePointDist)
			{
				closestEdgePointDist = datTest.traceDistance;
				*pSphereTraceData = datTest;
			}
		}
		if (sphereTraceColliderEdgeSphereTrace(from, dir, radius, &pPlaneCollider->transformedEdges[2], &datTest))
		{
			if (datTest.traceDistance < closestEdgePointDist)
			{
				closestEdgePointDist = datTest.traceDistance;
				*pSphereTraceData = datTest;
			}
		}
		if (sphereTraceColliderEdgeSphereTrace(from, dir, radius, &pPlaneCollider->transformedEdges[3], &datTest))
		{
			if (datTest.traceDistance < closestEdgePointDist)
			{
				closestEdgePointDist = datTest.traceDistance;
				*pSphereTraceData = datTest;
			}
		}
		if (closestEdgePointDist < FLT_MAX)
			return 1;
	}



	return 0;
}

b32 sphereTraceColliderPlaneSphereTraceOut(ST_Vector3 spherePos, float sphereRadius, ST_Direction clipoutDir, ST_PlaneCollider* const pPlaneCollider, ST_SphereTraceData* const pSphereCastData)
{
	sphereTraceDirectionNormalizeIfNotNormalizedByRef(&clipoutDir);
	ST_Vector3 castPoint = sphereTraceVector3AddAndScale(spherePos, clipoutDir.v, 2 * pPlaneCollider->collider.boundingRadius + 2 * sphereRadius);
	b32 ret = sphereTraceColliderPlaneSphereTrace(castPoint, sphereTraceDirectionNegative(clipoutDir), sphereRadius, pPlaneCollider, pSphereCastData);
	//ST_Direction dir = sphereTraceDirectionConstructNormalized(sphereTraceVector3Subtract(castPoint, pSphereCastData->sphereCenter));
	return ret;
}

b32 sphereTraceColliderImposedPlaneIsProjectedPointContained(ST_Vector3 projectedPoint, const ST_Vector3* const pPlanePos, const ST_Direction* const pPlaneRight, 
	const ST_Direction* const  pPlaneForward, float xHalfExtents, float zHalfExtents)
{
	sphereTraceVector3SubtractByRef(&projectedPoint, *pPlanePos);
	float dist = sphereTraceVector3Dot(projectedPoint, pPlaneRight->v);
	if (sphereTraceAbs(dist) > xHalfExtents)
	{
		return 0;
	}
	dist = sphereTraceVector3Dot(projectedPoint, pPlaneForward->v);
	if (sphereTraceAbs(dist) > zHalfExtents)
	{
		return 0;
	}
	return 1;
}

ST_Edge sphereTraceColliderImposedPlaneEdgeDirectionToEdge(const ST_Vector3* const pPlanePos, const ST_Direction* const pPlaneRight,
	const ST_Direction* const  pPlaneForward, float xHalfExtents, float zHalfExtents, ST_PlaneEdgeDirection dir)
{
	switch (dir)
	{
	case PLANE_EDGE_RIGHT:
		return sphereTraceEdgeConstruct1(sphereTraceVector3AddAndScale2(*pPlanePos, pPlaneRight->v, xHalfExtents, pPlaneForward->v, -zHalfExtents),
			sphereTraceVector3AddAndScale2(*pPlanePos, pPlaneRight->v, xHalfExtents, pPlaneForward->v, zHalfExtents), 2.0f * zHalfExtents, *pPlaneForward);
		break;
	case PLANE_EDGE_FORWARD:
		return sphereTraceEdgeConstruct1(sphereTraceVector3AddAndScale2(*pPlanePos, pPlaneRight->v, -xHalfExtents, pPlaneForward->v, zHalfExtents),
			sphereTraceVector3AddAndScale2(*pPlanePos, pPlaneRight->v, xHalfExtents, pPlaneForward->v, zHalfExtents), 2.0f * xHalfExtents, *pPlaneRight);
		break;
	case PLANE_EDGE_LEFT:
		return sphereTraceEdgeConstruct1(sphereTraceVector3AddAndScale2(*pPlanePos, pPlaneRight->v, -xHalfExtents, pPlaneForward->v, -zHalfExtents),
			sphereTraceVector3AddAndScale2(*pPlanePos, pPlaneRight->v, -xHalfExtents, pPlaneForward->v, zHalfExtents), 2.0f * zHalfExtents, *pPlaneForward);
		break;
	case PLANE_EDGE_BACK:
		return sphereTraceEdgeConstruct1(sphereTraceVector3AddAndScale2(*pPlanePos, pPlaneRight->v, -xHalfExtents, pPlaneForward->v, -zHalfExtents),
			sphereTraceVector3AddAndScale2(*pPlanePos, pPlaneRight->v, xHalfExtents, pPlaneForward->v, -zHalfExtents), 2.0f * xHalfExtents, *pPlaneRight);
		break;
	}
}

ST_PlaneEdgeDirection sphereTraceColliderImposedPlaneGetClosestTransformedEdgeDirectionToPoint(const ST_Vector3* const pPlanePos, const ST_Direction* const pPlaneRight,
	const ST_Direction* const  pPlaneForward, float xHalfExtents, float zHalfExtents, ST_Vector3 point)
{
	ST_Vector3 distVec = sphereTraceVector3Subtract(point, *pPlanePos);
	float xDist = sphereTraceVector3Dot(distVec, pPlaneRight->v) / xHalfExtents;
	float zDist = sphereTraceVector3Dot(distVec, pPlaneForward->v) / zHalfExtents;

	if (xDist > 0.0f)
	{
		if (sphereTraceAbs(zDist) > xDist)
		{
			if (zDist > 0.0f)
			{
				//return forward edge
				return PLANE_EDGE_FORWARD;
			}
			else
			{
				//return back edge
				return PLANE_EDGE_BACK;
			}
		}
		else
		{
			//return right edge
			return PLANE_EDGE_RIGHT;
		}
	}
	else
	{
		if (sphereTraceAbs(zDist) > -xDist)
		{
			if (zDist > 0.0f)
			{
				//return forward edge
				return PLANE_EDGE_FORWARD;
			}
			else
			{
				//return back edge
				return PLANE_EDGE_BACK;
			}
		}
		else
		{
			//return left edge
			return PLANE_EDGE_LEFT;
		}
	}
}

ST_Edge sphereTraceColliderImposedPlaneGetClosestTransformedEdgeToPoint(const ST_Vector3* const pPlanePos, const ST_Direction* const pPlaneRight,
	const ST_Direction* const  pPlaneForward, float xHalfExtents, float zHalfExtents, ST_Vector3 point)
{
	ST_Vector3 distVec = sphereTraceVector3Subtract(point, *pPlanePos);
	float xDist = sphereTraceVector3Dot(distVec, pPlaneRight->v) / xHalfExtents;
	float zDist = sphereTraceVector3Dot(distVec, pPlaneForward->v) / zHalfExtents;

	if (xDist > 0.0f)
	{
		if (sphereTraceAbs(zDist) > xDist)
		{
			if (zDist > 0.0f)
			{
				//return forward edge
				return sphereTraceEdgeConstruct1(sphereTraceVector3AddAndScale2(*pPlanePos, pPlaneRight->v, -xHalfExtents, pPlaneForward->v, zHalfExtents),
					sphereTraceVector3AddAndScale2(*pPlanePos, pPlaneRight->v, xHalfExtents, pPlaneForward->v, zHalfExtents), 2.0f * xHalfExtents, *pPlaneRight);
			}
			else
			{
				//return back edge
				return sphereTraceEdgeConstruct1(sphereTraceVector3AddAndScale2(*pPlanePos, pPlaneRight->v, -xHalfExtents, pPlaneForward->v, -zHalfExtents),
					sphereTraceVector3AddAndScale2(*pPlanePos, pPlaneRight->v, xHalfExtents, pPlaneForward->v, -zHalfExtents), 2.0f * xHalfExtents, *pPlaneRight);
			}
		}
		else
		{
			//return right edge
			return sphereTraceEdgeConstruct1(sphereTraceVector3AddAndScale2(*pPlanePos, pPlaneRight->v, xHalfExtents, pPlaneForward->v, -zHalfExtents),
				sphereTraceVector3AddAndScale2(*pPlanePos, pPlaneRight->v, xHalfExtents, pPlaneForward->v, zHalfExtents), 2.0f * zHalfExtents, *pPlaneForward);
		}
	}
	else
	{
		if (sphereTraceAbs(zDist) > -xDist)
		{
			if (zDist > 0.0f)
			{
				//return forward edge
				return sphereTraceEdgeConstruct1(sphereTraceVector3AddAndScale2(*pPlanePos, pPlaneRight->v, -xHalfExtents, pPlaneForward->v, zHalfExtents),
					sphereTraceVector3AddAndScale2(*pPlanePos, pPlaneRight->v, xHalfExtents, pPlaneForward->v, zHalfExtents), 2.0f * xHalfExtents, *pPlaneRight);
			}
			else
			{
				//return back edge
				return sphereTraceEdgeConstruct1(sphereTraceVector3AddAndScale2(*pPlanePos, pPlaneRight->v, -xHalfExtents, pPlaneForward->v, -zHalfExtents),
					sphereTraceVector3AddAndScale2(*pPlanePos, pPlaneRight->v, xHalfExtents, pPlaneForward->v, -zHalfExtents), 2.0f * xHalfExtents, *pPlaneRight);
			}
		}
		else
		{
			//return left edge
			return sphereTraceEdgeConstruct1(sphereTraceVector3AddAndScale2(*pPlanePos, pPlaneRight->v, -xHalfExtents, pPlaneForward->v, -zHalfExtents),
				sphereTraceVector3AddAndScale2(*pPlanePos, pPlaneRight->v, -xHalfExtents, pPlaneForward->v, zHalfExtents), 2.0f * zHalfExtents, *pPlaneForward);
		}
	}
}

b32 sphereTraceColliderImposedPlaneRayTrace(ST_Vector3 start, ST_Direction dir, ST_Vector3 planePos, ST_Direction planeNormal, ST_Direction planeRight, ST_Direction planeForward, float xHalfExtents, float zHalfExtents, ST_RayTraceData* const pRayTraceData)
{
	sphereTraceDirectionNormalizeIfNotNormalizedByRef(&dir);
	sphereTraceColliderInfinitePlaneRayTrace(start, dir, planeNormal, planePos, pRayTraceData);
	if (pRayTraceData->distance > 0.0f)
	{
		if (fpclassify(pRayTraceData->distance) == FP_INFINITE)
			return 0;
		pRayTraceData->startPoint = start;
		ST_Vector3 vectorFromCenter = sphereTraceVector3Subtract(pRayTraceData->contact.point, planePos);
		float xDist = sphereTraceVector3Dot(vectorFromCenter, planeRight.v);
		if (sphereTraceAbs(xDist) > xHalfExtents)
			return 0;
		float zDist = sphereTraceVector3Dot(vectorFromCenter, planeForward.v);
		if (sphereTraceAbs(zDist) > zHalfExtents)
			return 0;
		return 1;
	}
	return 0;
}

b32 sphereTraceColliderImposedPlaneEdgeTrace(ST_Edge* const pEdge, ST_Direction dir, ST_Vector3 planePos, ST_Direction planeNormal, ST_Direction planeRight, ST_Direction planeForward, float xHalfExtents, float zHalfExtents, ST_EdgeTraceData* const pEdgeTraceData)
{
	sphereTraceDirectionNormalizeIfNotNormalizedByRef(&dir);
	if (sphereTraceColliderInfinitePlaneEdgeTrace(pEdge, dir, planeNormal, planePos, pEdgeTraceData))
	{

		if (!pEdgeTraceData->usesBothContacts)
		{
			if (sphereTraceColliderImposedPlaneIsProjectedPointContained(pEdgeTraceData->contact1.point, &planePos, &planeRight, &planeForward, xHalfExtents, zHalfExtents))
			{
				pEdgeTraceData->contact1.collisionType = ST_COLLISION_FACE;
				return ST_TRUE;
			}
			else
			{
				ST_Edge edge = sphereTraceColliderImposedPlaneGetClosestTransformedEdgeToPoint(&planePos, &planeRight, &planeForward, xHalfExtents, zHalfExtents, pEdgeTraceData->contact1.point);
				if (sphereTraceColliderEdgeEdgeTrace(pEdge, dir, &edge, pEdgeTraceData))
				{
					return ST_TRUE;
				}
				else
				{
					edge = sphereTraceColliderImposedPlaneGetClosestTransformedEdgeToPoint(&planePos, &planeRight, &planeForward, xHalfExtents, zHalfExtents, pEdgeTraceData->contact1.point);

					if (sphereTraceColliderEdgeEdgeTrace(pEdge, dir, &edge, pEdgeTraceData))
					{
						return ST_TRUE;
					}
				}
			}
		}
		else
		{
			ST_EdgeTraceData etd = *pEdgeTraceData;
			b32 hits = ST_FALSE;

			//check the first point and correct the first contact
			if (!sphereTraceColliderImposedPlaneIsProjectedPointContained(etd.contact1.point, &planePos, &planeRight, &planeForward, xHalfExtents, zHalfExtents))
			{
				ST_Edge edge = sphereTraceColliderImposedPlaneGetClosestTransformedEdgeToPoint(&planePos, &planeRight, &planeForward, xHalfExtents, zHalfExtents, etd.contact1.point);
				if (sphereTraceColliderEdgeEdgeTrace(pEdge, dir, &edge, pEdgeTraceData))
				{
					etd.contact1 = pEdgeTraceData->contact1;
					etd.contact1.collisionType = ST_COLLISION_EDGE;
					hits = ST_TRUE;
				}
				else
				{
					ST_Edge edge = sphereTraceColliderImposedPlaneGetClosestTransformedEdgeToPoint(&planePos, &planeRight, &planeForward, xHalfExtents, zHalfExtents, pEdgeTraceData->contact1.point);

					if (sphereTraceColliderEdgeEdgeTrace(pEdge, dir, &edge, pEdgeTraceData))
					{
						etd.contact1 = pEdgeTraceData->contact1;
						etd.contact1.collisionType = ST_COLLISION_EDGE;
						hits = ST_TRUE;
					}

				}
			}
			else
			{
				etd.contact1.collisionType = ST_COLLISION_FACE;
				hits = ST_TRUE;
			}

			//check the second point and correct the second contact
			if (!sphereTraceColliderImposedPlaneIsProjectedPointContained(etd.contact2.point, &planePos, &planeRight, &planeForward, xHalfExtents, zHalfExtents))
			{
				ST_Edge edge = sphereTraceColliderImposedPlaneGetClosestTransformedEdgeToPoint(&planePos, &planeRight, &planeForward, xHalfExtents, zHalfExtents, etd.contact2.point);
				if (sphereTraceColliderEdgeEdgeTrace(pEdge, dir, &edge, pEdgeTraceData))
				{
					etd.contact2 = pEdgeTraceData->contact1;
					etd.contact2.collisionType = ST_COLLISION_EDGE;
					hits = ST_TRUE;
				}
				else
				{
					ST_Edge edge = sphereTraceColliderImposedPlaneGetClosestTransformedEdgeToPoint(&planePos, &planeRight, &planeForward, xHalfExtents, zHalfExtents, pEdgeTraceData->contact1.point);

					if (sphereTraceColliderEdgeEdgeTrace(pEdge, dir, &edge, pEdgeTraceData))
					{
						etd.contact2 = pEdgeTraceData->contact1;
						etd.contact2.collisionType = ST_COLLISION_EDGE;
						hits = ST_TRUE;
					}
				}
			}
			else
			{
				etd.contact2.collisionType = ST_COLLISION_FACE;
				hits = ST_TRUE;
			}

			if (hits)
			{
				*pEdgeTraceData = etd;
				return ST_TRUE;
			}
		}
	}
	return ST_FALSE;
}

b32 sphereTraceColliderImposedPlaneEdgeTrace1(ST_Edge* const pEdge, ST_Direction* const pDir, ST_Vector3* const pPlanePos, ST_Direction* const pPlaneNormal, ST_Direction* const pPlaneRight, ST_Direction* const pPlaneForward, float xHalfExtents, float zHalfExtents, ST_EdgeTraceData* const pEdgeTraceData)
{
	sphereTraceDirectionNormalizeIfNotNormalizedByRef(pDir);
	if (sphereTraceColliderInfinitePlaneEdgeTrace(pEdge, *pDir, *pPlaneNormal, *pPlanePos, pEdgeTraceData))
	{

		if (!pEdgeTraceData->usesBothContacts)
		{
			if (sphereTraceColliderImposedPlaneIsProjectedPointContained(pEdgeTraceData->contact1.point, pPlanePos, pPlaneRight, pPlaneForward, xHalfExtents, zHalfExtents))
			{
				pEdgeTraceData->contact1.collisionType = ST_COLLISION_FACE;
				return ST_TRUE;
			}
			else
			{
				ST_Edge edge = sphereTraceColliderImposedPlaneGetClosestTransformedEdgeToPoint(pPlanePos, pPlaneRight, pPlaneForward, xHalfExtents, zHalfExtents, pEdgeTraceData->contact1.point);
				if (sphereTraceColliderEdgeEdgeTrace(pEdge, *pDir, &edge, pEdgeTraceData))
				{
					return ST_TRUE;
				}
				else
				{
					edge = sphereTraceColliderImposedPlaneGetClosestTransformedEdgeToPoint(pPlanePos, pPlaneRight, pPlaneForward, xHalfExtents, zHalfExtents, pEdgeTraceData->contact1.point);

					if (sphereTraceColliderEdgeEdgeTrace(pEdge, *pDir, &edge, pEdgeTraceData))
					{
						return ST_TRUE;
					}
				}
			}
		}
		else
		{
			ST_EdgeTraceData etd = *pEdgeTraceData;
			b32 hits = ST_FALSE;

			//check the first point and correct the first contact
			if (!sphereTraceColliderImposedPlaneIsProjectedPointContained(etd.contact1.point, pPlanePos, pPlaneRight, pPlaneForward, xHalfExtents, zHalfExtents))
			{
				ST_Edge edge = sphereTraceColliderImposedPlaneGetClosestTransformedEdgeToPoint(pPlanePos, pPlaneRight, pPlaneForward, xHalfExtents, zHalfExtents, etd.contact1.point);
				if (sphereTraceColliderEdgeEdgeTrace(pEdge, *pDir, &edge, pEdgeTraceData))
				{
					etd.contact1 = pEdgeTraceData->contact1;
					etd.contact1.collisionType = ST_COLLISION_EDGE;
					hits = ST_TRUE;
				}
				else
				{
					ST_Edge edge = sphereTraceColliderImposedPlaneGetClosestTransformedEdgeToPoint(pPlanePos, pPlaneRight, pPlaneForward, xHalfExtents, zHalfExtents, pEdgeTraceData->contact1.point);

					if (sphereTraceColliderEdgeEdgeTrace(pEdge, *pDir, &edge, pEdgeTraceData))
					{
						etd.contact1 = pEdgeTraceData->contact1;
						etd.contact1.collisionType = ST_COLLISION_EDGE;
						hits = ST_TRUE;
					}

				}
			}
			else
			{
				etd.contact1.collisionType = ST_COLLISION_FACE;
				hits = ST_TRUE;
			}

			//check the second point and correct the second contact
			if (!sphereTraceColliderImposedPlaneIsProjectedPointContained(etd.contact2.point, pPlanePos, pPlaneRight, pPlaneForward, xHalfExtents, zHalfExtents))
			{
				ST_Edge edge = sphereTraceColliderImposedPlaneGetClosestTransformedEdgeToPoint(pPlanePos, pPlaneRight, pPlaneForward, xHalfExtents, zHalfExtents, etd.contact2.point);
				if (sphereTraceColliderEdgeEdgeTrace(pEdge, *pDir, &edge, pEdgeTraceData))
				{
					etd.contact2 = pEdgeTraceData->contact1;
					etd.contact2.collisionType = ST_COLLISION_EDGE;
					hits = ST_TRUE;
				}
				else
				{
					ST_Edge edge = sphereTraceColliderImposedPlaneGetClosestTransformedEdgeToPoint(pPlanePos, pPlaneRight, pPlaneForward, xHalfExtents, zHalfExtents, pEdgeTraceData->contact1.point);

					if (sphereTraceColliderEdgeEdgeTrace(pEdge, *pDir, &edge, pEdgeTraceData))
					{
						etd.contact2 = pEdgeTraceData->contact1;
						etd.contact2.collisionType = ST_COLLISION_EDGE;
						hits = ST_TRUE;
					}
				}
			}
			else
			{
				etd.contact2.collisionType = ST_COLLISION_FACE;
				hits = ST_TRUE;
			}

			if (hits)
			{
				*pEdgeTraceData = etd;
				return ST_TRUE;
			}
		}
	}
	return ST_FALSE;
}


b32 sphereTraceColliderPlanePlaneTrace(ST_Vector3 planePos, ST_Direction planeNormal, ST_Direction planeRight, ST_Direction planeForward, float xHalfExtents, float zHalfExtents, ST_Direction dir, ST_PlaneCollider* const pPlaneCollider, ST_BoxTraceData* const pBoxTraceData)
{
	ST_EdgeTraceData etd;
	ST_Direction negDir = sphereTraceDirectionNegative(dir);
	float minDist = FLT_MAX;
	pBoxTraceData->numContacts = 0;
	if (sphereTraceColliderImposedPlaneEdgeTrace(&pPlaneCollider->transformedEdges[PLANE_EDGE_BACK], negDir, planePos, planeNormal, planeRight, planeForward, xHalfExtents, zHalfExtents, &etd))
	{
		if (!etd.usesBothContacts)
		{
			if (etd.hitIndex == 0)
			{
				etd.contact1.point = pPlaneCollider->transformedEdges[PLANE_EDGE_BACK].point1;
				etd.contact1.normal = sphereTraceDirectionNegative(etd.contact1.normal);
			}
			else
			{
				etd.contact1.point = pPlaneCollider->transformedEdges[PLANE_EDGE_BACK].point2;
				etd.contact1.normal = sphereTraceDirectionNegative(etd.contact1.normal);
			}
		}
		else
		{
			etd.contact1.point = pPlaneCollider->transformedEdges[PLANE_EDGE_BACK].point1;
			etd.contact2.point = pPlaneCollider->transformedEdges[PLANE_EDGE_BACK].point2;
			etd.contact1.normal = sphereTraceDirectionNegative(etd.contact1.normal);
			etd.contact2.normal = sphereTraceDirectionNegative(etd.contact2.normal);
		}
		if (etd.contact1.collisionType == ST_COLLISION_FACE)
		{
			minDist = etd.distance;
			pBoxTraceData->contacts[pBoxTraceData->numContacts++] = etd.contact1;
		}
		if (etd.usesBothContacts)
		{
			if (etd.distance < (minDist - ST_COLLIDER_TOLERANCE))
			{
				minDist = etd.distance;
			}
			if (etd.contact2.collisionType == ST_COLLISION_FACE)
			{
				pBoxTraceData->contacts[pBoxTraceData->numContacts++] = etd.contact2;
			}
		}
	}

	if (sphereTraceColliderImposedPlaneEdgeTrace(&pPlaneCollider->transformedEdges[PLANE_EDGE_FORWARD], negDir, planePos, planeNormal, planeRight, planeForward, xHalfExtents, zHalfExtents, &etd))
	{
		if (!etd.usesBothContacts)
		{
			if (etd.hitIndex == 0)
			{
				etd.contact1.point = pPlaneCollider->transformedEdges[PLANE_EDGE_FORWARD].point1;
				etd.contact1.normal = sphereTraceDirectionNegative(etd.contact1.normal);
			}
			else
			{
				etd.contact1.point = pPlaneCollider->transformedEdges[PLANE_EDGE_FORWARD].point2;
				etd.contact1.normal = sphereTraceDirectionNegative(etd.contact1.normal);
			}
		}
		else
		{
			etd.contact1.point = pPlaneCollider->transformedEdges[PLANE_EDGE_FORWARD].point1;
			etd.contact2.point = pPlaneCollider->transformedEdges[PLANE_EDGE_FORWARD].point2;
			etd.contact1.normal = sphereTraceDirectionNegative(etd.contact1.normal);
			etd.contact2.normal = sphereTraceDirectionNegative(etd.contact2.normal);
		}
		if (etd.contact1.collisionType == ST_COLLISION_FACE)
		{
			if (etd.distance < (minDist - ST_COLLIDER_TOLERANCE))
			{
				minDist = etd.distance;
				pBoxTraceData->numContacts = 0;
				pBoxTraceData->contacts[pBoxTraceData->numContacts++] = etd.contact1;
			}
			else if (sphereTraceEpsilonEqual(etd.distance, minDist, ST_COLLIDER_TOLERANCE))
			{
				pBoxTraceData->contacts[pBoxTraceData->numContacts++] = etd.contact1;
			}
		}
		if (etd.usesBothContacts)
		{
			if (etd.distance < (minDist - ST_COLLIDER_TOLERANCE))
			{
				minDist = etd.distance;
			}
			if (etd.contact2.collisionType == ST_COLLISION_FACE)
			{
				pBoxTraceData->contacts[pBoxTraceData->numContacts++] = etd.contact2;
			}
		}
	}

	ST_Edge edge = sphereTraceEdgeConstruct1(sphereTraceVector3AddAndScale2(planePos, planeRight.v, -xHalfExtents, planeForward.v, zHalfExtents),
		sphereTraceVector3AddAndScale2(planePos, planeRight.v, xHalfExtents, planeForward.v, zHalfExtents), 2.0f * xHalfExtents, planeRight);
	if (sphereTraceColliderPlaneEdgeTrace(&edge, dir, pPlaneCollider, &etd))
	{
		if (etd.distance < (minDist - ST_COLLIDER_TOLERANCE))
		{
			minDist = etd.distance;
			pBoxTraceData->numContacts = 0;
			pBoxTraceData->contacts[pBoxTraceData->numContacts++] = etd.contact1;
		}
		else if (sphereTraceEpsilonEqual(etd.distance, minDist, ST_COLLIDER_TOLERANCE))
		{
			pBoxTraceData->contacts[pBoxTraceData->numContacts++] = etd.contact1;
		}

		if (etd.usesBothContacts)
		{
			pBoxTraceData->contacts[pBoxTraceData->numContacts++] = etd.contact2;
		}
	}

	edge = sphereTraceEdgeConstruct1(sphereTraceVector3AddAndScale2(planePos, planeRight.v, xHalfExtents, planeForward.v, -zHalfExtents),
		sphereTraceVector3AddAndScale2(planePos, planeRight.v, xHalfExtents, planeForward.v, zHalfExtents), 2.0f * zHalfExtents, planeForward);
	if (sphereTraceColliderPlaneEdgeTrace(&edge, dir, pPlaneCollider, &etd))
	{
		if (etd.distance < (minDist - ST_COLLIDER_TOLERANCE))
		{
			minDist = etd.distance;
			pBoxTraceData->numContacts = 0;
			pBoxTraceData->contacts[pBoxTraceData->numContacts++] = etd.contact1;
		}
		else if (sphereTraceEpsilonEqual(etd.distance, minDist, ST_COLLIDER_TOLERANCE))
		{
			pBoxTraceData->contacts[pBoxTraceData->numContacts++] = etd.contact1;
		}

		if (etd.usesBothContacts)
		{
			pBoxTraceData->contacts[pBoxTraceData->numContacts++] = etd.contact2;
		}
	}

	edge = sphereTraceEdgeConstruct1(sphereTraceVector3AddAndScale2(planePos, planeRight.v, -xHalfExtents, planeForward.v,-zHalfExtents),
		sphereTraceVector3AddAndScale2(planePos, planeRight.v, xHalfExtents, planeForward.v, -zHalfExtents), 2.0f * xHalfExtents, planeRight);
	if (sphereTraceColliderPlaneEdgeTrace(&edge, dir, pPlaneCollider, &etd))
	{
		if (etd.distance < (minDist - ST_COLLIDER_TOLERANCE))
		{
			minDist = etd.distance;
			pBoxTraceData->numContacts = 0;
			pBoxTraceData->contacts[pBoxTraceData->numContacts++] = etd.contact1;
		}
		else if (sphereTraceEpsilonEqual(etd.distance, minDist, ST_COLLIDER_TOLERANCE))
		{
			pBoxTraceData->contacts[pBoxTraceData->numContacts++] = etd.contact1;
		}

		if (etd.usesBothContacts)
		{
			pBoxTraceData->contacts[pBoxTraceData->numContacts++] = etd.contact2;
		}
	}

	edge = sphereTraceEdgeConstruct1(sphereTraceVector3AddAndScale2(planePos, planeRight.v, -xHalfExtents, planeForward.v, -zHalfExtents),
		sphereTraceVector3AddAndScale2(planePos, planeRight.v, -xHalfExtents, planeForward.v, zHalfExtents), 2.0f * zHalfExtents, planeForward);
	if (sphereTraceColliderPlaneEdgeTrace(&edge, dir, pPlaneCollider, &etd))
	{
		if (etd.distance < (minDist - ST_COLLIDER_TOLERANCE))
		{
			minDist = etd.distance;
			pBoxTraceData->numContacts = 0;
			pBoxTraceData->contacts[pBoxTraceData->numContacts++] = etd.contact1;
		}
		else if (sphereTraceEpsilonEqual(etd.distance, minDist, ST_COLLIDER_TOLERANCE))
		{
			pBoxTraceData->contacts[pBoxTraceData->numContacts++] = etd.contact1;
		}

		if (etd.usesBothContacts)
		{
			pBoxTraceData->contacts[pBoxTraceData->numContacts++] = etd.contact2;
		}
	}

	if (minDist < FLT_MAX)
	{
		pBoxTraceData->traceDistance = minDist;
		pBoxTraceData->boxCenter = sphereTraceVector3AddAndScale(planePos, dir.v, minDist);
		return ST_TRUE;
	}
	return ST_FALSE;
}

b32 sphereTraceColliderPlanePlaneTrace1(ST_Vector3* const pPlanePos, ST_Direction* const pPlaneNormal, ST_Direction* const pPlaneRight, ST_Direction* const pPlaneForward, 
	float xHalfExtents, float zHalfExtents, ST_Direction* const pDir, ST_PlaneCollider* const pPlaneCollider, ST_BoxTraceData* const pBoxTraceData)
{
	ST_EdgeTraceData etd;
	ST_Direction negDir = sphereTraceDirectionNegative(*pDir);
	float minDist = FLT_MAX;
	pBoxTraceData->numContacts = 0;
	if (sphereTraceColliderImposedPlaneEdgeTrace1(&pPlaneCollider->transformedEdges[PLANE_EDGE_BACK], &negDir, pPlanePos, pPlaneNormal, pPlaneRight, pPlaneForward, xHalfExtents, zHalfExtents, &etd))
	{
		if (!etd.usesBothContacts)
		{
			if (etd.hitIndex == 0)
			{
				etd.contact1.point = pPlaneCollider->transformedEdges[PLANE_EDGE_BACK].point1;
				etd.contact1.normal = sphereTraceDirectionNegative(etd.contact1.normal);
			}
			else
			{
				etd.contact1.point = pPlaneCollider->transformedEdges[PLANE_EDGE_BACK].point2;
				etd.contact1.normal = sphereTraceDirectionNegative(etd.contact1.normal);
			}
		}
		else
		{
			etd.contact1.point = pPlaneCollider->transformedEdges[PLANE_EDGE_BACK].point1;
			etd.contact2.point = pPlaneCollider->transformedEdges[PLANE_EDGE_BACK].point2;
			etd.contact1.normal = sphereTraceDirectionNegative(etd.contact1.normal);
			etd.contact2.normal = sphereTraceDirectionNegative(etd.contact2.normal);
		}
		if (etd.contact1.collisionType == ST_COLLISION_FACE)
		{
			minDist = etd.distance;
			pBoxTraceData->contacts[pBoxTraceData->numContacts++] = etd.contact1;
		}
		if (etd.usesBothContacts)
		{
			if (etd.distance < (minDist - ST_COLLIDER_TOLERANCE))
			{
				minDist = etd.distance;
			}
			if (etd.contact2.collisionType == ST_COLLISION_FACE)
			{
				pBoxTraceData->contacts[pBoxTraceData->numContacts++] = etd.contact2;
			}
		}
	}

	if (sphereTraceColliderImposedPlaneEdgeTrace1(&pPlaneCollider->transformedEdges[PLANE_EDGE_FORWARD], &negDir, pPlanePos, pPlaneNormal, pPlaneRight, pPlaneForward, xHalfExtents, zHalfExtents, &etd))
	{
		if (!etd.usesBothContacts)
		{
			if (etd.hitIndex == 0)
			{
				etd.contact1.point = pPlaneCollider->transformedEdges[PLANE_EDGE_FORWARD].point1;
				etd.contact1.normal = sphereTraceDirectionNegative(etd.contact1.normal);
			}
			else
			{
				etd.contact1.point = pPlaneCollider->transformedEdges[PLANE_EDGE_FORWARD].point2;
				etd.contact1.normal = sphereTraceDirectionNegative(etd.contact1.normal);
			}
		}
		else
		{
			etd.contact1.point = pPlaneCollider->transformedEdges[PLANE_EDGE_FORWARD].point1;
			etd.contact2.point = pPlaneCollider->transformedEdges[PLANE_EDGE_FORWARD].point2;
			etd.contact1.normal = sphereTraceDirectionNegative(etd.contact1.normal);
			etd.contact2.normal = sphereTraceDirectionNegative(etd.contact2.normal);
		}
		if (etd.contact1.collisionType == ST_COLLISION_FACE)
		{
			if (etd.distance < (minDist - ST_COLLIDER_TOLERANCE))
			{
				minDist = etd.distance;
				pBoxTraceData->numContacts = 0;
				pBoxTraceData->contacts[pBoxTraceData->numContacts++] = etd.contact1;
			}
			else if (sphereTraceEpsilonEqual(etd.distance, minDist, ST_COLLIDER_TOLERANCE))
			{
				pBoxTraceData->contacts[pBoxTraceData->numContacts++] = etd.contact1;
			}
		}
		if (etd.usesBothContacts)
		{
			if (etd.distance < (minDist - ST_COLLIDER_TOLERANCE))
			{
				minDist = etd.distance;
			}
			if (etd.contact2.collisionType == ST_COLLISION_FACE)
			{
				pBoxTraceData->contacts[pBoxTraceData->numContacts++] = etd.contact2;
			}
		}
	}

	ST_Edge edge = sphereTraceEdgeConstruct1(sphereTraceVector3AddAndScale2(*pPlanePos, pPlaneRight->v, -xHalfExtents, pPlaneForward->v, zHalfExtents),
		sphereTraceVector3AddAndScale2(*pPlanePos, pPlaneRight->v, xHalfExtents, pPlaneForward->v, zHalfExtents), 2.0f * xHalfExtents, *pPlaneRight);
	if (sphereTraceColliderPlaneEdgeTrace(&edge, *pDir, pPlaneCollider, &etd))
	{
		if (etd.distance < (minDist - ST_COLLIDER_TOLERANCE))
		{
			minDist = etd.distance;
			pBoxTraceData->numContacts = 0;
			pBoxTraceData->contacts[pBoxTraceData->numContacts++] = etd.contact1;
		}
		else if (sphereTraceEpsilonEqual(etd.distance, minDist, ST_COLLIDER_TOLERANCE))
		{
			pBoxTraceData->contacts[pBoxTraceData->numContacts++] = etd.contact1;
		}

		if (etd.usesBothContacts)
		{
			pBoxTraceData->contacts[pBoxTraceData->numContacts++] = etd.contact2;
		}
	}

	edge = sphereTraceEdgeConstruct1(sphereTraceVector3AddAndScale2(*pPlanePos, pPlaneRight->v, xHalfExtents, pPlaneForward->v, -zHalfExtents),
		sphereTraceVector3AddAndScale2(*pPlanePos, pPlaneRight->v, xHalfExtents, pPlaneForward->v, zHalfExtents), 2.0f * zHalfExtents, *pPlaneForward);
	if (sphereTraceColliderPlaneEdgeTrace(&edge, *pDir, pPlaneCollider, &etd))
	{
		if (etd.distance < (minDist - ST_COLLIDER_TOLERANCE))
		{
			minDist = etd.distance;
			pBoxTraceData->numContacts = 0;
			pBoxTraceData->contacts[pBoxTraceData->numContacts++] = etd.contact1;
		}
		else if (sphereTraceEpsilonEqual(etd.distance, minDist, ST_COLLIDER_TOLERANCE))
		{
			pBoxTraceData->contacts[pBoxTraceData->numContacts++] = etd.contact1;
		}

		if (etd.usesBothContacts)
		{
			pBoxTraceData->contacts[pBoxTraceData->numContacts++] = etd.contact2;
		}
	}

	edge = sphereTraceEdgeConstruct1(sphereTraceVector3AddAndScale2(*pPlanePos, pPlaneRight->v, -xHalfExtents, pPlaneForward->v, -zHalfExtents),
		sphereTraceVector3AddAndScale2(*pPlanePos, pPlaneRight->v, xHalfExtents, pPlaneForward->v, -zHalfExtents), 2.0f * xHalfExtents, *pPlaneRight);
	if (sphereTraceColliderPlaneEdgeTrace(&edge, *pDir, pPlaneCollider, &etd))
	{
		if (etd.distance < (minDist - ST_COLLIDER_TOLERANCE))
		{
			minDist = etd.distance;
			pBoxTraceData->numContacts = 0;
			pBoxTraceData->contacts[pBoxTraceData->numContacts++] = etd.contact1;
		}
		else if (sphereTraceEpsilonEqual(etd.distance, minDist, ST_COLLIDER_TOLERANCE))
		{
			pBoxTraceData->contacts[pBoxTraceData->numContacts++] = etd.contact1;
		}

		if (etd.usesBothContacts)
		{
			pBoxTraceData->contacts[pBoxTraceData->numContacts++] = etd.contact2;
		}
	}

	edge = sphereTraceEdgeConstruct1(sphereTraceVector3AddAndScale2(*pPlanePos, pPlaneRight->v, -xHalfExtents, pPlaneForward->v, -zHalfExtents),
		sphereTraceVector3AddAndScale2(*pPlanePos, pPlaneRight->v, -xHalfExtents, pPlaneForward->v, zHalfExtents), 2.0f * zHalfExtents, *pPlaneForward);
	if (sphereTraceColliderPlaneEdgeTrace(&edge, *pDir, pPlaneCollider, &etd))
	{
		if (etd.distance < (minDist - ST_COLLIDER_TOLERANCE))
		{
			minDist = etd.distance;
			pBoxTraceData->numContacts = 0;
			pBoxTraceData->contacts[pBoxTraceData->numContacts++] = etd.contact1;
		}
		else if (sphereTraceEpsilonEqual(etd.distance, minDist, ST_COLLIDER_TOLERANCE))
		{
			pBoxTraceData->contacts[pBoxTraceData->numContacts++] = etd.contact1;
		}

		if (etd.usesBothContacts)
		{
			pBoxTraceData->contacts[pBoxTraceData->numContacts++] = etd.contact2;
		}
	}

	if (minDist < FLT_MAX)
	{
		pBoxTraceData->traceDistance = minDist;
		pBoxTraceData->boxCenter = sphereTraceVector3AddAndScale(*pPlanePos, pDir->v, minDist);
		return ST_TRUE;
	}
	return ST_FALSE;
}