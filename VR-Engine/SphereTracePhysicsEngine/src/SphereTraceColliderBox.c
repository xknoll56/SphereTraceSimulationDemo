#include "SphereTraceColliderBox.h"
#include "SphereTraceGlobals.h"

#define ST_SMOOTH_DIST 0.1f

void sphereTraceColliderBoxUpdateLocalAxis(ST_BoxCollider* const pBoxCollider)
{
	pBoxCollider->localRight = sphereTraceDirectionGetLocalXAxisFromRotationMatrix(pBoxCollider->rigidBody.rotationMatrix);
	pBoxCollider->localUp = sphereTraceDirectionGetLocalYAxisFromRotationMatrix(pBoxCollider->rigidBody.rotationMatrix);
	pBoxCollider->localForward = sphereTraceDirectionGetLocalZAxisFromRotationMatrix(pBoxCollider->rigidBody.rotationMatrix);
	sphereTraceDirectionNormalizeIfNotNormalizedByRef(&pBoxCollider->localRight);
	sphereTraceDirectionNormalizeIfNotNormalizedByRef(&pBoxCollider->localUp);
	sphereTraceDirectionNormalizeIfNotNormalizedByRef(&pBoxCollider->localForward);
}

void sphereTraceColliderBoxUpdateTransformedVertices(ST_BoxCollider* const pBoxCollider)
{
	pBoxCollider->transformedVertices[ST_LEFT_DOWN_BACK] = sphereTraceVector3AddAndScale3(pBoxCollider->rigidBody.position, pBoxCollider->localRight.v, -pBoxCollider->halfExtents.x,
		pBoxCollider->localUp.v, -pBoxCollider->halfExtents.y, pBoxCollider->localForward.v, -pBoxCollider->halfExtents.z);
	pBoxCollider->transformedVertices[ST_RIGHT_DOWN_BACK] = sphereTraceVector3AddAndScale3(pBoxCollider->rigidBody.position, pBoxCollider->localRight.v, pBoxCollider->halfExtents.x,
		pBoxCollider->localUp.v, -pBoxCollider->halfExtents.y, pBoxCollider->localForward.v, -pBoxCollider->halfExtents.z);
	pBoxCollider->transformedVertices[ST_LEFT_DOWN_FORWARD] = sphereTraceVector3AddAndScale3(pBoxCollider->rigidBody.position, pBoxCollider->localRight.v, -pBoxCollider->halfExtents.x,
		pBoxCollider->localUp.v, -pBoxCollider->halfExtents.y, pBoxCollider->localForward.v, pBoxCollider->halfExtents.z);
	pBoxCollider->transformedVertices[ST_RIGHT_DOWN_FORWARD] = sphereTraceVector3AddAndScale3(pBoxCollider->rigidBody.position, pBoxCollider->localRight.v, pBoxCollider->halfExtents.x,
		pBoxCollider->localUp.v, -pBoxCollider->halfExtents.y, pBoxCollider->localForward.v, pBoxCollider->halfExtents.z);
	pBoxCollider->transformedVertices[ST_LEFT_UP_BACK] = sphereTraceVector3AddAndScale3(pBoxCollider->rigidBody.position, pBoxCollider->localRight.v, -pBoxCollider->halfExtents.x,
		pBoxCollider->localUp.v, pBoxCollider->halfExtents.y, pBoxCollider->localForward.v, -pBoxCollider->halfExtents.z);
	pBoxCollider->transformedVertices[ST_RIGHT_UP_BACK] = sphereTraceVector3AddAndScale3(pBoxCollider->rigidBody.position, pBoxCollider->localRight.v, pBoxCollider->halfExtents.x,
		pBoxCollider->localUp.v, pBoxCollider->halfExtents.y, pBoxCollider->localForward.v, -pBoxCollider->halfExtents.z);
	pBoxCollider->transformedVertices[ST_LEFT_UP_FORWARD] = sphereTraceVector3AddAndScale3(pBoxCollider->rigidBody.position, pBoxCollider->localRight.v, -pBoxCollider->halfExtents.x,
		pBoxCollider->localUp.v, pBoxCollider->halfExtents.y, pBoxCollider->localForward.v, pBoxCollider->halfExtents.z);
	pBoxCollider->transformedVertices[ST_RIGHT_UP_FORWARD] = sphereTraceVector3AddAndScale3(pBoxCollider->rigidBody.position, pBoxCollider->localRight.v, pBoxCollider->halfExtents.x,
		pBoxCollider->localUp.v, pBoxCollider->halfExtents.y, pBoxCollider->localForward.v, pBoxCollider->halfExtents.z);
}

void sphereTraceColliderImposedBoxFillVertices(ST_Vector3* const pImposedPosition, ST_Vector3* const pHalfExtents, ST_Direction* const pLocalRight,
	ST_Direction* const pLocalUp, ST_Direction* const pLocalForward, ST_Vector3 verts[8])
{
	verts[0] = sphereTraceVector3AddAndScale3(*pImposedPosition, pLocalRight->v, -pHalfExtents->x,
			pLocalUp->v, -pHalfExtents->y, pLocalForward->v, -pHalfExtents->z);
	verts[1] = sphereTraceVector3AddAndScale3(*pImposedPosition, pLocalRight->v, pHalfExtents->x,
			pLocalUp->v, -pHalfExtents->y, pLocalForward->v, -pHalfExtents->z);
	verts[2] = sphereTraceVector3AddAndScale3(*pImposedPosition, pLocalRight->v, -pHalfExtents->x,
			pLocalUp->v, -pHalfExtents->y, pLocalForward->v, pHalfExtents->z);
	verts[3] = sphereTraceVector3AddAndScale3(*pImposedPosition, pLocalRight->v, pHalfExtents->x,
			pLocalUp->v, -pHalfExtents->y, pLocalForward->v, pHalfExtents->z);
	verts[4] = sphereTraceVector3AddAndScale3(*pImposedPosition, pLocalRight->v, -pHalfExtents->x,
			pLocalUp->v, pHalfExtents->y, pLocalForward->v, -pHalfExtents->z);
	verts[5] = sphereTraceVector3AddAndScale3(*pImposedPosition, pLocalRight->v, pHalfExtents->x,
			pLocalUp->v, pHalfExtents->y, pLocalForward->v, -pHalfExtents->z);
	verts[6] = sphereTraceVector3AddAndScale3(*pImposedPosition, pLocalRight->v, -pHalfExtents->x,
			pLocalUp->v, pHalfExtents->y, pLocalForward->v, pHalfExtents->z);
	verts[7] = sphereTraceVector3AddAndScale3(*pImposedPosition, pLocalRight->v, pHalfExtents->x,
			pLocalUp->v, pHalfExtents->y, pLocalForward->v, pHalfExtents->z);
}

ST_Vector3 sphereTraceColliderImposedBoxGetVertexPositionFromOctant(ST_Vector3* const pImposedPosition, ST_Vector3* const pHalfExtents, ST_Direction* const pLocalRight,
	ST_Direction* const pLocalUp, ST_Direction* const pLocalForward, ST_Octant octant)
{
	switch (octant)
	{
	case ST_LEFT_DOWN_BACK:
		return sphereTraceVector3AddAndScale3(*pImposedPosition, pLocalRight->v, -pHalfExtents->x,
			pLocalUp->v, -pHalfExtents->y, pLocalForward->v, -pHalfExtents->z);
		break;
	case ST_RIGHT_DOWN_BACK:
		return sphereTraceVector3AddAndScale3(*pImposedPosition, pLocalRight->v, pHalfExtents->x,
			pLocalUp->v, -pHalfExtents->y, pLocalForward->v, -pHalfExtents->z);
		break;
	case ST_LEFT_DOWN_FORWARD:
		return sphereTraceVector3AddAndScale3(*pImposedPosition, pLocalRight->v, -pHalfExtents->x,
			pLocalUp->v, -pHalfExtents->y, pLocalForward->v, pHalfExtents->z);
		break;
	case ST_RIGHT_DOWN_FORWARD:
		return sphereTraceVector3AddAndScale3(*pImposedPosition, pLocalRight->v, pHalfExtents->x,
			pLocalUp->v, -pHalfExtents->y, pLocalForward->v, pHalfExtents->z);
		break;
	case ST_LEFT_UP_BACK:
		return sphereTraceVector3AddAndScale3(*pImposedPosition, pLocalRight->v, -pHalfExtents->x,
			pLocalUp->v, pHalfExtents->y, pLocalForward->v, -pHalfExtents->z);
		break;
	case ST_RIGHT_UP_BACK:
		return sphereTraceVector3AddAndScale3(*pImposedPosition, pLocalRight->v, pHalfExtents->x,
			pLocalUp->v, pHalfExtents->y, pLocalForward->v, -pHalfExtents->z);
		break;
	case ST_LEFT_UP_FORWARD:
		return sphereTraceVector3AddAndScale3(*pImposedPosition, pLocalRight->v, -pHalfExtents->x,
			pLocalUp->v, pHalfExtents->y, pLocalForward->v, pHalfExtents->z);
		break;
	case ST_RIGHT_UP_FORWARD:
		return sphereTraceVector3AddAndScale3(*pImposedPosition, pLocalRight->v, pHalfExtents->x,
			pLocalUp->v, pHalfExtents->y, pLocalForward->v, pHalfExtents->z);
		break;
	} 
}


ST_BoxCollider sphereTraceColliderBoxConstruct(ST_Vector3 halfExtents)
{
	ST_BoxCollider bc;
	bc.collider = sphereTraceColliderConstruct(COLLIDER_BOX, sphereTraceVector3Length(halfExtents));
	bc.rigidBody = sphereTraceRigidBodyConstruct(1.0f, 1.0f);
	bc.minDimension = sphereTraceMin(halfExtents.x, sphereTraceMin(halfExtents.y, halfExtents.z));
	bc.halfExtents = halfExtents;
	bc.ignoreCollisions = ST_FALSE;
	bc.restingContact = ST_FALSE;
	sphereTraceColliderBoxUpdateLocalAxis(&bc);
	sphereTraceColliderBoxUpdateTransformedVertices(&bc);
	//sphereTraceColliderBoxSetEdges(&bc);
	sphereTraceColliderBoxSetAABB(&bc);
	return bc;
}

void sphereTraceColliderBoxSetPosition(ST_BoxCollider* const pBoxCollider, ST_Vector3 position)
{
	pBoxCollider->rigidBody.position = position;
	sphereTraceColliderBoxUpdateTransformedVertices(pBoxCollider);
	sphereTraceColliderBoxSetAABB(pBoxCollider);
}

ST_Index sphereTraceColliderBoxGetVertexIndexClosestToDirection(const ST_Direction* pRight, const ST_Direction* pUp, const ST_Direction* pForward, const ST_Direction dir)
{
	float dotRight = sphereTraceVector3Dot(dir.v, pRight->v);
	float dotUp = sphereTraceVector3Dot(dir.v, pUp->v);
	float dotForward = sphereTraceVector3Dot(dir.v, pForward->v);
	if (dotRight >= 0.0f)
	{
		if (dotUp >= 0.0f)
		{
			if (dotForward >= 0.0f)
			{
				return ST_RIGHT_UP_FORWARD;
			}
			else
			{
				return ST_RIGHT_UP_BACK;
			}
		}
		else
		{
			if (dotForward >= 0.0f)
			{
				return ST_RIGHT_DOWN_FORWARD;
			}
			else
			{
				return ST_RIGHT_DOWN_BACK;
			}
		}
	}
	else
	{
		if (dotUp >= 0.0f)
		{
			if (dotForward >= 0.0f)
			{
				return ST_LEFT_UP_FORWARD;
			}
			else
			{
				return ST_LEFT_UP_BACK;
			}
		}
		else
		{
			if (dotForward >= 0.0f)
			{
				return ST_LEFT_DOWN_FORWARD;
			}
			else
			{
				return ST_LEFT_DOWN_BACK;
			}
		}
	}
}

ST_Vector3 sphereTraceColliderBoxGetUnitVertexClosestToDirection(const ST_BoxCollider* const pBoxCollider, const ST_Direction dir)
{
	float dotRight = sphereTraceVector3Dot(dir.v, pBoxCollider->localRight.v);
	float dotUp = sphereTraceVector3Dot(dir.v, pBoxCollider->localUp.v);
	float dotForward = sphereTraceVector3Dot(dir.v, pBoxCollider->localForward.v);
	ST_Vector3 ret = sphereTraceVector3Scale(pBoxCollider->localRight.v, sphereTraceSign(dotRight));
	return sphereTraceVector3AddAndScale2(ret, pBoxCollider->localUp.v, sphereTraceSign(dotUp),
		pBoxCollider->localForward.v, sphereTraceSign(dotForward));
}

ST_BoxFace sphereTraceColliderBoxGetFaceClosestToDirection(const ST_BoxCollider* const pBoxCollider, const ST_Direction dir)
{
	float dotRight = sphereTraceVector3Dot(dir.v, pBoxCollider->localRight.v);
	float dotUp = sphereTraceVector3Dot(dir.v, pBoxCollider->localUp.v);
	float dotForward = sphereTraceVector3Dot(dir.v, pBoxCollider->localForward.v);
	if (sphereTraceAbs(dotRight) > sphereTraceAbs(dotUp))
	{
		//right/left closer that up
		if (sphereTraceAbs(dotRight) > sphereTraceAbs(dotForward))
		{
			//right/left closer than forward
			if (dotRight > 0.0f)
			{
				return gFaceRight;
			}
			else
			{
				return gFaceLeft;
			}
		}
		else
		{
			if (dotForward > 0.0f)
			{
				return gFaceForward;
			}
			else
			{
				return gFaceBack;
			}
		}
	}
	else
	{
		if (sphereTraceAbs(dotUp) > sphereTraceAbs(dotForward))
		{
			if (dotUp > 0.0f)
			{
				return gFaceUp;
			}
			else
			{
				return gFaceDown;
			}
		}
		else
		{
			if (dotForward > 0.0f)
			{
				return gFaceForward;
			}
			else
			{
				return gFaceBack;
			}
		}
	}
}



ST_BoxFace sphereTraceColliderBoxFaceFromDirection(ST_DirectionType dir)
{
	switch (dir)
	{
	case ST_DIRECTION_RIGHT:
	{
		return gFaceRight;
	}
	break;
	case ST_DIRECTION_LEFT:
	{
		return gFaceLeft;
	}
	break;
	case ST_DIRECTION_UP:
	{
		return gFaceUp;
	}
	break;
	case ST_DIRECTION_DOWN:
	{
		return gFaceDown;
	}
	break;
	case ST_DIRECTION_FORWARD:
	{
		return gFaceForward;
	}
	break;
	case ST_DIRECTION_BACK:
	{
		return gFaceBack;
	}
	break;
	}
}

ST_Direction sphereTraceColliderBoxFaceGetNormal(ST_BoxCollider* const pBoxCollider, ST_BoxFace* const pBoxFace)
{
	switch (pBoxFace->dir)
	{
	case ST_DIRECTION_RIGHT:
	{
		return pBoxCollider->localRight;
	}
	break;
	case ST_DIRECTION_LEFT:
	{
		return sphereTraceDirectionNegative(pBoxCollider->localRight);
	}
	break;
	case ST_DIRECTION_UP:
	{
		return pBoxCollider->localUp;
	}
	break;
	case ST_DIRECTION_DOWN:
	{
		return sphereTraceDirectionNegative(pBoxCollider->localUp);
	}
	break;
	case ST_DIRECTION_FORWARD:
	{
		return pBoxCollider->localForward;
	}
	break;
	case ST_DIRECTION_BACK:
	{
		return sphereTraceDirectionNegative(pBoxCollider->localForward);
	}
	break;
	}
}

b32 sphereTraceColliderBoxFaceIsDirectionNormal(const ST_BoxFace* const pBoxFace, const ST_DirectionType dir)
{
	switch (pBoxFace->dir)
	{
	case ST_DIRECTION_RIGHT:
	{
		return (dir == ST_DIRECTION_RIGHT || dir == ST_DIRECTION_LEFT);
	}
	break;
	case ST_DIRECTION_LEFT:
	{
		return (dir == ST_DIRECTION_RIGHT || dir == ST_DIRECTION_LEFT);
	}
	break;
	case ST_DIRECTION_UP:
	{
		return (dir == ST_DIRECTION_UP || dir == ST_DIRECTION_DOWN);
	}
	break;
	case ST_DIRECTION_DOWN:
	{
		return (dir == ST_DIRECTION_UP || dir == ST_DIRECTION_DOWN);
	}
	break;
	case ST_DIRECTION_FORWARD:
	{
		return (dir == ST_DIRECTION_FORWARD || dir == ST_DIRECTION_BACK);
	}
	break;
	case ST_DIRECTION_BACK:
	{
		return (dir == ST_DIRECTION_FORWARD || dir == ST_DIRECTION_BACK);
	}
	break;
	}
}

void sphereTraceColliderBoxSetAABB(ST_BoxCollider* const pBoxCollider)
{
	ST_BoxFace rightFace = sphereTraceColliderBoxGetFaceClosestToDirection(pBoxCollider, gDirectionRight);
	ST_BoxFace upFace = sphereTraceColliderBoxGetFaceClosestToDirection(pBoxCollider, gDirectionUp);
	ST_BoxFace forwardFace = sphereTraceColliderBoxGetFaceClosestToDirection(pBoxCollider, gDirectionForward);
	pBoxCollider->collider.aabb.halfExtents = sphereTraceVector3Subtract(sphereTraceVector3Construct(sphereTraceMax(pBoxCollider->transformedVertices[rightFace.vertexIndices[0]].x,
		sphereTraceMax(pBoxCollider->transformedVertices[rightFace.vertexIndices[1]].x,
			sphereTraceMax(pBoxCollider->transformedVertices[rightFace.vertexIndices[2]].x, pBoxCollider->transformedVertices[rightFace.vertexIndices[3]].x))),
		sphereTraceMax(pBoxCollider->transformedVertices[upFace.vertexIndices[0]].y,
			sphereTraceMax(pBoxCollider->transformedVertices[upFace.vertexIndices[1]].y,
				sphereTraceMax(pBoxCollider->transformedVertices[upFace.vertexIndices[2]].y, pBoxCollider->transformedVertices[upFace.vertexIndices[3]].y))),
		sphereTraceMax(pBoxCollider->transformedVertices[forwardFace.vertexIndices[0]].z,
			sphereTraceMax(pBoxCollider->transformedVertices[forwardFace.vertexIndices[1]].z,
				sphereTraceMax(pBoxCollider->transformedVertices[forwardFace.vertexIndices[2]].z, pBoxCollider->transformedVertices[forwardFace.vertexIndices[3]].z)))),
		pBoxCollider->rigidBody.position);
	pBoxCollider->collider.aabb.center = pBoxCollider->rigidBody.position;
	pBoxCollider->collider.aabb.lowExtent = sphereTraceVector3Subtract(pBoxCollider->collider.aabb.center, pBoxCollider->collider.aabb.halfExtents);
	pBoxCollider->collider.aabb.highExtent = sphereTraceVector3Add(pBoxCollider->collider.aabb.center, pBoxCollider->collider.aabb.halfExtents);

}

void sphereTraceColliderBoxUpdateTransform(ST_BoxCollider* const pBoxCollider)
{
	sphereTraceColliderBoxUpdateLocalAxis(pBoxCollider);
	sphereTraceColliderBoxUpdateTransformedVertices(pBoxCollider);
	sphereTraceColliderBoxSetAABB(pBoxCollider);
}

float sphereTraceColliderImposedBoxEdgeDirectionToLength(ST_Vector3 halfExtents, ST_DirectionType dir)
{
	switch (dir)
	{
	case ST_DIRECTION_RIGHT:
		return 2.0f*halfExtents.x;
		break;
	case ST_DIRECTION_LEFT:
		return 2.0f * halfExtents.x;
		break;
	case ST_DIRECTION_UP:
		return 2.0f * halfExtents.y;
		break;
	case ST_DIRECTION_DOWN:
		return 2.0f * halfExtents.y;
		break;
	case ST_DIRECTION_FORWARD:
		return 2.0f * halfExtents.z;
		break;
	case ST_DIRECTION_BACK:
		return 2.0f * halfExtents.z;
		break;
	}
}

ST_Direction sphereTraceColliderImposedBoxEdgeDirectionToDirection(ST_Direction* const pRight, ST_Direction* const pUp, ST_Direction* const pForward, ST_DirectionType dir)
{
	switch (dir)
	{
	case ST_DIRECTION_RIGHT:
		return *pRight;
		break;
	case ST_DIRECTION_LEFT:
		return sphereTraceDirectionNegative(*pRight);
		break;
	case ST_DIRECTION_UP:
		return *pUp;
		break;
	case ST_DIRECTION_DOWN:
		return sphereTraceDirectionNegative(*pUp);
		break;
	case ST_DIRECTION_FORWARD:
		return *pForward;
		break;
	case ST_DIRECTION_BACK:
		return sphereTraceDirectionNegative(*pForward);
		break;
	}
}

b32 sphereTraceColliderPlaneBoxTrace(ST_Vector3 start, ST_Vector3 halfExtents, ST_Quaternion rotation, ST_Direction dir, ST_PlaneCollider* const pPlaneCollider, ST_BoxTraceData* const pBoxTraceData)
{
	ST_Matrix4 rotationMatrix = sphereTraceMatrixFromQuaternion(rotation);
	ST_Direction localRight = sphereTraceDirectionGetLocalXAxisFromRotationMatrix(rotationMatrix);
	ST_Direction localUp = sphereTraceDirectionGetLocalYAxisFromRotationMatrix(rotationMatrix);
	ST_Direction localForward = sphereTraceDirectionGetLocalZAxisFromRotationMatrix(rotationMatrix);
	ST_Vector3 dp = sphereTraceVector3Subtract(pPlaneCollider->position, start);
	float dot = sphereTraceVector3Dot(dp, pPlaneCollider->normal.v);
	ST_Direction planeNormal = pPlaneCollider->normal;
	if (dot > 0.0f)
	{
		planeNormal = sphereTraceDirectionNegative(planeNormal);
	}
	float dirDotRight = sphereTraceVector3Dot(dir.v, localRight.v);
	float dirDotUp = sphereTraceVector3Dot(dir.v, localUp.v);
	float dirDotForward = sphereTraceVector3Dot(dir.v, localForward.v);
	ST_BoxFace rightFace;
	if (dirDotRight >= 0.0f)
		rightFace = gFaceRight;
	else
		rightFace = gFaceLeft;
	ST_BoxFace upFace;
	if (dirDotUp >= 0.0f)
		upFace = gFaceUp;
	else
		upFace = gFaceDown;
	ST_BoxFace forwardFace;
	if (dirDotForward >= 0.0f)
		forwardFace = gFaceForward;
	else
		forwardFace = gFaceBack;
	ST_BoxTraceData btd;
	float minDist = FLT_MAX;
	if (sphereTraceColliderPlanePlaneTrace(sphereTraceVector3AddAndScale(start, sphereTraceColliderImposedBoxEdgeDirectionToDirection(&localRight, &localUp, &localForward, rightFace.dir).v, halfExtents.x),
		sphereTraceColliderImposedBoxEdgeDirectionToDirection(&localRight, &localUp, &localForward, rightFace.dir), localForward, localUp, halfExtents.z, halfExtents.y, dir, pPlaneCollider, &btd))
	{
		if (btd.traceDistance <= minDist)
		{
			minDist = btd.traceDistance;
			*pBoxTraceData = btd;
		}
	}
	if (sphereTraceColliderPlanePlaneTrace(sphereTraceVector3AddAndScale(start, sphereTraceColliderImposedBoxEdgeDirectionToDirection(&localRight, &localUp, &localForward, upFace.dir).v, halfExtents.y),
		sphereTraceColliderImposedBoxEdgeDirectionToDirection(&localRight, &localUp, &localForward, upFace.dir), localRight, localForward, halfExtents.x, halfExtents.z, dir, pPlaneCollider, &btd))
	{
		if (btd.traceDistance < minDist)
		{
			minDist = btd.traceDistance;
			*pBoxTraceData = btd;
		}
		else if (btd.traceDistance == minDist && btd.numContacts > pBoxTraceData->numContacts)
		{
			minDist = btd.traceDistance;
			*pBoxTraceData = btd;
		}
	}
	if (sphereTraceColliderPlanePlaneTrace(sphereTraceVector3AddAndScale(start, sphereTraceColliderImposedBoxEdgeDirectionToDirection(&localRight, &localUp, &localForward, forwardFace.dir).v, halfExtents.z),
		sphereTraceColliderImposedBoxEdgeDirectionToDirection(&localRight, &localUp, &localForward, forwardFace.dir), localRight, localUp, halfExtents.x, halfExtents.y, dir, pPlaneCollider, &btd))
	{
		if (btd.traceDistance < minDist)
		{
			minDist = btd.traceDistance;
			*pBoxTraceData = btd;
		}
		else if (btd.traceDistance == minDist && btd.numContacts > pBoxTraceData->numContacts)
		{
			minDist = btd.traceDistance;
			*pBoxTraceData = btd;
		}
	}
	if (minDist < FLT_MAX)
	{
		pBoxTraceData->traceDistance = minDist;
		pBoxTraceData->boxCenter = sphereTraceVector3AddAndScale(start, dir.v, minDist);
		return ST_TRUE;
	}
	return ST_FALSE;
}

b32 sphereTraceColliderPlaneBoxTrace1(ST_Vector3* const pStart, ST_Vector3* const pHalfExtents, ST_Direction* const pLocalRight, ST_Direction* const pLocalUp,
	ST_Direction* const pLocalForward, ST_Direction* const pDir, ST_PlaneCollider* const pPlaneCollider, ST_BoxTraceData* const pBoxTraceData)
{
	ST_Vector3 dp = sphereTraceVector3Subtract(pPlaneCollider->position, *pStart);
	float dot = sphereTraceVector3Dot(dp, pPlaneCollider->normal.v);
	ST_Direction planeNormal = pPlaneCollider->normal;
	if (dot > 0.0f)
	{
		planeNormal = sphereTraceDirectionNegative(planeNormal);
	}
	float dirDotRight = sphereTraceVector3Dot(pDir->v, pLocalRight->v);
	float dirDotUp = sphereTraceVector3Dot(pDir->v, pLocalUp->v);
	float dirDotForward = sphereTraceVector3Dot(pDir->v, pLocalForward->v);
	ST_BoxFace rightFace;
	if (dirDotRight >= 0.0f)
		rightFace = gFaceRight;
	else
		rightFace = gFaceLeft;
	ST_BoxFace upFace;
	if (dirDotUp >= 0.0f)
		upFace = gFaceUp;
	else
		upFace = gFaceDown;
	ST_BoxFace forwardFace;
	if (dirDotForward >= 0.0f)
		forwardFace = gFaceForward;
	else
		forwardFace = gFaceBack;
	ST_BoxTraceData btd;
	float minDist = FLT_MAX;
	ST_Direction normalDir = sphereTraceColliderImposedBoxEdgeDirectionToDirection(pLocalRight, pLocalUp, pLocalForward, rightFace.dir);
	ST_Vector3 startPos = sphereTraceVector3AddAndScale(*pStart, normalDir.v, pHalfExtents->x);
	if (sphereTraceColliderPlanePlaneTrace(startPos, normalDir, *pLocalForward, *pLocalUp, pHalfExtents->z, pHalfExtents->y, *pDir, pPlaneCollider, &btd))
	{
		if (btd.traceDistance <= minDist)
		{
			minDist = btd.traceDistance;
			*pBoxTraceData = btd;
		}
	}
	normalDir = sphereTraceColliderImposedBoxEdgeDirectionToDirection(pLocalRight, pLocalUp, pLocalForward, upFace.dir);
	startPos = sphereTraceVector3AddAndScale(*pStart, normalDir.v, pHalfExtents->y);
	if (sphereTraceColliderPlanePlaneTrace(startPos, normalDir, *pLocalRight, *pLocalForward, pHalfExtents->x, pHalfExtents->z, *pDir, pPlaneCollider, &btd))
	{
		if (btd.traceDistance < minDist)
		{
			minDist = btd.traceDistance;
			*pBoxTraceData = btd;
		}
		else if (btd.traceDistance == minDist && btd.numContacts > pBoxTraceData->numContacts)
		{
			minDist = btd.traceDistance;
			*pBoxTraceData = btd;
		}
	}
	normalDir = sphereTraceColliderImposedBoxEdgeDirectionToDirection(pLocalRight, pLocalUp, pLocalForward, forwardFace.dir);
	startPos = sphereTraceVector3AddAndScale(*pStart, normalDir.v, pHalfExtents->z);
	if (sphereTraceColliderPlanePlaneTrace(startPos, normalDir, *pLocalRight, *pLocalUp, pHalfExtents->x, pHalfExtents->y, *pDir, pPlaneCollider, &btd))
	{
		if (btd.traceDistance < minDist)
		{
			minDist = btd.traceDistance;
			*pBoxTraceData = btd;
		}
		else if (btd.traceDistance == minDist && btd.numContacts > pBoxTraceData->numContacts)
		{
			minDist = btd.traceDistance;
			*pBoxTraceData = btd;
		}
	}
	if (minDist < FLT_MAX)
	{
		pBoxTraceData->traceDistance = minDist;
		pBoxTraceData->boxCenter = sphereTraceVector3AddAndScale(*pStart, pDir->v, minDist);
		return ST_TRUE;
	}
	return ST_FALSE;
}

void sphereTraceColliderBoxFaceGetTangentialDirections(const ST_BoxFace* const pBoxFace, ST_DirectionType* const pTangent1, ST_DirectionType* const pTangent2)
{
	switch (pBoxFace->dir)
	{
	case ST_DIRECTION_RIGHT:
		*pTangent1 = ST_DIRECTION_UP;
		*pTangent2 = ST_DIRECTION_FORWARD;
		break;
	case ST_DIRECTION_LEFT:
		*pTangent1 = ST_DIRECTION_UP;
		*pTangent2 = ST_DIRECTION_FORWARD;
		break;
	case ST_DIRECTION_UP:
		*pTangent1 = ST_DIRECTION_RIGHT;
		*pTangent2 = ST_DIRECTION_FORWARD;
		break;
	case ST_DIRECTION_DOWN:
		*pTangent1 = ST_DIRECTION_RIGHT;
		*pTangent2 = ST_DIRECTION_FORWARD;
		break;
	case ST_DIRECTION_FORWARD:
		*pTangent1 = ST_DIRECTION_RIGHT;
		*pTangent2 = ST_DIRECTION_UP;
		break;
	case ST_DIRECTION_BACK:
		*pTangent1 = ST_DIRECTION_RIGHT;
		*pTangent2 = ST_DIRECTION_UP;
		break;
	}
}

float sphereTraceColliderBoxGetEdgeHalfLength(const ST_BoxCollider* const pBox, ST_DirectionType dir)
{
	switch (dir)
	{
	case ST_DIRECTION_RIGHT:
		return pBox->halfExtents.x;
		break;
	case ST_DIRECTION_LEFT:
		return pBox->halfExtents.x;
		break;
	case ST_DIRECTION_UP:
		return pBox->halfExtents.y;
		break;
	case ST_DIRECTION_DOWN:
		return pBox->halfExtents.y;
		break;
	case ST_DIRECTION_FORWARD:
		return pBox->halfExtents.z;
		break;
	case ST_DIRECTION_BACK:
		return pBox->halfExtents.z;
		break;
	}
}

b32 sphereTraceColliderEdgeEdgeCollisionTest(ST_Edge* const pEdge, ST_Edge* const pOtherEdge, ST_ContactPoint* const pContact)
{
	if (sphereTraceEpsilonEqual(sphereTraceVector3Dot(pEdge->dir.v, pOtherEdge->dir.v), 1.0f, ST_COLLIDER_TOLERANCE))
		return ST_FALSE;
	ST_Vector3 cross = sphereTraceVector3Cross(pEdge->dir.v, pOtherEdge->dir.v);
	float crossSquared = sphereTraceVector3Length2(cross);
	float t = sphereTraceVector3Dot(sphereTraceVector3Cross(sphereTraceVector3Subtract(pOtherEdge->point1, pEdge->point1), pOtherEdge->dir.v), cross) / crossSquared;
	float s = sphereTraceVector3Dot(sphereTraceVector3Cross(sphereTraceVector3Subtract(pOtherEdge->point1, pEdge->point1), pEdge->dir.v), cross) / crossSquared;
	if (t >= 0.0f && t <= pEdge->dist)
	{
		if (s >= 0.0f && s <= pOtherEdge->dist)
		{
			pContact->point = sphereTraceVector3AddAndScale(pOtherEdge->point1, pOtherEdge->dir.v, s);
			pContact->normal = sphereTraceDirectionConstructNormalized(cross);
			if (sphereTraceVector3Dot(pContact->normal.v, sphereTraceVector3Subtract(pOtherEdge->point1, pEdge->point1)) < 0.0f)
				pContact->normal = sphereTraceDirectionNegative(pContact->normal);
			pContact->penetrationDistance = sphereTraceVector3Distance(pContact->point,
				sphereTraceVector3AddAndScale(pEdge->point1, pEdge->dir.v, t));
			return ST_TRUE;
		}
	}
	return ST_FALSE;
}

void sphereTraceColliderPlaneBoxFaceEdgeCollisionTest(ST_BoxCollider* const pBoxCollider, ST_ImposedPlane* pFacePlane,  float* minPen, float* maxPen, ST_PlaneCollider* const pPlaneCollider, ST_BoxContact* const pContact)
{
	ST_ContactPoint cp;
	ST_Direction inward1, inward2;
	inward1 = sphereTraceDirectionNegative(pFacePlane->normal);
	for (int i = 0; i < 4; i++)
	{
		switch (i)
		{
		case 0:
			inward2 = pFacePlane->forward;
			break;
		case 1:
			inward2 = sphereTraceDirectionNegative(pFacePlane->right);
			break;
		case 2:
			inward2 = pFacePlane->right;
			break;
		case 3:
			inward2 = sphereTraceDirectionNegative(pFacePlane->forward);
		}
		ST_PlaneEdgeDirection pe0 = sphereTraceColliderPlaneGetClosestTransformedEdgeToPoint(pPlaneCollider, pFacePlane->edges[i].point1);
		if (sphereTraceColliderEdgeEdgeCollisionTest(&pFacePlane->edges[i], &pPlaneCollider->transformedEdges[pe0], &cp))
		{
			if (sphereTraceVector3Dot(inward1.v, cp.normal.v) >= 0.0f && sphereTraceVector3Dot(inward2.v, cp.normal.v) >= 0.0f)
			{
				pContact->contactPoints[pContact->numContacts++] = cp;
				if (cp.penetrationDistance < *minPen)
					*minPen = cp.penetrationDistance;
				if (cp.penetrationDistance > *maxPen)
				{
					pContact->maxPenetrationIndex = pContact->numContacts - 1;
					*maxPen = cp.penetrationDistance;
				}
			}
		}
		ST_PlaneEdgeDirection pe1 = sphereTraceColliderPlaneGetClosestTransformedEdgeToPoint(pPlaneCollider, pFacePlane->edges[i].point2);
		if (pe0 != pe1)
		{
			if (sphereTraceColliderEdgeEdgeCollisionTest(&pFacePlane->edges[i], &pPlaneCollider->transformedEdges[pe1], &cp))
			{
				if (sphereTraceVector3Dot(inward1.v, cp.normal.v) >= 0.0f && sphereTraceVector3Dot(inward2.v, cp.normal.v) >= 0.0f)
				{
					pContact->contactPoints[pContact->numContacts++] = cp;
					if (cp.penetrationDistance < *minPen)
						*minPen = cp.penetrationDistance;
					if (cp.penetrationDistance > *maxPen)
					{
						pContact->maxPenetrationIndex = pContact->numContacts - 1;
						*maxPen = cp.penetrationDistance;
					}
				}
			}
		}
	}
	return minPen;
}

ST_ImposedPlane sphereTraceColliderBoxFaceToImposedPlane(ST_BoxCollider* const pBoxCollider, ST_BoxFace* const pBoxFace)
{
	ST_ImposedPlane ip;
	ip.normal = sphereTraceColliderBoxFaceGetNormal(pBoxCollider, pBoxFace);
	float normLen = sphereTraceColliderBoxGetEdgeHalfLength(pBoxCollider, pBoxFace->dir);
	ip.position = sphereTraceVector3AddAndScale(pBoxCollider->rigidBody.position, ip.normal.v, normLen);
	ST_DirectionType t1, t2;
	sphereTraceColliderBoxFaceGetTangentialDirections(pBoxFace, &t1, &t2);
	ip.right = sphereTraceColliderImposedBoxEdgeDirectionToDirection(&pBoxCollider->localRight, &pBoxCollider->localUp, &pBoxCollider->localForward, t1);
	ip.forward = sphereTraceColliderImposedBoxEdgeDirectionToDirection(&pBoxCollider->localRight, &pBoxCollider->localUp, &pBoxCollider->localForward, t2);
	ip.xHalfExtent = sphereTraceColliderBoxGetEdgeHalfLength(pBoxCollider, t1);
	ip.zHalfExtent = sphereTraceColliderBoxGetEdgeHalfLength(pBoxCollider, t2);
	ST_Octant o1 = pBoxFace->vertexIndices[0];
	ST_Octant o2 = sphereTraceOctantGetNextFromDirection(o1, t1);
	ST_Octant o3 = sphereTraceOctantGetNextFromDirection(o2, t2);
	ST_Octant o4 = sphereTraceOctantGetNextFromDirection(o1, t2);
	ip.edges[0] = sphereTraceEdgeConstruct1(pBoxCollider->transformedVertices[o1], pBoxCollider->transformedVertices[o2], ip.xHalfExtent * 2.0f, ip.right);
	ip.edges[1] = sphereTraceEdgeConstruct1(pBoxCollider->transformedVertices[o2], pBoxCollider->transformedVertices[o3], ip.zHalfExtent * 2.0f, ip.forward);
	ip.edges[2] = sphereTraceEdgeConstruct1(pBoxCollider->transformedVertices[o1], pBoxCollider->transformedVertices[o4], ip.zHalfExtent * 2.0f, ip.forward);
	ip.edges[3] = sphereTraceEdgeConstruct1(pBoxCollider->transformedVertices[o4], pBoxCollider->transformedVertices[o3], ip.xHalfExtent * 2.0f, ip.right);
	return ip;
}

void sphereTraceOctantCombineDirs(ST_DirectionType dir1, ST_DirectionType dir2, ST_Octant *po1, ST_Octant* po2)
{
	switch (dir1)
	{
	case ST_DIRECTION_RIGHT:
		switch (dir2)
		{
		case ST_DIRECTION_UP:
			*po1 = ST_RIGHT_UP_FORWARD;
			*po2 = ST_RIGHT_UP_BACK;
			break;
		case ST_DIRECTION_DOWN:
			*po1 = ST_RIGHT_DOWN_FORWARD;
			*po2 = ST_RIGHT_DOWN_BACK;
			break;
		case ST_DIRECTION_FORWARD:
			*po1 = ST_RIGHT_UP_FORWARD;
			*po2 = ST_RIGHT_DOWN_FORWARD;
			break;
		case ST_DIRECTION_BACK:
			*po1 = ST_RIGHT_UP_BACK;
			*po2 = ST_RIGHT_DOWN_BACK;
			break;
		}
	break;
	case ST_DIRECTION_LEFT:
		switch (dir2)
		{
		case ST_DIRECTION_UP:
			*po1 = ST_LEFT_UP_FORWARD;
			*po2 = ST_LEFT_UP_BACK;
			break;
		case ST_DIRECTION_DOWN:
			*po1 = ST_LEFT_DOWN_FORWARD;
			*po2 = ST_LEFT_DOWN_BACK;
			break;
		case ST_DIRECTION_FORWARD:
			*po1 = ST_LEFT_UP_FORWARD;
			*po2 = ST_LEFT_DOWN_FORWARD;
			break;
		case ST_DIRECTION_BACK:
			*po1 = ST_LEFT_UP_BACK;
			*po2 = ST_LEFT_DOWN_BACK;
			break;
		}
	break;
	case ST_DIRECTION_UP:
		switch (dir2)
		{
		case ST_DIRECTION_RIGHT:
			*po1 = ST_RIGHT_UP_FORWARD;
			*po2 = ST_RIGHT_UP_BACK;
			break;
		case ST_DIRECTION_LEFT:
			*po1 = ST_LEFT_UP_FORWARD;
			*po2 = ST_LEFT_UP_BACK;
			break;
		case ST_DIRECTION_FORWARD:
			*po1 = ST_RIGHT_UP_FORWARD;
			*po2 = ST_LEFT_UP_FORWARD;
			break;
		case ST_DIRECTION_BACK:
			*po1 = ST_RIGHT_UP_BACK;
			*po2 = ST_LEFT_UP_BACK;
			break;
		}
	break;
	case ST_DIRECTION_DOWN:
		switch (dir2)
		{
		case ST_DIRECTION_RIGHT:
			*po1 = ST_RIGHT_DOWN_FORWARD;
			*po2 = ST_RIGHT_DOWN_BACK;
			break;
		case ST_DIRECTION_LEFT:
			*po1 = ST_LEFT_DOWN_FORWARD;
			*po2 = ST_LEFT_DOWN_BACK;
			break;
		case ST_DIRECTION_FORWARD:
			*po1 = ST_RIGHT_DOWN_FORWARD;
			*po2 = ST_LEFT_DOWN_FORWARD;
			break;
		case ST_DIRECTION_BACK:
			*po1 = ST_RIGHT_DOWN_BACK;
			*po2 = ST_LEFT_DOWN_BACK;
			break;
		}
	break;
	case ST_DIRECTION_FORWARD:
		switch (dir2)
		{
		case ST_DIRECTION_RIGHT:
			*po1 = ST_RIGHT_UP_FORWARD;
			*po2 = ST_RIGHT_DOWN_FORWARD;
			break;
		case ST_DIRECTION_LEFT:
			*po1 = ST_LEFT_UP_FORWARD;
			*po2 = ST_LEFT_DOWN_FORWARD;
			break;
		case ST_DIRECTION_UP:
			*po1 = ST_RIGHT_UP_FORWARD;
			*po2 = ST_LEFT_UP_FORWARD;
			break;
		case ST_DIRECTION_DOWN:
			*po1 = ST_RIGHT_DOWN_FORWARD;
			*po2 = ST_LEFT_DOWN_FORWARD;
			break;
		}
	break;
	case ST_DIRECTION_BACK:
		switch (dir2)
		{
		case ST_DIRECTION_RIGHT:
			*po1 = ST_RIGHT_UP_BACK;
			*po2 = ST_RIGHT_DOWN_BACK;
			break;
		case ST_DIRECTION_LEFT:
			*po1 = ST_LEFT_UP_BACK;
			*po2 = ST_LEFT_DOWN_BACK;
			break;
		case ST_DIRECTION_UP:
			*po1 = ST_RIGHT_UP_BACK;
			*po2 = ST_LEFT_UP_BACK;
			break;
		case ST_DIRECTION_DOWN:
			*po1 = ST_RIGHT_DOWN_BACK;
			*po2 = ST_LEFT_DOWN_BACK;
			break;
		}
	break;
	}
}

void sphereTraceColliderPlaneBoxFaceTangentialEdgeCollisionTest(ST_BoxCollider* const pBoxCollider, ST_Octant vo1, ST_Octant vo2, ST_DirectionType normDirType, ST_Direction* inwardDir1,
	ST_Direction* inwardDir2, float* minPen, float* maxPen, ST_PlaneCollider* const pPlaneCollider, ST_BoxContact* const pContact)
{

	ST_Direction nDir = sphereTraceColliderImposedBoxEdgeDirectionToDirection(&pBoxCollider->localRight, &pBoxCollider->localUp, &pBoxCollider->localForward, normDirType);
	float nDist = 2.0f*sphereTraceColliderBoxGetEdgeHalfLength(pBoxCollider, normDirType);
	ST_Octant v1 = vo1;
	ST_Octant v2 = sphereTraceOctantGetNextFromDirection(v1, normDirType);
	ST_Octant v3 = vo2;
	ST_Octant v4 = sphereTraceOctantGetNextFromDirection(v3, normDirType);
	ST_Edge edge = sphereTraceEdgeConstruct1(pBoxCollider->transformedVertices[v1], pBoxCollider->transformedVertices[v2], nDist, nDir);
	ST_ContactPoint cp;
	{
		ST_Direction temp = sphereTraceDirectionNegative(*inwardDir2);
		ST_PlaneEdgeDirection pe0 = sphereTraceColliderPlaneGetClosestTransformedEdgeToPoint(pPlaneCollider, edge.point1);
		ST_Direction dirNormal = sphereTraceColliderPlaneEdgeDirectionToDirection(pPlaneCollider, pe0);
		if (sphereTraceColliderEdgeEdgeCollisionTest(&edge, &pPlaneCollider->transformedEdges[pe0], &cp) && sphereTraceVector3Dot(dirNormal.v, cp.normal.v)>=0.0f
			&& sphereTraceVector3Dot(inwardDir1->v, dirNormal.v) >= 0.0f && sphereTraceVector3Dot(temp.v, dirNormal.v) >= 0.0f)
		{

			pContact->contactPoints[pContact->numContacts++] = cp;
			if (cp.penetrationDistance < *minPen)
				*minPen = cp.penetrationDistance;
			if (cp.penetrationDistance > *maxPen)
				*maxPen = cp.penetrationDistance;
		}
		ST_PlaneEdgeDirection pe1 = sphereTraceColliderPlaneGetClosestTransformedEdgeToPoint(pPlaneCollider, edge.point2);
		if (pe0 != pe1)
		{
			ST_Direction dirNormal = sphereTraceColliderPlaneEdgeDirectionToDirection(pPlaneCollider, pe1);
			if (sphereTraceColliderEdgeEdgeCollisionTest(&edge, &pPlaneCollider->transformedEdges[pe1], &cp) && sphereTraceVector3Dot(dirNormal.v, cp.normal.v) >= 0.0f
				&& sphereTraceVector3Dot(inwardDir1->v, dirNormal.v) >= 0.0f && sphereTraceVector3Dot(inwardDir2->v, dirNormal.v) >= 0.0f)
			{
				pContact->contactPoints[pContact->numContacts++] = cp;
				if (cp.penetrationDistance < *minPen)
					*minPen = cp.penetrationDistance;
				if (cp.penetrationDistance > *maxPen)
					*maxPen = cp.penetrationDistance;
			}
		}
	}
	//second edge
	{
		//*inwardDir2 = sphereTraceDirectionNegative(*inwardDir2);
		edge = sphereTraceEdgeConstruct1(pBoxCollider->transformedVertices[v3], pBoxCollider->transformedVertices[v4], nDist, nDir);
		ST_PlaneEdgeDirection pe0 = sphereTraceColliderPlaneGetClosestTransformedEdgeToPoint(pPlaneCollider, edge.point1);
		ST_Direction dirNormal = sphereTraceColliderPlaneEdgeDirectionToDirection(pPlaneCollider, pe0);
		if (sphereTraceColliderEdgeEdgeCollisionTest(&edge, &pPlaneCollider->transformedEdges[pe0], &cp) && sphereTraceVector3Dot(dirNormal.v, cp.normal.v) >= 0.0f
			&& sphereTraceVector3Dot(inwardDir1->v, dirNormal.v) >= 0.0f && sphereTraceVector3Dot(inwardDir2->v, dirNormal.v) >= 0.0f)
		{
			pContact->contactPoints[pContact->numContacts++] = cp;
			if (cp.penetrationDistance < *minPen)
				*minPen = cp.penetrationDistance;
			if (cp.penetrationDistance > *maxPen)
				*maxPen = cp.penetrationDistance;
		}
		ST_PlaneEdgeDirection pe1 = sphereTraceColliderPlaneGetClosestTransformedEdgeToPoint(pPlaneCollider, edge.point2);
		if (pe0 != pe1)
		{
			ST_Direction dirNormal = sphereTraceColliderPlaneEdgeDirectionToDirection(pPlaneCollider, pe1);
			if (sphereTraceColliderEdgeEdgeCollisionTest(&edge, &pPlaneCollider->transformedEdges[pe1], &cp) && sphereTraceVector3Dot(dirNormal.v, cp.normal.v) >= 0.0f
				&& sphereTraceVector3Dot(inwardDir1->v, dirNormal.v) >= 0.0f && sphereTraceVector3Dot(inwardDir2->v, dirNormal.v) >= 0.0f)
			{
				pContact->contactPoints[pContact->numContacts++] = cp;
				if (cp.penetrationDistance < *minPen)
					*minPen = cp.penetrationDistance;
				if (cp.penetrationDistance > *maxPen)
					*maxPen = cp.penetrationDistance;
			}
		}
	}
}

b32 sphereTraceColliderBoxPointCollisionTest(const ST_Vector3 point, ST_BoxCollider* const pBoxCollider, ST_ContactPoint* const pContactPoint)
{
	ST_Vector3 dp = sphereTraceVector3Subtract(pBoxCollider->rigidBody.position, point);
	float dpx = sphereTraceVector3Dot(dp, pBoxCollider->localRight.v);
	float dpy = sphereTraceVector3Dot(dp, pBoxCollider->localUp.v);
	float dpz = sphereTraceVector3Dot(dp, pBoxCollider->localForward.v);
	float penx = pBoxCollider->halfExtents.x - sphereTraceAbs(dpx);
	float peny = pBoxCollider->halfExtents.y - sphereTraceAbs(dpy);
	float penz = pBoxCollider->halfExtents.z - sphereTraceAbs(dpz);
	if (penx >= 0.0f && peny >= 0.0f && penz >= 0.0f)
	{
		pContactPoint->collisionType = ST_COLLISION_POINT;
		pContactPoint->point = point;
		if (penx < peny)
		{
			if (penx < penz)
			{
				pContactPoint->penetrationDistance = penx;
				if (dpx >= 0.0f)
				{
					pContactPoint->normal = pBoxCollider->localRight;
				}
				else
				{
					pContactPoint->normal = sphereTraceDirectionNegative(pBoxCollider->localRight);
				}
			}
			else
			{
				pContactPoint->penetrationDistance = penz;
				if (dpz >= 0.0f)
				{
					pContactPoint->normal = pBoxCollider->localForward;
				}
				else
				{
					pContactPoint->normal = sphereTraceDirectionNegative(pBoxCollider->localForward);
				}
			}
		}
		else
		{
			if (peny < penz)
			{
				pContactPoint->penetrationDistance = peny;
				if (dpy >= 0.0f)
				{
					pContactPoint->normal = pBoxCollider->localUp;
				}
				else
				{
					pContactPoint->normal = sphereTraceDirectionNegative(pBoxCollider->localUp);
				}
			}
			else
			{
				pContactPoint->penetrationDistance = penz;
				if (dpz >= 0.0f)
				{
					pContactPoint->normal = pBoxCollider->localForward;
				}
				else
				{
					pContactPoint->normal = sphereTraceDirectionNegative(pBoxCollider->localForward);
				}
			}
		}
		return ST_TRUE;
	}
	return ST_FALSE;
}

b32 sphereTraceColliderBoxRayTrace(ST_Vector3 start, ST_Direction dir, ST_BoxCollider* const pBoxCollider, ST_RayTraceData* const prtd)
{
	ST_Vector3 dp = sphereTraceVector3Subtract(pBoxCollider->rigidBody.position, start);
	float dpx = sphereTraceVector3Dot(dp, pBoxCollider->localRight.v);
	float dpy = sphereTraceVector3Dot(dp, pBoxCollider->localUp.v);
	float dpz = sphereTraceVector3Dot(dp, pBoxCollider->localForward.v);
	float penx = pBoxCollider->halfExtents.x - sphereTraceAbs(dpx);
	float peny = pBoxCollider->halfExtents.y - sphereTraceAbs(dpy);
	float penz = pBoxCollider->halfExtents.z - sphereTraceAbs(dpz);
	if (penx >= 0.0f && peny >= 0.0f && penz >= 0.0f)
	{
		prtd->contact.collisionType = ST_COLLISION_POINT;
		prtd->contact.point = start;
		if (penx < peny)
		{
			if (penx < penz)
			{
				prtd->contact.penetrationDistance = penx;
				if (dpx >= 0.0f)
				{
					prtd->contact.normal = pBoxCollider->localRight;
				}
				else
				{
					prtd->contact.normal = sphereTraceDirectionNegative(pBoxCollider->localRight);
				}
			}
			else
			{
				prtd->contact.penetrationDistance = penz;
				if (dpz >= 0.0f)
				{
					prtd->contact.normal = pBoxCollider->localForward;
				}
				else
				{
					prtd->contact.normal = sphereTraceDirectionNegative(pBoxCollider->localForward);
				}
			}
		}
		else
		{
			if (peny < penz)
			{
				prtd->contact.penetrationDistance = peny;
				if (dpy >= 0.0f)
				{
					prtd->contact.normal = pBoxCollider->localUp;
				}
				else
				{
					prtd->contact.normal = sphereTraceDirectionNegative(pBoxCollider->localUp);
				}
			}
			else
			{
				prtd->contact.penetrationDistance = penz;
				if (dpz >= 0.0f)
				{
					prtd->contact.normal = pBoxCollider->localForward;
				}
				else
				{
					prtd->contact.normal = sphereTraceDirectionNegative(pBoxCollider->localForward);
				}
			}
		}
		prtd->distance = 0.0f;
		prtd->startPoint = start;
		return ST_TRUE;
	}
	ST_Vector3 pointOnPlane;
	ST_Vector3 df;
	dpx = sphereTraceVector3Dot(dir.v, pBoxCollider->localRight.v);
	dpy = sphereTraceVector3Dot(dir.v, pBoxCollider->localUp.v);
	dpz = sphereTraceVector3Dot(dir.v, pBoxCollider->localForward.v);
	if (dpx <= 0.0f)
	{
		pointOnPlane = sphereTraceVector3AddAndScale(pBoxCollider->rigidBody.position,
			pBoxCollider->localRight.v, pBoxCollider->halfExtents.x);
		sphereTraceColliderInfinitePlaneRayTrace(start, dir, pBoxCollider->localRight, pointOnPlane, prtd);
		if (prtd->distance >= 0.0f)
		{
			df = sphereTraceVector3Subtract(prtd->contact.point, pointOnPlane);
			if (sphereTraceAbs(sphereTraceVector3Dot(df, pBoxCollider->localUp.v)) <= pBoxCollider->halfExtents.y)
			{
				if (sphereTraceAbs(sphereTraceVector3Dot(df, pBoxCollider->localForward.v)) <= pBoxCollider->halfExtents.z)
				{
					prtd->directionType = ST_DIRECTION_RIGHT;
					return ST_TRUE;
				}
			}
		}
	}
	else
	{
		pointOnPlane = sphereTraceVector3AddAndScale(pBoxCollider->rigidBody.position,
			pBoxCollider->localRight.v, -pBoxCollider->halfExtents.x);
		sphereTraceColliderInfinitePlaneRayTrace(start, dir, pBoxCollider->localRight, pointOnPlane, prtd);
		if (prtd->distance >= 0.0f)
		{
			df = sphereTraceVector3Subtract(prtd->contact.point, pointOnPlane);
			if (sphereTraceAbs(sphereTraceVector3Dot(df, pBoxCollider->localUp.v)) <= pBoxCollider->halfExtents.y)
			{
				if (sphereTraceAbs(sphereTraceVector3Dot(df, pBoxCollider->localForward.v)) <= pBoxCollider->halfExtents.z)
				{
					prtd->directionType = ST_DIRECTION_LEFT;
					return ST_TRUE;
				}
			}
		}
	}

	if (dpy <= 0.0f)
	{
		pointOnPlane = sphereTraceVector3AddAndScale(pBoxCollider->rigidBody.position,
			pBoxCollider->localUp.v, pBoxCollider->halfExtents.y);
		sphereTraceColliderInfinitePlaneRayTrace(start, dir, pBoxCollider->localUp, pointOnPlane, prtd);
		if (prtd->distance >= 0.0f)
		{
			df = sphereTraceVector3Subtract(prtd->contact.point, pointOnPlane);
			if (sphereTraceAbs(sphereTraceVector3Dot(df, pBoxCollider->localRight.v)) <= pBoxCollider->halfExtents.x)
			{
				if (sphereTraceAbs(sphereTraceVector3Dot(df, pBoxCollider->localForward.v)) <= pBoxCollider->halfExtents.z)
				{
					prtd->directionType = ST_DIRECTION_UP;
					return ST_TRUE;
				}
			}
		}
	}
	else
	{
		pointOnPlane = sphereTraceVector3AddAndScale(pBoxCollider->rigidBody.position,
			pBoxCollider->localUp.v, -pBoxCollider->halfExtents.y);
		sphereTraceColliderInfinitePlaneRayTrace(start, dir, pBoxCollider->localUp, pointOnPlane, prtd);
		if (prtd->distance >= 0.0f)
		{
			df = sphereTraceVector3Subtract(prtd->contact.point, pointOnPlane);
			if (sphereTraceAbs(sphereTraceVector3Dot(df, pBoxCollider->localRight.v)) <= pBoxCollider->halfExtents.x)
			{
				if (sphereTraceAbs(sphereTraceVector3Dot(df, pBoxCollider->localForward.v)) <= pBoxCollider->halfExtents.z)
				{
					prtd->directionType = ST_DIRECTION_DOWN;
					return ST_TRUE;
				}
			}
		}
	}

	if (dpz <= 0.0f)
	{
		pointOnPlane = sphereTraceVector3AddAndScale(pBoxCollider->rigidBody.position,
			pBoxCollider->localForward.v, pBoxCollider->halfExtents.z);
		sphereTraceColliderInfinitePlaneRayTrace(start, dir, pBoxCollider->localForward, pointOnPlane, prtd);
		if (prtd->distance >= 0.0f)
		{
			df = sphereTraceVector3Subtract(prtd->contact.point, pointOnPlane);
			if (sphereTraceAbs(sphereTraceVector3Dot(df, pBoxCollider->localRight.v)) <= pBoxCollider->halfExtents.x)
			{
				if (sphereTraceAbs(sphereTraceVector3Dot(df, pBoxCollider->localUp.v)) <= pBoxCollider->halfExtents.y)
				{
					prtd->directionType = ST_DIRECTION_FORWARD;
					return ST_TRUE;
				}
			}
		}
	}
	else
	{
		pointOnPlane = sphereTraceVector3AddAndScale(pBoxCollider->rigidBody.position,
			pBoxCollider->localForward.v, -pBoxCollider->halfExtents.z);
		sphereTraceColliderInfinitePlaneRayTrace(start, dir, pBoxCollider->localForward, pointOnPlane, prtd);
		if (prtd->distance >= 0.0f)
		{
			df = sphereTraceVector3Subtract(prtd->contact.point, pointOnPlane);
			if (sphereTraceAbs(sphereTraceVector3Dot(df, pBoxCollider->localRight.v)) <= pBoxCollider->halfExtents.x)
			{
				if (sphereTraceAbs(sphereTraceVector3Dot(df, pBoxCollider->localUp.v)) <= pBoxCollider->halfExtents.y)
				{
					prtd->directionType = ST_DIRECTION_BACK;
					return ST_TRUE;
				}
			}
		}
	}
	return ST_FALSE;
}

b32 sphereTraceColliderBoxRayTraceClosestEdge(ST_Vector3 start, ST_Direction dir, ST_Edge* pClosestEdge, ST_Direction* pInward, ST_Direction* pInward1, ST_BoxCollider* const pBoxCollider, ST_RayTraceData* const prtd)
{
	ST_Vector3 dp = sphereTraceVector3Subtract(pBoxCollider->rigidBody.position, start);
	float dpx = sphereTraceVector3Dot(dp, pBoxCollider->localRight.v);
	float dpy = sphereTraceVector3Dot(dp, pBoxCollider->localUp.v);
	float dpz = sphereTraceVector3Dot(dp, pBoxCollider->localForward.v);
	float penx = pBoxCollider->halfExtents.x - sphereTraceAbs(dpx);
	float peny = pBoxCollider->halfExtents.y - sphereTraceAbs(dpy);
	float penz = pBoxCollider->halfExtents.z - sphereTraceAbs(dpz);
	if (penx >= 0.0f && peny >= 0.0f && penz >= 0.0f)
	{
		prtd->contact.collisionType = ST_COLLISION_POINT;
		prtd->contact.point = start;
		if (penx < peny)
		{
			if (penx < penz)
			{
				prtd->contact.penetrationDistance = penx;
				if (dpx >= 0.0f)
				{
					prtd->contact.normal = pBoxCollider->localRight;
				}
				else
				{
					prtd->contact.normal = sphereTraceDirectionNegative(pBoxCollider->localRight);
				}
			}
			else
			{
				prtd->contact.penetrationDistance = penz;
				if (dpz >= 0.0f)
				{
					prtd->contact.normal = pBoxCollider->localForward;
				}
				else
				{
					prtd->contact.normal = sphereTraceDirectionNegative(pBoxCollider->localForward);
				}
			}
		}
		else
		{
			if (peny < penz)
			{
				prtd->contact.penetrationDistance = peny;
				if (dpy >= 0.0f)
				{
					prtd->contact.normal = pBoxCollider->localUp;
				}
				else
				{
					prtd->contact.normal = sphereTraceDirectionNegative(pBoxCollider->localUp);
				}
			}
			else
			{
				prtd->contact.penetrationDistance = penz;
				if (dpz >= 0.0f)
				{
					prtd->contact.normal = pBoxCollider->localForward;
				}
				else
				{
					prtd->contact.normal = sphereTraceDirectionNegative(pBoxCollider->localForward);
				}
			}
		}
		prtd->distance = 0.0f;
		prtd->startPoint = start;
		return ST_TRUE;
	}
	ST_Vector3 pointOnPlane;
	ST_Vector3 df;
	dpx = sphereTraceVector3Dot(dir.v, pBoxCollider->localRight.v);
	dpy = sphereTraceVector3Dot(dir.v, pBoxCollider->localUp.v);
	dpz = sphereTraceVector3Dot(dir.v, pBoxCollider->localForward.v);
	prtd->contact.collisionType = ST_COLLISION_EDGE;
	if (dpx <= 0.0f)
	{
		pointOnPlane = sphereTraceVector3AddAndScale(pBoxCollider->rigidBody.position,
			pBoxCollider->localRight.v, pBoxCollider->halfExtents.x);
		sphereTraceColliderInfinitePlaneRayTrace(start, dir, pBoxCollider->localRight, pointOnPlane, prtd);
		df = sphereTraceVector3Subtract(prtd->contact.point, pointOnPlane);
		float dfu = sphereTraceVector3Dot(df, pBoxCollider->localUp.v);
		float dff = sphereTraceVector3Dot(df, pBoxCollider->localForward.v);
		if (sphereTraceAbs(dfu) <= pBoxCollider->halfExtents.y)
		{
			if (sphereTraceAbs(dff) <= pBoxCollider->halfExtents.z)
			{
				prtd->directionType = ST_DIRECTION_RIGHT;
				dfu /= pBoxCollider->halfExtents.y;
				dff /= pBoxCollider->halfExtents.z;
				*pInward1 = sphereTraceDirectionNegative(pBoxCollider->localRight);
				if (sphereTraceAbs(dfu) >= sphereTraceAbs(dff))
				{
					if (dfu >= 0.0f)
					{
						*pClosestEdge = sphereTraceEdgeConstruct1(pBoxCollider->transformedVertices[ST_RIGHT_UP_BACK], pBoxCollider->transformedVertices[ST_RIGHT_UP_FORWARD],
							sphereTraceColliderBoxGetEdgeHalfLength(pBoxCollider, ST_DIRECTION_FORWARD) * 2.0f, pBoxCollider->localForward);
						*pInward = sphereTraceDirectionNegative(pBoxCollider->localUp);
					}
					else
					{
						*pClosestEdge = sphereTraceEdgeConstruct1(pBoxCollider->transformedVertices[ST_RIGHT_DOWN_BACK], pBoxCollider->transformedVertices[ST_RIGHT_DOWN_FORWARD],
							sphereTraceColliderBoxGetEdgeHalfLength(pBoxCollider, ST_DIRECTION_FORWARD) * 2.0f, pBoxCollider->localForward);
						*pInward = pBoxCollider->localUp;
					}
				}
				else
				{
					if (dff >= 0.0f)
					{
						*pClosestEdge = sphereTraceEdgeConstruct1(pBoxCollider->transformedVertices[ST_RIGHT_DOWN_FORWARD], pBoxCollider->transformedVertices[ST_RIGHT_UP_FORWARD],
							sphereTraceColliderBoxGetEdgeHalfLength(pBoxCollider, ST_DIRECTION_UP) * 2.0f, pBoxCollider->localUp);
						*pInward = sphereTraceDirectionNegative(pBoxCollider->localForward);
					}
					else
					{
						*pClosestEdge = sphereTraceEdgeConstruct1(pBoxCollider->transformedVertices[ST_RIGHT_DOWN_BACK], pBoxCollider->transformedVertices[ST_RIGHT_UP_BACK],
							sphereTraceColliderBoxGetEdgeHalfLength(pBoxCollider, ST_DIRECTION_UP) * 2.0f, pBoxCollider->localUp);
						*pInward = pBoxCollider->localForward;
					}
				}
				return ST_TRUE;
			}
		}
	}
	else
	{
		pointOnPlane = sphereTraceVector3AddAndScale(pBoxCollider->rigidBody.position,
			pBoxCollider->localRight.v, -pBoxCollider->halfExtents.x);
		sphereTraceColliderInfinitePlaneRayTrace(start, dir, pBoxCollider->localRight, pointOnPlane, prtd);
		df = sphereTraceVector3Subtract(prtd->contact.point, pointOnPlane);
		float dfu = sphereTraceVector3Dot(df, pBoxCollider->localUp.v);
		float dff = sphereTraceVector3Dot(df, pBoxCollider->localForward.v);
		if (sphereTraceAbs(dfu) <= pBoxCollider->halfExtents.y)
		{
			if (sphereTraceAbs(dff) <= pBoxCollider->halfExtents.z)
			{
				prtd->directionType = ST_DIRECTION_LEFT;
				dfu /= pBoxCollider->halfExtents.y;
				dff /= pBoxCollider->halfExtents.z;
				*pInward1 = pBoxCollider->localRight;
				if (sphereTraceAbs(dfu) >= sphereTraceAbs(dff))
				{
					if (dfu >= 0.0f)
					{
						*pClosestEdge = sphereTraceEdgeConstruct1(pBoxCollider->transformedVertices[ST_LEFT_UP_BACK], pBoxCollider->transformedVertices[ST_LEFT_UP_FORWARD],
							sphereTraceColliderBoxGetEdgeHalfLength(pBoxCollider, ST_DIRECTION_FORWARD) * 2.0f, pBoxCollider->localForward);
						*pInward = sphereTraceDirectionNegative(pBoxCollider->localUp);
					}
					else
					{
						*pClosestEdge = sphereTraceEdgeConstruct1(pBoxCollider->transformedVertices[ST_LEFT_DOWN_BACK], pBoxCollider->transformedVertices[ST_LEFT_DOWN_FORWARD],
							sphereTraceColliderBoxGetEdgeHalfLength(pBoxCollider, ST_DIRECTION_FORWARD) * 2.0f, pBoxCollider->localForward);
						*pInward = pBoxCollider->localUp;
					}
				}
				else
				{
					if (dff >= 0.0f)
					{
						*pClosestEdge = sphereTraceEdgeConstruct1(pBoxCollider->transformedVertices[ST_LEFT_DOWN_FORWARD], pBoxCollider->transformedVertices[ST_LEFT_UP_FORWARD],
							sphereTraceColliderBoxGetEdgeHalfLength(pBoxCollider, ST_DIRECTION_UP) * 2.0f, pBoxCollider->localUp);
						*pInward = sphereTraceDirectionNegative(pBoxCollider->localForward);
					}
					else
					{
						*pClosestEdge = sphereTraceEdgeConstruct1(pBoxCollider->transformedVertices[ST_LEFT_DOWN_BACK], pBoxCollider->transformedVertices[ST_LEFT_UP_BACK],
							sphereTraceColliderBoxGetEdgeHalfLength(pBoxCollider, ST_DIRECTION_UP) * 2.0f, pBoxCollider->localUp);
						*pInward =pBoxCollider->localForward;
					}
				}
				return ST_TRUE;
			}
		}
	}

	if (dpy <= 0.0f)
	{
		pointOnPlane = sphereTraceVector3AddAndScale(pBoxCollider->rigidBody.position,
			pBoxCollider->localUp.v, pBoxCollider->halfExtents.y);
		sphereTraceColliderInfinitePlaneRayTrace(start, dir, pBoxCollider->localUp, pointOnPlane, prtd);
		df = sphereTraceVector3Subtract(prtd->contact.point, pointOnPlane);
		float dfr = sphereTraceVector3Dot(df, pBoxCollider->localRight.v);
		float dff = sphereTraceVector3Dot(df, pBoxCollider->localForward.v);
		if (sphereTraceAbs(dfr) <= pBoxCollider->halfExtents.x)
		{
			if (sphereTraceAbs(dff) <= pBoxCollider->halfExtents.z)
			{
				prtd->directionType = ST_DIRECTION_UP;
				dfr /= pBoxCollider->halfExtents.x;
				dff /= pBoxCollider->halfExtents.z;
				*pInward1 = sphereTraceDirectionNegative(pBoxCollider->localUp);
				if (sphereTraceAbs(dfr) >= sphereTraceAbs(dff))
				{
					if (dfr >= 0.0f)
					{
						*pClosestEdge = sphereTraceEdgeConstruct1(pBoxCollider->transformedVertices[ST_RIGHT_UP_BACK], pBoxCollider->transformedVertices[ST_RIGHT_UP_FORWARD],
							sphereTraceColliderBoxGetEdgeHalfLength(pBoxCollider, ST_DIRECTION_FORWARD) * 2.0f, pBoxCollider->localForward);
						*pInward = sphereTraceDirectionNegative(pBoxCollider->localRight);
					}
					else
					{
						*pClosestEdge = sphereTraceEdgeConstruct1(pBoxCollider->transformedVertices[ST_LEFT_UP_BACK], pBoxCollider->transformedVertices[ST_LEFT_UP_FORWARD],
							sphereTraceColliderBoxGetEdgeHalfLength(pBoxCollider, ST_DIRECTION_FORWARD) * 2.0f, pBoxCollider->localForward);
						*pInward = pBoxCollider->localRight;
					}
				}
				else
				{
					if (dff >= 0.0f)
					{
						*pClosestEdge = sphereTraceEdgeConstruct1(pBoxCollider->transformedVertices[ST_LEFT_UP_FORWARD], pBoxCollider->transformedVertices[ST_RIGHT_UP_FORWARD],
							sphereTraceColliderBoxGetEdgeHalfLength(pBoxCollider, ST_DIRECTION_RIGHT) * 2.0f, pBoxCollider->localRight);
						*pInward = sphereTraceDirectionNegative(pBoxCollider->localForward);
					}
					else
					{
						*pClosestEdge = sphereTraceEdgeConstruct1(pBoxCollider->transformedVertices[ST_LEFT_UP_BACK], pBoxCollider->transformedVertices[ST_RIGHT_UP_BACK],
							sphereTraceColliderBoxGetEdgeHalfLength(pBoxCollider, ST_DIRECTION_RIGHT) * 2.0f, pBoxCollider->localRight);
						*pInward = pBoxCollider->localForward;
					}
				}
				return ST_TRUE;
			}
		}
	}
	else
	{
		pointOnPlane = sphereTraceVector3AddAndScale(pBoxCollider->rigidBody.position,
			pBoxCollider->localUp.v, -pBoxCollider->halfExtents.y);
		sphereTraceColliderInfinitePlaneRayTrace(start, dir, pBoxCollider->localUp, pointOnPlane, prtd);
		df = sphereTraceVector3Subtract(prtd->contact.point, pointOnPlane);
		float dfr = sphereTraceVector3Dot(df, pBoxCollider->localRight.v);
		float dff = sphereTraceVector3Dot(df, pBoxCollider->localForward.v);
		if (sphereTraceAbs(dfr) <= pBoxCollider->halfExtents.x)
		{
			if (sphereTraceAbs(dff) <= pBoxCollider->halfExtents.z)
			{
				prtd->directionType = ST_DIRECTION_DOWN;
				dfr /= pBoxCollider->halfExtents.x;
				dff /= pBoxCollider->halfExtents.z;
				*pInward1 = pBoxCollider->localUp;
				if (sphereTraceAbs(dfr) >= sphereTraceAbs(dff))
				{
					if (dfr >= 0.0f)
					{
						*pClosestEdge = sphereTraceEdgeConstruct1(pBoxCollider->transformedVertices[ST_RIGHT_DOWN_BACK], pBoxCollider->transformedVertices[ST_RIGHT_DOWN_FORWARD],
							sphereTraceColliderBoxGetEdgeHalfLength(pBoxCollider, ST_DIRECTION_FORWARD) * 2.0f, pBoxCollider->localForward);
						*pInward = sphereTraceDirectionNegative(pBoxCollider->localRight);
					}
					else
					{
						*pClosestEdge = sphereTraceEdgeConstruct1(pBoxCollider->transformedVertices[ST_LEFT_DOWN_BACK], pBoxCollider->transformedVertices[ST_LEFT_DOWN_FORWARD],
							sphereTraceColliderBoxGetEdgeHalfLength(pBoxCollider, ST_DIRECTION_FORWARD) * 2.0f, pBoxCollider->localForward);
						*pInward = pBoxCollider->localRight;
					}
				}
				else
				{
					if (dff >= 0.0f)
					{
						*pClosestEdge = sphereTraceEdgeConstruct1(pBoxCollider->transformedVertices[ST_LEFT_DOWN_FORWARD], pBoxCollider->transformedVertices[ST_RIGHT_DOWN_FORWARD],
							sphereTraceColliderBoxGetEdgeHalfLength(pBoxCollider, ST_DIRECTION_RIGHT) * 2.0f, pBoxCollider->localRight);
						*pInward = sphereTraceDirectionNegative(pBoxCollider->localForward);
					}
					else
					{
						*pClosestEdge = sphereTraceEdgeConstruct1(pBoxCollider->transformedVertices[ST_LEFT_DOWN_BACK], pBoxCollider->transformedVertices[ST_RIGHT_DOWN_BACK],
							sphereTraceColliderBoxGetEdgeHalfLength(pBoxCollider, ST_DIRECTION_RIGHT) * 2.0f, pBoxCollider->localRight);
						*pInward = pBoxCollider->localForward;
					}
				}
				return ST_TRUE;
			}
		}
	}

	if (dpz <= 0.0f)
	{
		pointOnPlane = sphereTraceVector3AddAndScale(pBoxCollider->rigidBody.position,
			pBoxCollider->localForward.v, pBoxCollider->halfExtents.z);
		sphereTraceColliderInfinitePlaneRayTrace(start, dir, pBoxCollider->localForward, pointOnPlane, prtd);
		df = sphereTraceVector3Subtract(prtd->contact.point, pointOnPlane);
		float dfr = sphereTraceVector3Dot(df, pBoxCollider->localRight.v);
		float dfu = sphereTraceVector3Dot(df, pBoxCollider->localUp.v);
		if (sphereTraceAbs(dfr) <= pBoxCollider->halfExtents.x)
		{
			if (sphereTraceAbs(dfu) <= pBoxCollider->halfExtents.y)
			{
				prtd->directionType = ST_DIRECTION_FORWARD;
				dfr /= pBoxCollider->halfExtents.x;
				dfu /= pBoxCollider->halfExtents.y;
				*pInward1 = sphereTraceDirectionNegative(pBoxCollider->localForward);
				if (sphereTraceAbs(dfr) >= sphereTraceAbs(dfu))
				{
					if (dfr >= 0.0f)
					{
						*pClosestEdge = sphereTraceEdgeConstruct1(pBoxCollider->transformedVertices[ST_RIGHT_DOWN_FORWARD], pBoxCollider->transformedVertices[ST_RIGHT_UP_FORWARD],
							sphereTraceColliderBoxGetEdgeHalfLength(pBoxCollider, ST_DIRECTION_UP) * 2.0f, pBoxCollider->localUp);
						*pInward = sphereTraceDirectionNegative(pBoxCollider->localRight);
					}
					else
					{
						*pClosestEdge = sphereTraceEdgeConstruct1(pBoxCollider->transformedVertices[ST_LEFT_DOWN_FORWARD], pBoxCollider->transformedVertices[ST_LEFT_UP_FORWARD],
							sphereTraceColliderBoxGetEdgeHalfLength(pBoxCollider, ST_DIRECTION_UP) * 2.0f, pBoxCollider->localUp);
						*pInward = pBoxCollider->localRight;
					}
				}
				else
				{
					if (dfu >= 0.0f)
					{
						*pClosestEdge = sphereTraceEdgeConstruct1(pBoxCollider->transformedVertices[ST_LEFT_UP_FORWARD], pBoxCollider->transformedVertices[ST_RIGHT_UP_FORWARD],
							sphereTraceColliderBoxGetEdgeHalfLength(pBoxCollider, ST_DIRECTION_RIGHT) * 2.0f, pBoxCollider->localRight);
						*pInward = sphereTraceDirectionNegative(pBoxCollider->localUp);
					}
					else
					{
						*pClosestEdge = sphereTraceEdgeConstruct1(pBoxCollider->transformedVertices[ST_LEFT_DOWN_FORWARD], pBoxCollider->transformedVertices[ST_RIGHT_DOWN_FORWARD],
							sphereTraceColliderBoxGetEdgeHalfLength(pBoxCollider, ST_DIRECTION_RIGHT) * 2.0f, pBoxCollider->localRight);
						*pInward = pBoxCollider->localUp;
					}
				}
				return ST_TRUE;
			}
		}
	}
	else
	{
		pointOnPlane = sphereTraceVector3AddAndScale(pBoxCollider->rigidBody.position,
			pBoxCollider->localForward.v, -pBoxCollider->halfExtents.z);
		sphereTraceColliderInfinitePlaneRayTrace(start, dir, pBoxCollider->localForward, pointOnPlane, prtd);
		df = sphereTraceVector3Subtract(prtd->contact.point, pointOnPlane);
		float dfr = sphereTraceVector3Dot(df, pBoxCollider->localRight.v);
		float dfu = sphereTraceVector3Dot(df, pBoxCollider->localUp.v);
		if (sphereTraceAbs(dfr) <= pBoxCollider->halfExtents.x)
		{
			if (sphereTraceAbs(dfu) <= pBoxCollider->halfExtents.y)
			{
				prtd->directionType = ST_DIRECTION_BACK;
				dfr /= pBoxCollider->halfExtents.x;
				dfu /= pBoxCollider->halfExtents.y;
				*pInward1 = pBoxCollider->localForward;
				if (sphereTraceAbs(dfr) >= sphereTraceAbs(dfu))
				{
					if (dfr >= 0.0f)
					{
						*pClosestEdge = sphereTraceEdgeConstruct1(pBoxCollider->transformedVertices[ST_RIGHT_DOWN_BACK], pBoxCollider->transformedVertices[ST_RIGHT_UP_BACK],
							sphereTraceColliderBoxGetEdgeHalfLength(pBoxCollider, ST_DIRECTION_UP) * 2.0f, pBoxCollider->localUp);
						*pInward = sphereTraceDirectionNegative(pBoxCollider->localRight);
					}
					else
					{
						*pClosestEdge = sphereTraceEdgeConstruct1(pBoxCollider->transformedVertices[ST_LEFT_DOWN_BACK], pBoxCollider->transformedVertices[ST_LEFT_UP_BACK],
							sphereTraceColliderBoxGetEdgeHalfLength(pBoxCollider, ST_DIRECTION_UP) * 2.0f, pBoxCollider->localUp);
						*pInward = pBoxCollider->localRight;
					}
				}
				else
				{
					if (dfu >= 0.0f)
					{
						*pClosestEdge = sphereTraceEdgeConstruct1(pBoxCollider->transformedVertices[ST_LEFT_UP_BACK], pBoxCollider->transformedVertices[ST_RIGHT_UP_BACK],
							sphereTraceColliderBoxGetEdgeHalfLength(pBoxCollider, ST_DIRECTION_RIGHT) * 2.0f, pBoxCollider->localRight);
						*pInward = sphereTraceDirectionNegative(pBoxCollider->localUp);
					}
					else
					{
						*pClosestEdge = sphereTraceEdgeConstruct1(pBoxCollider->transformedVertices[ST_LEFT_DOWN_BACK], pBoxCollider->transformedVertices[ST_RIGHT_DOWN_BACK],
							sphereTraceColliderBoxGetEdgeHalfLength(pBoxCollider, ST_DIRECTION_RIGHT) * 2.0f, pBoxCollider->localRight);
						*pInward = pBoxCollider->localUp;
					}
				}
				
				return ST_TRUE;
			}
		}
	}
	return ST_FALSE;
}

b32 sphereTraceColliderBoxEdgeCollisionTest(ST_Edge* const pEdge, ST_BoxCollider* const pBoxCollider, ST_BoxContact* const pContact)
{
	ST_RayTraceData rtd1, rtd2;
	ST_ContactPoint cp1, cp2, cpFail;
	ST_Edge ce1, ce2;
	ST_Direction inwarda1, inwarda2, inwardb1, inwardb2;
	pContact->numContacts = 0;
	int c = 0;
	pContact->maxPenetrationIndex = 0;
	float maxPen = 0.0f;
	if (sphereTraceColliderBoxRayTraceClosestEdge(pEdge->point1, pEdge->dir, &ce1, &inwarda1, &inwarda2, pBoxCollider, &rtd1) &&
		sphereTraceColliderBoxRayTraceClosestEdge(pEdge->point2, sphereTraceDirectionNegative(pEdge->dir), &ce2, &inwardb1, &inwardb2, pBoxCollider, &rtd2))
	{
		if(rtd1.contact.collisionType == ST_COLLISION_POINT)
		{
			pContact->contactPoints[pContact->numContacts++] = rtd1.contact;
			pContact->maxPenetrationIndex = 0;
			maxPen = rtd1.contact.penetrationDistance;
		}
		else if (sphereTraceColliderEdgeEdgeCollisionTest(&ce1, pEdge, &cp1))
		{
			if (sphereTraceVector3Dot(cp1.normal.v, inwarda1.v) >= 0.0f && sphereTraceVector3Dot(cp1.normal.v, inwarda2.v) >= 0.0f)
			{
				pContact->contactPoints[pContact->numContacts++] = cp1;
				pContact->maxPenetrationIndex = 0;
				maxPen = cp1.penetrationDistance;
			}
			else
				cpFail = cp1;
			c++;
		}

		if (rtd2.contact.collisionType == ST_COLLISION_POINT)
		{
			pContact->contactPoints[pContact->numContacts++] = rtd2.contact;
			if (rtd2.contact.penetrationDistance > maxPen)
			{
				pContact->maxPenetrationIndex = pContact->numContacts - 1;
			}
		}
		else if (sphereTraceColliderEdgeEdgeCollisionTest( &ce2, pEdge, &cp2))
		{
			if (sphereTraceVector3Dot(cp2.normal.v, inwardb1.v) >= 0.0f && sphereTraceVector3Dot(cp2.normal.v, inwardb2.v) >= 0.0f)
			{
				pContact->contactPoints[pContact->numContacts++] = cp2;
				if (cp2.penetrationDistance > maxPen)
				{
					pContact->maxPenetrationIndex = pContact->numContacts-1;
				}
			}
			else
				cpFail = cp2;
			c++;
		}
		if (c == 2)
		{
			if (pContact->numContacts == 1)
			{
				if (sphereTraceVector3Dot(cpFail.normal.v, pContact->contactPoints[0].normal.v) > 0.0f)
					pContact->contactPoints[pContact->numContacts++] = cpFail;
			}
			else if (pContact->numContacts == 0)
			{
				pContact->contactPoints[pContact->numContacts++] = cpFail;
			}
		}
		else if (c == 1)
		{
			if (pContact->numContacts == 0)
			{
				pContact->contactPoints[pContact->numContacts++] = cpFail;
			}
		}
		return pContact->numContacts > 0;
	}
	return ST_FALSE;
}

ST_BoxFace sphereTraceColliderBoxFaceDirectionToBoxFace(ST_DirectionType dir)
{
	switch (dir)
	{
		case ST_DIRECTION_RIGHT:
			return gFaceRight;
			break;
		case ST_DIRECTION_LEFT:
			return gFaceLeft;
			break;
		case ST_DIRECTION_UP:
			return gFaceUp;
			break;
		case ST_DIRECTION_DOWN:
			return gFaceDown;
			break;
		case ST_DIRECTION_FORWARD:
			return gFaceForward;
			break;
		case ST_DIRECTION_BACK:
			return gFaceBack;
			break;
	}
}

ST_Edge sphereTraceColliderBoxFaceGetClosestEdgeToPoint(ST_BoxCollider* const pBoxCollider, ST_BoxFace* const pBoxFace, ST_Vector3 point)
{
	ST_DirectionType t1, t2;
	sphereTraceColliderBoxFaceGetTangentialDirections(pBoxFace, &t1, &t2);
	ST_Direction t1Dir = sphereTraceColliderImposedBoxEdgeDirectionToDirection(&pBoxCollider->localRight, &pBoxCollider->localUp, &pBoxCollider->localForward, t1);
	ST_Direction t2Dir = sphereTraceColliderImposedBoxEdgeDirectionToDirection(&pBoxCollider->localRight, &pBoxCollider->localUp, &pBoxCollider->localForward, t2);
	float t1Dist = sphereTraceColliderBoxGetEdgeHalfLength(pBoxCollider, t1);
	float t2Dist = sphereTraceColliderBoxGetEdgeHalfLength(pBoxCollider, t2);
	ST_Vector3 dp = sphereTraceVector3Subtract(point, pBoxCollider->rigidBody.position);
	float dpt1 = sphereTraceVector3Dot(dp, t1Dir.v)/t1Dist;
	float dpt2 = sphereTraceVector3Dot(dp, t2Dir.v)/t2Dist;
	ST_Octant v0 = pBoxFace->vertexIndices[0];
	ST_Octant v1 = sphereTraceOctantGetNextFromDirection(v0, t1);
	ST_Octant v2 = sphereTraceOctantGetNextFromDirection(v1, t2);
	ST_Octant v3 = sphereTraceOctantGetNextFromDirection(v0, t2);

	if (sphereTraceAbs(dpt1) >= sphereTraceAbs(dpt2))
	{
		if (dpt1 >= 0.0f)
		{
			return sphereTraceEdgeConstruct1(pBoxCollider->transformedVertices[v1], pBoxCollider->transformedVertices[v2], 2.0f * t2Dist, t2Dir);
		}
		else
		{
			return sphereTraceEdgeConstruct1(pBoxCollider->transformedVertices[v0], pBoxCollider->transformedVertices[v3], 2.0f * t2Dist, t2Dir);
		}
	}
	else
	{
		if (dpt2 >= 0.0f)
		{
			return sphereTraceEdgeConstruct1(pBoxCollider->transformedVertices[v3], pBoxCollider->transformedVertices[v2], 2.0f * t1Dist, t1Dir);
		}
		else
		{
			return sphereTraceEdgeConstruct1(pBoxCollider->transformedVertices[v0], pBoxCollider->transformedVertices[v1], 2.0f * t1Dist, t1Dir);
		}
	}
}


b32 sphereTraceColliderBoxPlaneEdgeCollisionTest(ST_Edge* const pEdge, ST_Direction dirRestriction, ST_BoxCollider* const pBoxCollider, ST_Direction normal, ST_BoxFace* const pClosestFaceToPlane, ST_BoxContact* const pContact, b32* pIsTangential)
{
	ST_RayTraceData rtd1, rtd2;
	pContact->numContacts = 0;
	if (sphereTraceColliderBoxRayTrace(pEdge->point1, pEdge->dir, pBoxCollider, &rtd1) && sphereTraceColliderBoxRayTrace(pEdge->point2, sphereTraceDirectionNegative(pEdge->dir), pBoxCollider, &rtd2))
	{
		ST_Edge e = sphereTraceColliderBoxFaceGetClosestEdgeToPoint(pBoxCollider, pClosestFaceToPlane, rtd1.contact.point);
		ST_ContactPoint cn1, ct1, cn2, ct2;
		b32 n1h = ST_FALSE, t1h = ST_FALSE, n2h = ST_FALSE, t2h = ST_FALSE;
		float minNormal = FLT_MAX, minTangential = FLT_MAX;
		ST_Direction inward;
		ST_BoxFace inwardFace;
		inward = sphereTraceDirectionNegative(dirRestriction);
		inwardFace = sphereTraceColliderBoxGetFaceClosestToDirection(pBoxCollider, inward);
		if (rtd1.contact.collisionType == ST_COLLISION_POINT)
		{
			cn1 = rtd1.contact;
			minNormal = cn1.penetrationDistance;
			n1h = ST_TRUE;
		}
		else
		{
			if (sphereTraceColliderEdgeEdgeCollisionTest(&e, pEdge, &cn1))
			{
				if (sphereTraceVector3Dot(normal.v, cn1.normal.v) > 0.0f)
				{
					minNormal = cn1.penetrationDistance;
					n1h = ST_TRUE;
				}
			}

			if (inwardFace.dir != sphereTraceDirectionTypeGetReverse(rtd1.directionType))
			{
				e = sphereTraceColliderBoxFaceGetClosestEdgeToPoint(pBoxCollider, &inwardFace, rtd1.contact.point);
				if (sphereTraceColliderEdgeEdgeCollisionTest(&e, pEdge, &ct1))
				{
					if (sphereTraceVector3Dot(dirRestriction.v, ct1.normal.v) > 0.0f)
					{
						minTangential = ct1.penetrationDistance;
						t1h = ST_TRUE;
					}
				}
			}
		}
		if (rtd2.contact.collisionType == ST_COLLISION_POINT)
		{
			cn2 = rtd2.contact;
			if (cn2.penetrationDistance < minNormal)
			{
				minNormal = cn2.penetrationDistance;
			}
			n2h = ST_TRUE;
		}
		else
		{
			e = sphereTraceColliderBoxFaceGetClosestEdgeToPoint(pBoxCollider, pClosestFaceToPlane, rtd2.contact.point);
			if (sphereTraceColliderEdgeEdgeCollisionTest(&e, pEdge, &cn2))
			{
				if (sphereTraceVector3Dot(normal.v, cn2.normal.v) > 0.0f)
				{
					if (cn2.penetrationDistance < minNormal)
					{
						minNormal = cn2.penetrationDistance;
					}
					n2h = ST_TRUE;
				}
			}
			if (inwardFace.dir != sphereTraceDirectionTypeGetReverse(rtd2.directionType))
			{
				e = sphereTraceColliderBoxFaceGetClosestEdgeToPoint(pBoxCollider, &inwardFace, rtd2.contact.point);
				if (sphereTraceColliderEdgeEdgeCollisionTest(&e, pEdge, &ct2))
				{
					if (sphereTraceVector3Dot(dirRestriction.v, ct2.normal.v) > 0.0f)
					{
						if (ct2.penetrationDistance < minTangential)
						{
							minTangential = ct2.penetrationDistance;
						}
						t2h = ST_TRUE;
					}
				}
			}
		}
		if (minTangential < FLT_MAX || minNormal < FLT_MAX)
		{
			if (minTangential < minNormal)
			{
				if (t1h)
					pContact->contactPoints[pContact->numContacts++] = ct1;
				if (t2h)
					pContact->contactPoints[pContact->numContacts++] = ct2;
				*pIsTangential = ST_TRUE;
			}
			else
			{
				if (n1h)
					pContact->contactPoints[pContact->numContacts++] = cn1;
				if (n2h)
					pContact->contactPoints[pContact->numContacts++] = cn2;
				*pIsTangential = ST_FALSE;
			}
		}
		return pContact->numContacts>0;
	}
	return ST_FALSE;
}

b32 sphereTraceColliderBoxFacesEdgeCollisionTest(ST_Edge* const pEdge, ST_Direction dirRestriction, ST_BoxCollider* const pBoxCollider, ST_Direction normal, ST_BoxFace* const pClosestFaceToPlane, ST_BoxContact* const pContact)
{
	ST_RayTraceData rtd1, rtd2;
	pContact->numContacts = 0;
	if (sphereTraceColliderBoxRayTrace(pEdge->point1, pEdge->dir, pBoxCollider, &rtd1) && sphereTraceColliderBoxRayTrace(pEdge->point2, sphereTraceDirectionNegative(pEdge->dir), pBoxCollider, &rtd2))
	{
		ST_Edge e = sphereTraceColliderBoxFaceGetClosestEdgeToPoint(pBoxCollider, pClosestFaceToPlane, rtd1.contact.point);
		ST_ContactPoint cn1, cn2;
		b32 n1h = ST_FALSE, n2h = ST_FALSE;
		float minNormal = FLT_MAX;
		if (rtd1.contact.collisionType == ST_COLLISION_POINT)
		{
			cn1 = rtd1.contact;
			minNormal = cn1.penetrationDistance;
			n1h = ST_TRUE;
		}
		else
		{
			if (sphereTraceColliderEdgeEdgeCollisionTest(&e, pEdge, &cn1))
			{
				if (sphereTraceVector3Dot(normal.v, cn1.normal.v) > 0.0f)
				{
					minNormal = cn1.penetrationDistance;
					n1h = ST_TRUE;
				}
			}
		}
		if (rtd2.contact.collisionType == ST_COLLISION_POINT)
		{
			cn2 = rtd2.contact;
			if (cn2.penetrationDistance < minNormal)
			{
				minNormal = cn2.penetrationDistance;
			}
			n2h = ST_TRUE;
		}
		else
		{
			e = sphereTraceColliderBoxFaceGetClosestEdgeToPoint(pBoxCollider, pClosestFaceToPlane, rtd2.contact.point);
			if (sphereTraceColliderEdgeEdgeCollisionTest(&e, pEdge, &cn2))
			{
				if (sphereTraceVector3Dot(normal.v, cn2.normal.v) > 0.0f)
				{
					if (cn2.penetrationDistance < minNormal)
					{
						minNormal = cn2.penetrationDistance;
					}
					n2h = ST_TRUE;
				}
			}
		}
		if (minNormal < FLT_MAX)
		{
			if (n1h)
				pContact->contactPoints[pContact->numContacts++] = cn1;
			if (n2h)
				pContact->contactPoints[pContact->numContacts++] = cn2;

		}
		return pContact->numContacts > 0;
	}
	return ST_FALSE;
}




void sphereTraceColliderBoxForceSolvePenetration(ST_BoxCollider* const pBoxCollider, ST_BoxContact* const pContact)
{
	if (pContact->numContacts == 1)
	{
		sphereTraceColliderBoxSetPosition(pBoxCollider, sphereTraceVector3AddAndScale(pBoxCollider->rigidBody.position, 
			pContact->contactPoints[0].normal.v, pContact->contactPoints[0].penetrationDistance));
	}
	else if (pContact->numContacts == 2)
	{
		ST_Direction dirSideways = sphereTraceDirectionConstructNormalized(sphereTraceVector3Subtract(pContact->contactPoints[0].point, pContact->contactPoints[1].point));
		if (!sphereTraceVector3NanAny(dirSideways.v) && sphereTraceEpsilonEqual(sphereTraceVector3Dot(pContact->contactPoints[0].normal.v, pContact->contactPoints[1].normal.v), 1.0f, ST_COLLIDER_TOLERANCE))
		{
			ST_BoxFace sf = sphereTraceColliderBoxGetFaceClosestToDirection(pBoxCollider, dirSideways);
			ST_Direction sfDir = sphereTraceColliderBoxFaceGetNormal(pBoxCollider, &sf);
			ST_Vector3 cross = sphereTraceVector3Normalize(sphereTraceVector3Cross(dirSideways.v, sfDir.v));
			float angle = acosf(sphereTraceVector3Dot(dirSideways.v, sfDir.v));
			ST_Quaternion rot = sphereTraceQuaternionFromAngleAxis(cross, - angle);
			sphereTraceColliderBoxSetPosition(pBoxCollider, sphereTraceVector3AddAndScale(pBoxCollider->rigidBody.position,
				pContact->contactPoints[pContact->maxPenetrationIndex].normal.v, pContact->contactPoints[pContact->maxPenetrationIndex].penetrationDistance));
			if(sphereTraceAbs(angle)>ST_COLLIDER_TOLERANCE)
				sphereTraceRigidBodyRotateAroundPoint(&pBoxCollider->rigidBody, pContact->contactPoints[pContact->maxPenetrationIndex].point, rot);
		}
		else
		{
			sphereTraceColliderBoxSetPosition(pBoxCollider, sphereTraceVector3AddAndScale(pBoxCollider->rigidBody.position,
				pContact->contactPoints[pContact->maxPenetrationIndex].normal.v, pContact->contactPoints[pContact->maxPenetrationIndex].penetrationDistance));
		}
	}
	else
	{
		sphereTraceColliderBoxSetPosition(pBoxCollider, sphereTraceVector3AddAndScale(pBoxCollider->rigidBody.position,
			pContact->contactPoints[0].normal.v, pContact->contactPoints[0].penetrationDistance));
	}
}

b32 sphereTraceColliderPlaneBoxCollisionTest_(ST_BoxCollider* const pBoxCollider, ST_PlaneCollider* const pPlaneCollider, ST_BoxContact* const pContact)
{
	ST_Vector3 dp = sphereTraceVector3Subtract(pPlaneCollider->position, pBoxCollider->rigidBody.position);
	ST_Direction normal = pPlaneCollider->normal;
	float dot = sphereTraceVector3Dot(dp, normal.v);
	if (dot > 0.0f)
	{
		normal = sphereTraceDirectionNegative(normal);
	}
	ST_Direction tangentialdpDir = sphereTraceDirectionConstructNormalized(sphereTraceVector3Add(sphereTraceVector3Scale(pPlaneCollider->right.v, sphereTraceVector3Dot(pPlaneCollider->right.v, dp)),
		sphereTraceVector3Scale(pPlaneCollider->forward.v, sphereTraceVector3Dot(pPlaneCollider->forward.v, dp))));

	ST_Direction closestFaceDirection = sphereTraceDirectionNegative(normal);
	ST_BoxFace closestFace = sphereTraceColliderBoxGetFaceClosestToDirection(pBoxCollider, closestFaceDirection);
	ST_RayTraceData rtd;
	b32 hits = ST_FALSE;
	float minPenetration = FLT_MAX;
	float maxPenetration = 0.0f;
	pContact->numContacts = 0;
	pContact->pOtherCollider = pPlaneCollider;
	pContact->pBoxCollider = pBoxCollider;
	pContact->maxPenetrationIndex = 0;
	int numEdgeTests = 0;
	int edgesToCheck[4];
	//check all points
	for (int i = 0; i < 4; i++)
	{
		sphereTraceColliderInfinitePlaneRayTrace(pBoxCollider->transformedVertices[closestFace.vertexIndices[i]], normal, pPlaneCollider->normal, pPlaneCollider->position, &rtd);
		if (sphereTraceColliderPlaneIsProjectedPointContained(rtd.contact.point, pPlaneCollider) && rtd.distance>0.0f)
		{
			pContact->contactPoints[pContact->numContacts] = rtd.contact;
			pContact->contactPoints[pContact->numContacts].penetrationDistance = rtd.distance;
			pContact->contactPoints[pContact->numContacts].normal = normal;
			pContact->numContacts++;

			if (rtd.distance <= minPenetration)
			{
				minPenetration = rtd.distance;
			}
			if (rtd.distance > maxPenetration)
			{
				maxPenetration = rtd.distance;
				pContact->maxPenetrationIndex = pContact->numContacts - 1;
			}
			hits = ST_TRUE;
		}
		else
		{
			int checkInd = sphereTraceColliderPlaneGetClosestTransformedEdgeToPoint(pPlaneCollider, rtd.contact.point);
			b32 notFound = ST_TRUE;
			for (int i = 0; i < numEdgeTests; i++)
			{
				if (edgesToCheck[i] == checkInd)
				{
					notFound = ST_FALSE;
					break;
				}
			}
			if (notFound)
				edgesToCheck[numEdgeTests++] = checkInd;
		}
	}
	ST_BoxContact bc;
	b32 isTangential = ST_FALSE;
	for (int i = 0; i < numEdgeTests; i++)
	{
		if (sphereTraceColliderBoxPlaneEdgeCollisionTest(&pPlaneCollider->transformedEdges[edgesToCheck[i]], sphereTraceColliderPlaneEdgeDirectionToDirection(pPlaneCollider, edgesToCheck[i]),
			pBoxCollider, normal, &closestFace, &bc, &isTangential))
		{
			if (isTangential)
			{
				pContact->numContacts = 0;
				minPenetration = FLT_MAX;
				maxPenetration = 0.0f;
			}
			for (int j = 0; j < bc.numContacts; j++)
			{
				pContact->contactPoints[pContact->numContacts] = bc.contactPoints[j];
				pContact->numContacts++;

				if (bc.contactPoints[j].penetrationDistance <= minPenetration)
				{
					minPenetration = bc.contactPoints[j].penetrationDistance;
				}
				if (bc.contactPoints[j].penetrationDistance > maxPenetration)
				{
					maxPenetration = bc.contactPoints[j].penetrationDistance;
					pContact->maxPenetrationIndex = pContact->numContacts - 1;
				}
			}
			hits = ST_TRUE;
		}
	}
	return hits;
}

void sphereTraceColliderImposedPlaneBoxDetermineContactPoints(const ST_Direction planeNormal, const ST_Vector3 planePos, const ST_Direction planeRight, const ST_Direction planeForward,
	float xHalfExtents, float zHalfExtents, ST_BoxCollider* const pBoxCollider, ST_BoxContact* const pContact)
{
	float minPenetration = FLT_MAX;
	float maxPenetration = 0.0f;
	pContact->numContacts = 0;
	pContact->pBoxCollider = pBoxCollider;
	pContact->numLerpPoints = 0;
	pContact->maxPenetrationIndex = 0;
	int numEdgeTests = 0;
	int edgesToCheck[4];
	ST_RayTraceData rtd;
	ST_BoxFace closestFace = sphereTraceColliderBoxGetFaceClosestToDirection(pBoxCollider, sphereTraceDirectionNegative(planeNormal));
	//check all points
	for (int i = 0; i < 4; i++)
	{
		sphereTraceColliderInfinitePlaneRayTrace(pBoxCollider->transformedVertices[closestFace.vertexIndices[i]], planeNormal, planeNormal, planePos, &rtd);
		if (rtd.distance > 0.0f)
		{
			if (sphereTraceColliderImposedPlaneIsProjectedPointContained(rtd.contact.point, &planePos, &planeRight, &planeForward, xHalfExtents, zHalfExtents))
			{
				pContact->contactPoints[pContact->numContacts] = rtd.contact;
				pContact->contactPoints[pContact->numContacts].penetrationDistance = rtd.distance;
				pContact->contactPoints[pContact->numContacts].normal = planeNormal;
				pContact->numContacts++;

				if (rtd.distance <= minPenetration)
				{
					minPenetration = rtd.distance;
				}
				if (rtd.distance > maxPenetration)
				{
					maxPenetration = rtd.distance;
					pContact->maxPenetrationIndex = pContact->numContacts - 1;
				}
			}
			else
			{
				int checkInd = sphereTraceColliderImposedPlaneGetClosestTransformedEdgeDirectionToPoint(&planePos, &planeRight, &planeForward, xHalfExtents, zHalfExtents, rtd.contact.point);
				b32 notFound = ST_TRUE;
				for (int i = 0; i < numEdgeTests; i++)
				{
					if (edgesToCheck[i] == checkInd)
					{
						notFound = ST_FALSE;
						break;
					}
				}
				if (notFound)
					edgesToCheck[numEdgeTests++] = checkInd;
			}
		}
		else if (-rtd.distance < ST_SMOOTH_DIST)
		{
			pContact->lerpPoints[pContact->numLerpPoints++] = rtd.contact;
		}
	}
	ST_BoxContact bc;
	b32 isTangential = ST_FALSE;
	ST_Edge edge;
	for (int i = 0; i < numEdgeTests; i++)
	{
		edge = sphereTraceColliderImposedPlaneEdgeDirectionToEdge(&planePos, &planeRight, &planeForward, xHalfExtents, zHalfExtents, edgesToCheck[i]);
		if (sphereTraceColliderBoxPlaneEdgeCollisionTest(&edge, sphereTraceColliderImposedPlaneEdgeDirectionToDirection(planeRight, planeForward, edgesToCheck[i]),
			pBoxCollider, planeNormal, &closestFace, &bc, &isTangential))
		{
			if (isTangential)
			{
				pContact->numContacts = 0;
				minPenetration = FLT_MAX;
				maxPenetration = 0.0f;
			}
			for (int j = 0; j < bc.numContacts; j++)
			{
				pContact->contactPoints[pContact->numContacts] = bc.contactPoints[j];
				pContact->numContacts++;

				if (bc.contactPoints[j].penetrationDistance <= minPenetration)
				{
					minPenetration = bc.contactPoints[j].penetrationDistance;
				}
				if (bc.contactPoints[j].penetrationDistance > maxPenetration)
				{
					maxPenetration = bc.contactPoints[j].penetrationDistance;
					pContact->maxPenetrationIndex = pContact->numContacts - 1;
				}
			}
		}
	}
}

void sphereTraceColliderImposedFaceBoxDetermineContactPoints(const ST_Direction planeNormal, const ST_Vector3 planePos, const ST_Direction planeRight, const ST_Direction planeForward,
	float xHalfExtents, float zHalfExtents, ST_BoxCollider* const pBoxCollider, ST_BoxContact* const pContact)
{
	float minPenetration = FLT_MAX;
	float maxPenetration = 0.0f;
	pContact->numContacts = 0;
	pContact->pBoxCollider = pBoxCollider;
	pContact->maxPenetrationIndex = 0;
	pContact->numLerpPoints = 0;
	int numEdgeTests = 0;
	int edgesToCheck[4];
	ST_RayTraceData rtd;
	ST_BoxFace closestFace = sphereTraceColliderBoxGetFaceClosestToDirection(pBoxCollider, sphereTraceDirectionNegative(planeNormal));
	//check all points
	for (int i = 0; i < 4; i++)
	{
		sphereTraceColliderInfinitePlaneRayTrace(pBoxCollider->transformedVertices[closestFace.vertexIndices[i]], planeNormal, planeNormal, planePos, &rtd);
		if (rtd.distance > 0.0f)
		{
			if (sphereTraceColliderImposedPlaneIsProjectedPointContained(rtd.contact.point, &planePos, &planeRight, &planeForward, xHalfExtents, zHalfExtents))
			{
				pContact->contactPoints[pContact->numContacts] = rtd.contact;
				pContact->contactPoints[pContact->numContacts].penetrationDistance = rtd.distance;
				pContact->contactPoints[pContact->numContacts].normal = planeNormal;
				pContact->numContacts++;

				if (rtd.distance <= minPenetration)
				{
					minPenetration = rtd.distance;
				}
				if (rtd.distance > maxPenetration)
				{
					maxPenetration = rtd.distance;
					pContact->maxPenetrationIndex = pContact->numContacts - 1;
				}
			}
			else
			{
				int checkInd = sphereTraceColliderImposedPlaneGetClosestTransformedEdgeDirectionToPoint(&planePos, &planeRight, &planeForward, xHalfExtents, zHalfExtents, rtd.contact.point);
				b32 notFound = ST_TRUE;
				for (int i = 0; i < numEdgeTests; i++)
				{
					if (edgesToCheck[i] == checkInd)
					{
						notFound = ST_FALSE;
						break;
					}
				}
				if (notFound)
					edgesToCheck[numEdgeTests++] = checkInd;
			}
		}
	}
	ST_BoxContact bc;
	b32 isTangential = ST_FALSE;
	ST_Edge edge;
	for (int i = 0; i < numEdgeTests; i++)
	{
		edge = sphereTraceColliderImposedPlaneEdgeDirectionToEdge(&planePos, &planeRight, &planeForward, xHalfExtents, zHalfExtents, edgesToCheck[i]);
		if (sphereTraceColliderBoxFacesEdgeCollisionTest(&edge, sphereTraceColliderImposedPlaneEdgeDirectionToDirection(planeRight, planeForward, edgesToCheck[i]),
			pBoxCollider, planeNormal, &closestFace, &bc))
		{
			if (isTangential)
			{
				pContact->numContacts = 0;
				minPenetration = FLT_MAX;
				maxPenetration = 0.0f;
			}
			for (int j = 0; j < bc.numContacts; j++)
			{
				pContact->contactPoints[pContact->numContacts] = bc.contactPoints[j];
				pContact->numContacts++;

				if (bc.contactPoints[j].penetrationDistance <= minPenetration)
				{
					minPenetration = bc.contactPoints[j].penetrationDistance;
				}
				if (bc.contactPoints[j].penetrationDistance > maxPenetration)
				{
					maxPenetration = bc.contactPoints[j].penetrationDistance;
					pContact->maxPenetrationIndex = pContact->numContacts - 1;
				}
			}
		}
	}
}


b32 sphereTraceColliderPlaneBoxCollisionTest(ST_BoxCollider* const pBoxCollider, ST_PlaneCollider* const pPlaneCollider, ST_BoxContact* const pContact)
{
	float wa = pBoxCollider->halfExtents.x;
	float ha = pBoxCollider->halfExtents.y;
	float da = pBoxCollider->halfExtents.z;
	float wb = pPlaneCollider->xHalfExtent;
	float db = pPlaneCollider->zHalfExtent;

	ST_Vector3 dp = sphereTraceVector3Subtract(pPlaneCollider->position, pBoxCollider->rigidBody.position);

	float rxx = sphereTraceVector3Dot(pBoxCollider->localRight.v, pPlaneCollider->right.v);
	float rxy = sphereTraceVector3Dot(pBoxCollider->localRight.v, pPlaneCollider->normal.v);
	float rxz = sphereTraceVector3Dot(pBoxCollider->localRight.v, pPlaneCollider->forward.v);
	float ryx = sphereTraceVector3Dot(pBoxCollider->localUp.v, pPlaneCollider->right.v);
	float ryy = sphereTraceVector3Dot(pBoxCollider->localUp.v, pPlaneCollider->normal.v);
	float ryz = sphereTraceVector3Dot(pBoxCollider->localUp.v, pPlaneCollider->forward.v);
	float rzx = sphereTraceVector3Dot(pBoxCollider->localForward.v, pPlaneCollider->right.v);
	float rzy = sphereTraceVector3Dot(pBoxCollider->localForward.v, pPlaneCollider->normal.v);
	float rzz = sphereTraceVector3Dot(pBoxCollider->localForward.v, pPlaneCollider->forward.v);

	float minFacePen = FLT_MAX;
	float minOtherFacePen = FLT_MAX;
	float minEdgePen = FLT_MAX;

	float dpDotRight = sphereTraceVector3Dot(dp, pBoxCollider->localRight.v);
	float dpDotUp = sphereTraceVector3Dot(dp, pBoxCollider->localUp.v);
	float dpDotForward = sphereTraceVector3Dot(dp, pBoxCollider->localForward.v);
	float dpDotOtherRight = sphereTraceVector3Dot(dp, pPlaneCollider->right.v);
	float dpDotOtherUp = sphereTraceVector3Dot(dp, pPlaneCollider->normal.v);
	float dpDotOtherForward = sphereTraceVector3Dot(dp, pPlaneCollider->forward.v);
	ST_Direction normal = pPlaneCollider->normal;

	pContact->pBoxCollider = pBoxCollider;
	pContact->pOtherCollider = pPlaneCollider;
	pContact->numContacts = 0;
	//case ax
	float penetration = sphereTraceAbs(dpDotRight) - (wa + sphereTraceAbs(wb * rxx) + sphereTraceAbs(db * rxz));
	if (penetration > 0.0f)
		return ST_FALSE;
	else
	{
		penetration *= -1.0f;
		if (penetration < minFacePen)
		{
			minFacePen = penetration;
			//pContact->contactPoints[0].point = sphereTraceVector3AddAndScale(pBoxColliderA->rigidBody.position, pBoxColliderA->localRight.v,
			//	sphereTraceSign(dpDotRight) * pBoxColliderA->halfExtents.x);
		}

	}

	//case ay
	penetration = sphereTraceAbs(dpDotUp) - (ha + sphereTraceAbs(wb * ryx) + sphereTraceAbs(db * ryz));
	if (penetration > 0.0f)
		return ST_FALSE;
	else
	{
		penetration *= -1.0f;
		if (penetration < minFacePen)
		{
			minFacePen = penetration;
			//pContact->contactPoints[0].point = sphereTraceVector3AddAndScale(pBoxColliderA->rigidBody.position, pBoxColliderA->localUp.v,
			//	sphereTraceSign(dpDotUp) * pBoxColliderA->halfExtents.y);
		}
	}

	//case az
	penetration = sphereTraceAbs(dpDotForward) - (da + sphereTraceAbs(wb * rzx) + sphereTraceAbs(db * rzz));
	if (penetration > 0.0f)
		return ST_FALSE;
	else
	{
		penetration *= -1.0f;
		if (penetration < minFacePen)
		{
			minFacePen = penetration;
			//pContact->contactPoints[0].point = sphereTraceVector3AddAndScale(pBoxColliderA->rigidBody.position, pBoxColliderA->localForward.v,
			//	sphereTraceSign(dpDotForward) * pBoxColliderA->halfExtents.z);
		}
	}


	//case by
	penetration = sphereTraceAbs(dpDotOtherUp) - (sphereTraceAbs(wa * rxy) + sphereTraceAbs(ha * ryy) + sphereTraceAbs(da * rzy));
	if (penetration > 0.0f)
		return ST_FALSE;
	else
	{
		penetration *= -1.0f;
		if (penetration < minOtherFacePen)
		{
			minOtherFacePen = penetration;
			//pContact->contactPoints[1].point = sphereTraceVector3AddAndScale(pBoxColliderB->rigidBody.position, pBoxColliderB->localUp.v,
			//	-sphereTraceSign(dpDotOtherUp) * pBoxColliderB->halfExtents.y);
			if (dpDotOtherUp > 0.0f)
				normal = sphereTraceDirectionNegative(normal);
		}
	}


	//case ax cross bx
	penetration = sphereTraceAbs(sphereTraceVector3Dot(dp, pBoxCollider->localForward.v) * ryx - sphereTraceVector3Dot(dp, pBoxCollider->localUp.v) * rzx) - (sphereTraceAbs(ha * rzx) +
		sphereTraceAbs(da * ryx) + sphereTraceAbs(db * rxy));
	if (penetration > 0.0f)
		return ST_FALSE;

	//case ax cross bz
	penetration = sphereTraceAbs(sphereTraceVector3Dot(dp, pBoxCollider->localForward.v) * ryz - sphereTraceVector3Dot(dp, pBoxCollider->localUp.v) * rzz) - (sphereTraceAbs(ha * rzz) +
		sphereTraceAbs(da * ryz) + sphereTraceAbs(wb * rxy));
	if (penetration > 0.0f)
		return ST_FALSE;

	//case ay cross bx
	penetration = sphereTraceAbs(sphereTraceVector3Dot(dp, pBoxCollider->localRight.v) * rzx - sphereTraceVector3Dot(dp, pBoxCollider->localForward.v) * rxx) - (sphereTraceAbs(wa * rzx) +
		sphereTraceAbs(da * rxx) + sphereTraceAbs(db * ryy));
	if (penetration > 0.0f)
		return ST_FALSE;

	//case ay cross bz
	penetration = sphereTraceAbs(sphereTraceVector3Dot(dp, pBoxCollider->localRight.v) * rzz - sphereTraceVector3Dot(dp, pBoxCollider->localForward.v) * rxz) - (sphereTraceAbs(wa * rzz) +
		sphereTraceAbs(da * rxz) + sphereTraceAbs(wb * ryy));
	if (penetration > 0.0f)
		return ST_FALSE;

	//case az cross bx
	penetration = sphereTraceAbs(sphereTraceVector3Dot(dp, pBoxCollider->localUp.v) * rxx - sphereTraceVector3Dot(dp, pBoxCollider->localRight.v) * ryx) - (sphereTraceAbs(wa * ryx) +
		sphereTraceAbs(ha * rxx) + sphereTraceAbs(db * rzy));
	if (penetration > 0.0f)
		return ST_FALSE;

	//case az cross bz
	penetration = sphereTraceAbs(sphereTraceVector3Dot(dp, pBoxCollider->localUp.v) * rxz - sphereTraceVector3Dot(dp, pBoxCollider->localRight.v) * ryz) - (sphereTraceAbs(wa * ryz) +
		sphereTraceAbs(ha * rxz) + sphereTraceAbs(wb * rzy));
	if (penetration > 0.0f)
		return ST_FALSE;

	sphereTraceColliderImposedPlaneBoxDetermineContactPoints(normal, pPlaneCollider->position, pPlaneCollider->right, pPlaneCollider->forward, pPlaneCollider->xHalfExtent,
		pPlaneCollider->zHalfExtent, pBoxCollider, pContact);
	return pContact->numContacts>0;
}

void sphereTraceColliderBoxConstructImposedPlaneWithFace(ST_BoxCollider* const pBoxCollider, ST_DirectionType faceDir, ST_Vector3* const pPlanePos, ST_Direction* const pPlaneNormal,
	ST_Direction* const pPlaneRight, ST_Direction* const pPlaneForward, float* pxHalfExtents, float* pzHalfExtents)
{
	switch (faceDir)
	{
	case ST_DIRECTION_RIGHT:
		*pPlanePos     = sphereTraceVector3AddAndScale(pBoxCollider->rigidBody.position, pBoxCollider->localRight.v, pBoxCollider->halfExtents.x);
		*pPlaneNormal  = pBoxCollider->localRight;
		*pPlaneForward = pBoxCollider->localForward;
		*pPlaneRight   = pBoxCollider->localUp;
		*pxHalfExtents = pBoxCollider->halfExtents.y;
		*pzHalfExtents = pBoxCollider->halfExtents.z;
		break;
	case ST_DIRECTION_LEFT:
		*pPlanePos      = sphereTraceVector3AddAndScale(pBoxCollider->rigidBody.position, pBoxCollider->localRight.v, -pBoxCollider->halfExtents.x);
		*pPlaneNormal   = sphereTraceDirectionNegative(pBoxCollider->localRight);
		*pPlaneForward  = pBoxCollider->localForward;
		*pPlaneRight    = pBoxCollider->localUp;
		*pxHalfExtents  = pBoxCollider->halfExtents.y;
		*pzHalfExtents  = pBoxCollider->halfExtents.z;
		break;
	case ST_DIRECTION_UP:
		*pPlanePos      = sphereTraceVector3AddAndScale(pBoxCollider->rigidBody.position, pBoxCollider->localUp.v, pBoxCollider->halfExtents.y);
		*pPlaneNormal   = pBoxCollider->localUp;
		*pPlaneForward  = pBoxCollider->localForward;
		*pPlaneRight    = pBoxCollider->localRight;
		*pxHalfExtents  = pBoxCollider->halfExtents.x;
		*pzHalfExtents  = pBoxCollider->halfExtents.z;
		break;
	case ST_DIRECTION_DOWN:
		*pPlanePos      = sphereTraceVector3AddAndScale(pBoxCollider->rigidBody.position, pBoxCollider->localUp.v, -pBoxCollider->halfExtents.y);
		*pPlaneNormal   = sphereTraceDirectionNegative(pBoxCollider->localUp);
		*pPlaneForward  = pBoxCollider->localForward;
		*pPlaneRight    = pBoxCollider->localRight;
		*pxHalfExtents  = pBoxCollider->halfExtents.x;
		*pzHalfExtents  = pBoxCollider->halfExtents.z;
		break;
	case ST_DIRECTION_FORWARD:
		*pPlanePos      = sphereTraceVector3AddAndScale(pBoxCollider->rigidBody.position, pBoxCollider->localForward.v, pBoxCollider->halfExtents.z);
		*pPlaneNormal   = pBoxCollider->localForward;
		*pPlaneForward  = pBoxCollider->localUp;
		*pPlaneRight    = pBoxCollider->localRight;
		*pxHalfExtents  = pBoxCollider->halfExtents.x;
		*pzHalfExtents  = pBoxCollider->halfExtents.y;
		break;
	case ST_DIRECTION_BACK:
		*pPlanePos     = sphereTraceVector3AddAndScale(pBoxCollider->rigidBody.position, pBoxCollider->localForward.v, -pBoxCollider->halfExtents.z);
		*pPlaneNormal  = sphereTraceDirectionNegative(pBoxCollider->localForward);
		*pPlaneForward = pBoxCollider->localUp;
		*pPlaneRight   = pBoxCollider->localRight;
		*pxHalfExtents = pBoxCollider->halfExtents.x;
		*pzHalfExtents = pBoxCollider->halfExtents.y;
		break;
	}
}


b32 sphereTraceColliderBoxBoxCollisionTest(ST_BoxCollider* const pBoxColliderA, ST_BoxCollider* const pBoxColliderB, ST_BoxContact* const pContact)
{
	float wa = pBoxColliderA->halfExtents.x;
	float ha = pBoxColliderA->halfExtents.y;
	float da = pBoxColliderA->halfExtents.z;
	float wb = pBoxColliderB->halfExtents.x;
	float hb = pBoxColliderB->halfExtents.y;
	float db = pBoxColliderB->halfExtents.z;
	ST_Vector3 dp = sphereTraceVector3Subtract(pBoxColliderB->rigidBody.position, pBoxColliderA->rigidBody.position);
	float rxx = sphereTraceVector3Dot(pBoxColliderA->localRight.v, pBoxColliderB->localRight.v);
	float rxy = sphereTraceVector3Dot(pBoxColliderA->localRight.v, pBoxColliderB->localUp.v);
	float rxz = sphereTraceVector3Dot(pBoxColliderA->localRight.v, pBoxColliderB->localForward.v);
	float ryx = sphereTraceVector3Dot(pBoxColliderA->localUp.v, pBoxColliderB->localRight.v);
	float ryy = sphereTraceVector3Dot(pBoxColliderA->localUp.v, pBoxColliderB->localUp.v);
	float ryz = sphereTraceVector3Dot(pBoxColliderA->localUp.v, pBoxColliderB->localForward.v);
	float rzx = sphereTraceVector3Dot(pBoxColliderA->localForward.v, pBoxColliderB->localRight.v);
	float rzy = sphereTraceVector3Dot(pBoxColliderA->localForward.v, pBoxColliderB->localUp.v);
	float rzz = sphereTraceVector3Dot(pBoxColliderA->localForward.v, pBoxColliderB->localForward.v);

	float minFacePen = FLT_MAX;
	float minOtherFacePen = FLT_MAX;
	float minEdgePen = FLT_MAX;

	float dpDotRight = sphereTraceVector3Dot(dp, pBoxColliderA->localRight.v);
	float dpDotUp = sphereTraceVector3Dot(dp, pBoxColliderA->localUp.v);
	float dpDotForward = sphereTraceVector3Dot(dp, pBoxColliderA->localForward.v);
	float dpDotOtherRight = sphereTraceVector3Dot(dp, pBoxColliderB->localRight.v);
	float dpDotOtherUp = sphereTraceVector3Dot(dp, pBoxColliderB->localUp.v);
	float dpDotOtherForward = sphereTraceVector3Dot(dp, pBoxColliderB->localForward.v);
	

	//ST_DirectionType otherFaceDir;
	
	pContact->numContacts = 0;
	//case ax
	float penetration = sphereTraceAbs(dpDotRight) - (wa + sphereTraceAbs(wb * rxx) + sphereTraceAbs(hb * rxy) + sphereTraceAbs(db * rxz));
	if (penetration > 0.0f)
		return ST_FALSE;
	else
	{
		penetration *= -1.0f;
		if (penetration < minFacePen)
		{
			minFacePen = penetration;
		}
		
	}

	//case ay
	penetration = sphereTraceAbs(dpDotUp) - (ha + sphereTraceAbs(wb * ryx) + sphereTraceAbs(hb * ryy) + sphereTraceAbs(db * ryz));
	if (penetration > 0.0f)
		return ST_FALSE;
	else
	{
		penetration *= -1.0f;
		if (penetration < minFacePen)
		{
			minFacePen = penetration;
		}
	}

	//case az
	penetration = sphereTraceAbs(dpDotForward) - (da + sphereTraceAbs(wb * rzx) + sphereTraceAbs(hb * rzy) + sphereTraceAbs(db * rzz));
	if (penetration > 0.0f)
		return ST_FALSE;
	else
	{
		penetration *= -1.0f;
		if (penetration < minFacePen)
		{
			minFacePen = penetration;
		}
	}

	//case bx
	penetration = sphereTraceAbs(dpDotOtherRight) - (sphereTraceAbs(wa * rxx) + sphereTraceAbs(ha * ryx) + sphereTraceAbs(da * rzx) + wb);
	if (penetration > 0.0f)
		return ST_FALSE;
	else
	{
		penetration *= -1.0f;
		if (penetration < minOtherFacePen)
		{
			minOtherFacePen = penetration;
			if (dpDotOtherRight >= 0.0f)
			{
				pContact->otherBoxPlaneDirection = ST_DIRECTION_LEFT;
			}
			else
			{
				pContact->otherBoxPlaneDirection = ST_DIRECTION_RIGHT;
			}
		}
	}

	//case by
	penetration = sphereTraceAbs(dpDotOtherUp) - (sphereTraceAbs(wa * rxy) + sphereTraceAbs(ha * ryy) + sphereTraceAbs(da * rzy) + hb);
	if (penetration > 0.0f)
		return ST_FALSE;
	else
	{
		penetration *= -1.0f;
		if (penetration < minOtherFacePen)
		{
			minOtherFacePen = penetration;
			if (dpDotOtherUp >= 0.0f)
			{
				pContact->otherBoxPlaneDirection = ST_DIRECTION_DOWN;
			}
			else
			{
				pContact->otherBoxPlaneDirection = ST_DIRECTION_UP;
			}
		}
	}

	//case bz 
	penetration = sphereTraceAbs(dpDotOtherForward) - (sphereTraceAbs(wa * rxz) + sphereTraceAbs(ha * ryz) + sphereTraceAbs(da * rzz) + db);
	if (penetration > 0.0f)
		return ST_FALSE;
	else
	{
		penetration *= -1.0f;
		if (penetration < minOtherFacePen)
		{
			minOtherFacePen = penetration;
			if (dpDotOtherForward >= 0.0f)
			{
				pContact->otherBoxPlaneDirection = ST_DIRECTION_BACK;
			}
			else
			{
				pContact->otherBoxPlaneDirection = ST_DIRECTION_FORWARD;
			}
		}
	}

	//case ax cross bx
	penetration = sphereTraceAbs(sphereTraceVector3Dot(dp, pBoxColliderA->localForward.v) * ryx - sphereTraceVector3Dot(dp, pBoxColliderA->localUp.v) * rzx) - (sphereTraceAbs(ha * rzx) +
		sphereTraceAbs(da * ryx) + sphereTraceAbs(hb * rxz) + sphereTraceAbs(db * rxy));
	if (penetration > 0.0f)
		return ST_FALSE;

	//case ax cross by
	penetration = sphereTraceAbs(sphereTraceVector3Dot(dp, pBoxColliderA->localForward.v) * ryy - sphereTraceVector3Dot(dp, pBoxColliderA->localUp.v) * rzy) - (sphereTraceAbs(ha * rzy) +
		sphereTraceAbs(da * ryy) + sphereTraceAbs(wb * rxz) + sphereTraceAbs(db * rxx));
	if (penetration > 0.0f)
		return ST_FALSE;

	//case ax cross bz
	penetration = sphereTraceAbs(sphereTraceVector3Dot(dp, pBoxColliderA->localForward.v) * ryz - sphereTraceVector3Dot(dp, pBoxColliderA->localUp.v) * rzz) - (sphereTraceAbs(ha * rzz) +
		sphereTraceAbs(da * ryz) + sphereTraceAbs(wb * rxy) + sphereTraceAbs(hb * rxx));
	if (penetration > 0.0f)
		return ST_FALSE;

	//case ay cross bx
	penetration = sphereTraceAbs(sphereTraceVector3Dot(dp, pBoxColliderA->localRight.v) * rzx - sphereTraceVector3Dot(dp, pBoxColliderA->localForward.v) * rxx) - (sphereTraceAbs(wa * rzx) +
		sphereTraceAbs(da * rxx) + sphereTraceAbs(hb * ryz) + sphereTraceAbs(db * ryy));
	if (penetration > 0.0f)
		return ST_FALSE;

	//case ay cross by
	penetration = sphereTraceAbs(sphereTraceVector3Dot(dp, pBoxColliderA->localRight.v) * rzy - sphereTraceVector3Dot(dp, pBoxColliderA->localForward.v) * rxy) - (sphereTraceAbs(wa * rzy) +
		sphereTraceAbs(da * rxy) + sphereTraceAbs(wb * ryz) + sphereTraceAbs(db * ryx));
	if (penetration > 0.0f)
		return ST_FALSE;

	//case ay cross bz
	penetration = sphereTraceAbs(sphereTraceVector3Dot(dp, pBoxColliderA->localRight.v) * rzz - sphereTraceVector3Dot(dp, pBoxColliderA->localForward.v) * rxz) - (sphereTraceAbs(wa * rzz) +
		sphereTraceAbs(da * rxz) + sphereTraceAbs(wb * ryy) + sphereTraceAbs(hb * ryx));
	if (penetration > 0.0f)
		return ST_FALSE;

	//case az cross bx
	penetration = sphereTraceAbs(sphereTraceVector3Dot(dp, pBoxColliderA->localUp.v) * rxx - sphereTraceVector3Dot(dp, pBoxColliderA->localRight.v) * ryx) - (sphereTraceAbs(wa * ryx) +
		sphereTraceAbs(ha * rxx) + sphereTraceAbs(hb * rzz) + sphereTraceAbs(db * rzy));
	if (penetration > 0.0f)
		return ST_FALSE;

	//case az cross by
	penetration = sphereTraceAbs(sphereTraceVector3Dot(dp, pBoxColliderA->localUp.v) * rxy - sphereTraceVector3Dot(dp, pBoxColliderA->localRight.v) * ryy) - (sphereTraceAbs(wa * ryy) +
		sphereTraceAbs(ha * rxy) + sphereTraceAbs(wb * rzz) + sphereTraceAbs(db * rzx));
	if (penetration > 0.0f)
		return ST_FALSE;

	//case az cross bz
	penetration = sphereTraceAbs(sphereTraceVector3Dot(dp, pBoxColliderA->localUp.v) * rxz - sphereTraceVector3Dot(dp, pBoxColliderA->localRight.v) * ryz) - (sphereTraceAbs(wa * ryz) +
		sphereTraceAbs(ha * rxz) + sphereTraceAbs(wb * rzy) + sphereTraceAbs(hb * rzx));
	if (penetration > 0.0f)
		return ST_FALSE;

	ST_Direction planeNormal, planeRight, planeForward;
	float xHalfExtents, zHalfExtents;
	ST_Vector3 planePos;

	sphereTraceColliderBoxConstructImposedPlaneWithFace(pBoxColliderB, pContact->otherBoxPlaneDirection, &planePos, &planeNormal, 
		&planeRight, &planeForward, &xHalfExtents, &zHalfExtents);
	sphereTraceColliderImposedFaceBoxDetermineContactPoints(planeNormal, planePos, planeRight, planeForward, xHalfExtents,
		zHalfExtents, pBoxColliderA, pContact);
	pContact->pBoxCollider = pBoxColliderA;
	pContact->pOtherCollider = pBoxColliderB;

	return pContact->numContacts>0;
}

b32 sphereTraceColliderPlaneBoxTraceOut1(ST_BoxCollider* const pBoxCollider, ST_Direction clipoutDir, ST_PlaneCollider* const pPlaneCollider, ST_BoxTraceData* const pBoxTraceData)
{
	sphereTraceDirectionNormalizeIfNotNormalizedByRef(&clipoutDir);
	if (sphereTraceColliderPlaneBoxTrace(sphereTraceVector3AddAndScale(pBoxCollider->rigidBody.position, clipoutDir.v, pPlaneCollider->collider.boundingRadius+pBoxCollider->collider.boundingRadius),
		pBoxCollider->halfExtents, pBoxCollider->rigidBody.rotation, sphereTraceDirectionNegative(clipoutDir), pPlaneCollider, pBoxTraceData))
	{
		return ST_TRUE;
	}
	return ST_FALSE;
}

b32 sphereTraceColliderBoxBoxTraceOut1(ST_BoxCollider* const pBoxCollider, ST_Direction clipoutDir, ST_BoxCollider* const pOtherBox, ST_DirectionType faceDir, ST_BoxTraceData* const pBoxTraceData)
{
	sphereTraceDirectionNormalizeIfNotNormalizedByRef(&clipoutDir);
	ST_PlaneCollider pc;
	sphereTraceColliderBoxConstructImposedPlaneWithFace(pOtherBox, faceDir, &pc.position, &pc.normal,
		&pc.right, &pc.forward, &pc.xHalfExtent, &pc.zHalfExtent);
	sphereTraceColliderPlaneSetTransformedVerticesAndEdges(&pc);
	float boundingRadius = sqrtf(pc.xHalfExtent * pc.xHalfExtent + pc.zHalfExtent * pc.zHalfExtent) + pBoxCollider->collider.boundingRadius;
	if (sphereTraceColliderPlaneBoxTrace(sphereTraceVector3AddAndScale(pBoxCollider->rigidBody.position, clipoutDir.v, boundingRadius),
		pBoxCollider->halfExtents, pBoxCollider->rigidBody.rotation, sphereTraceDirectionNegative(clipoutDir), &pc, pBoxTraceData))
	{
		return ST_TRUE;
	}
	return ST_FALSE;
}

b32 sphereTraceColliderBoxBoxTraceOut2(ST_BoxCollider* const pBoxCollider, ST_Direction clipoutDir, ST_BoxCollider* const pOtherBox, ST_BoxTraceData* const pBoxTraceData)
{
	ST_Direction negDir = sphereTraceDirectionNegative(clipoutDir);
	ST_DirectionType dirRight, dirUp, dirFor;
	float dotRight = sphereTraceVector3Dot(negDir.v, pOtherBox->localRight.v);
	if (dotRight >= 0.0f)
		dirRight = ST_DIRECTION_RIGHT;
	else
		dirRight = ST_DIRECTION_LEFT;
	float dotUp = sphereTraceVector3Dot(negDir.v, pOtherBox->localUp.v);
	if (dotUp >= 0.0f)
		dirUp = ST_DIRECTION_UP;
	else
		dirUp = ST_DIRECTION_DOWN;
	float dotForward = sphereTraceVector3Dot(negDir.v, pOtherBox->localForward.v);
	if (dotForward >= 0.0f)
		dirFor = ST_DIRECTION_FORWARD;
	else
		dirFor = ST_DIRECTION_BACK;
	sphereTraceDirectionNormalizeIfNotNormalizedByRef(&clipoutDir);
	float minDist = FLT_MAX;
	ST_BoxTraceData btd;
	float boundingRadius = pOtherBox->collider.boundingRadius+ pBoxCollider->collider.boundingRadius;
	ST_PlaneCollider pc;
	sphereTraceColliderBoxConstructImposedPlaneWithFace(pOtherBox, dirRight, &pc.position, &pc.normal,
		&pc.right, &pc.forward, &pc.xHalfExtent, &pc.zHalfExtent);
	sphereTraceColliderPlaneSetTransformedVerticesAndEdges(&pc);
	if (sphereTraceColliderPlaneBoxTrace(sphereTraceVector3AddAndScale(pBoxCollider->rigidBody.position, clipoutDir.v, boundingRadius),
		pBoxCollider->halfExtents, pBoxCollider->rigidBody.rotation, negDir, &pc, pBoxTraceData))
	{
		minDist = pBoxTraceData->traceDistance;
	}

	sphereTraceColliderBoxConstructImposedPlaneWithFace(pOtherBox, dirUp, &pc.position, &pc.normal,
		&pc.right, &pc.forward, &pc.xHalfExtent, &pc.zHalfExtent);
	sphereTraceColliderPlaneSetTransformedVerticesAndEdges(&pc);
	//boundingRadius = sqrtf(pc.xHalfExtent * pc.xHalfExtent + pc.zHalfExtent * pc.zHalfExtent) + pBoxCollider->collider.boundingRadius;
	if (sphereTraceColliderPlaneBoxTrace(sphereTraceVector3AddAndScale(pBoxCollider->rigidBody.position, clipoutDir.v, boundingRadius),
		pBoxCollider->halfExtents, pBoxCollider->rigidBody.rotation, negDir, &pc, &btd))
	{
		if (btd.traceDistance < minDist)
		{
			*pBoxTraceData = btd;
			minDist = btd.traceDistance;
		}
	}

	sphereTraceColliderBoxConstructImposedPlaneWithFace(pOtherBox, dirFor, &pc.position, &pc.normal,
		&pc.right, &pc.forward, &pc.xHalfExtent, &pc.zHalfExtent);
	sphereTraceColliderPlaneSetTransformedVerticesAndEdges(&pc);
	//boundingRadius = sqrtf(pc.xHalfExtent * pc.xHalfExtent + pc.zHalfExtent * pc.zHalfExtent) + pBoxCollider->collider.boundingRadius;
	if (sphereTraceColliderPlaneBoxTrace(sphereTraceVector3AddAndScale(pBoxCollider->rigidBody.position, clipoutDir.v, boundingRadius),
		pBoxCollider->halfExtents, pBoxCollider->rigidBody.rotation, negDir, &pc, &btd))
	{
		if (btd.traceDistance < minDist)
		{
			*pBoxTraceData = btd;
			minDist = btd.traceDistance;
		}
	}

	return minDist < FLT_MAX;
}

