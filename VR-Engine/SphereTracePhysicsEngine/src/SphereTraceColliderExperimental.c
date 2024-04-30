#include "SphereTraceColliderExperimental.h"
#include "SphereTraceGlobals.h"

ST_BowlCollider sphereTraceColliderBowlConstruct(ST_Vector3 position, float radius, ST_Direction normal)
{
	ST_BowlCollider bowlCollider;
	bowlCollider.collider = sphereTraceColliderConstruct(COLLIDER_BOWL, radius);
	bowlCollider.position = position;
	bowlCollider.radius = radius;
	bowlCollider.normal = normal;
	//bowlCollider.collider.colliderType = COLLIDER_BOWL;
	//bowlCollider.collider.bucketIndices = sphereTraceIndexListConstruct();
	sphereTraceColliderBowlsSetAABB(&bowlCollider);
	return bowlCollider;
}

b32 sphereTraceColliderBowlImposedSphereCollisionTest(ST_BowlCollider* const pBowlCollider, ST_Vector3 imposedPosition, float imposedRadius, ST_SphereContact* pContact)
{
	ST_Vector3 dp = sphereTraceVector3Subtract(imposedPosition, pBowlCollider->position);
	float dist = sphereTraceVector3Length(dp);
	float normDist = sphereTraceVector3Dot(pBowlCollider->normal.v, dp);
	if (normDist <= 0.0f)
	{
		if (dist > pBowlCollider->radius && dist <= (pBowlCollider->radius + imposedRadius))
		{
			//outside of bowl
			pContact->collisionType = ST_COLLISION_OUTWARD_SPHEREICAL;
			pContact->pOtherCollider = pBowlCollider;
			pContact->otherColliderType = COLLIDER_BOWL;
			pContact->normal = sphereTraceDirectionConstruct(sphereTraceVector3Scale(dp, 1.0f / dist), 1);
			pContact->point = sphereTraceVector3AddAndScale(pBowlCollider->position, pContact->normal.v, pBowlCollider->radius);
			pContact->penetrationDistance = pBowlCollider->radius + imposedRadius - dist;
			pContact->radiusOfCurvature = pBowlCollider->radius;
			return 1;
		}
		else if (dist < pBowlCollider->radius && dist >= (pBowlCollider->radius - imposedRadius))
		{
			//inside of bowl
			pContact->collisionType = ST_COLLISION_INWARD_SPHEREICAL;
			pContact->pOtherCollider = pBowlCollider;
			pContact->otherColliderType = COLLIDER_BOWL;
			pContact->normal = sphereTraceDirectionConstruct(sphereTraceVector3Scale(dp, -1.0f / dist), 1);
			pContact->point = sphereTraceVector3AddAndScale(pBowlCollider->position, sphereTraceVector3Negative(pContact->normal.v), pBowlCollider->radius);
			pContact->penetrationDistance = -pBowlCollider->radius + imposedRadius + dist;
			pContact->radiusOfCurvature = pBowlCollider->radius;
			return 1;
		}
	}
	else if (normDist <= imposedRadius)
	{
		ST_Vector3 bowlRight = sphereTraceVector3Normalize(sphereTraceVector3Cross(dp, pBowlCollider->normal.v));
		ST_Vector3 bowlFwd = sphereTraceVector3Cross(pBowlCollider->normal.v, bowlRight);
		ST_Vector3 bowlCirclePlanePos = sphereTraceVector3AddAndScale2(gVector3Zero, bowlRight, sphereTraceVector3Dot(bowlRight, dp),
			bowlFwd, sphereTraceVector3Dot(bowlFwd, dp));
		ST_Vector3 bowlCirclePlaneDir = sphereTraceVector3Normalize(bowlCirclePlanePos);
		ST_Vector3 closestPointOnCircle = sphereTraceVector3AddAndScale(pBowlCollider->position, bowlCirclePlaneDir, pBowlCollider->radius);
		ST_Vector3 dEdge = sphereTraceVector3Subtract(imposedPosition, closestPointOnCircle);
		float dist = sphereTraceVector3Length(dEdge);
		if (dist <= imposedRadius)
		{
			pContact->collisionType = ST_COLLISION_EDGE;
			pContact->pOtherCollider = pBowlCollider;
			pContact->otherColliderType = COLLIDER_BOWL;
			pContact->normal = sphereTraceDirectionConstruct(sphereTraceVector3Scale(dEdge, 1.0f / dist), 1);
			pContact->point = closestPointOnCircle;
			pContact->penetrationDistance = imposedRadius - dist;
			return 1;
		}
	}

	return 0;
}


b32 sphereTraceColliderBowlSphereCollisionTest(ST_BowlCollider* const pBowlCollider, ST_SphereCollider* const pSphere, ST_SphereContact* pContact)
{
	if (sphereTraceColliderBowlImposedSphereCollisionTest(pBowlCollider, pSphere->rigidBody.position, pSphere->radius, pContact))
	{
		pContact->pSphereCollider = pSphere;
		return 1;
	}
	return 0;
}


void sphereTraceColliderBowlsSetAABB(ST_BowlCollider* const pBowlCollider)
{
	ST_Vector3 centroid = pBowlCollider->position;
	pBowlCollider->collider.aabb.halfExtents = sphereTraceVector3UniformSize(pBowlCollider->radius);
	pBowlCollider->collider.aabb.highExtent = sphereTraceVector3Add(centroid, pBowlCollider->collider.aabb.halfExtents);
	pBowlCollider->collider.aabb.lowExtent = sphereTraceVector3Subtract(centroid, pBowlCollider->collider.aabb.halfExtents);
	pBowlCollider->collider.aabb.center = centroid;
}


ST_PipeCollider sphereTraceColliderPipeConstruct(ST_Vector3 position, float radius, float length, ST_Direction up)
{
	ST_PipeCollider pipeCollider;
	pipeCollider.position = position;
	pipeCollider.radius = radius;
	pipeCollider.up = up;
	pipeCollider.length = length;
	float dot = sphereTraceVector3Dot(up.v, gVector3Up);
	if (dot == 1.0f || dot == -1.0f)
	{
		pipeCollider.right = gDirectionRight;
		pipeCollider.forward = gDirectionForward;
	}
	else
	{
		pipeCollider.right = sphereTraceDirectionConstructNormalized(sphereTraceVector3Cross(up.v, gVector3Up));
		pipeCollider.forward = sphereTraceDirectionConstructNormalized(sphereTraceVector3Cross(up.v, pipeCollider.right.v));
	}
	//pipeCollider.collider.colliderType = COLLIDER_PIPE;
	float boundingRadius = sqrtf((length * 0.5f) * (length * 0.5f) + radius * radius);
	pipeCollider.collider = sphereTraceColliderConstruct(COLLIDER_PIPE, boundingRadius);
	pipeCollider.collider.bucketIndices = sphereTraceIndexListConstruct();
	sphereTraceColliderPipeSetAABB(&pipeCollider);
	return pipeCollider;
}

b32 sphereTraceColliderPipeImposedSphereCollisionTest(ST_PipeCollider* const pPipeCollider, ST_Vector3 imposedPosition, float imposedRadius, ST_SphereContact* pContact)
{
	ST_Vector3 dp = sphereTraceVector3Subtract(imposedPosition, pPipeCollider->position);
	float upDist = sphereTraceVector3Dot(pPipeCollider->up.v, dp);
	if (sphereTraceAbs(upDist) < pPipeCollider->length * 0.5f)
	{
		float rightDist = sphereTraceVector3Dot(dp, pPipeCollider->right.v);
		float fwdDist = sphereTraceVector3Dot(dp, pPipeCollider->forward.v);
		ST_Vector3 tangent = sphereTraceVector3AddAndScale2(gVector3Zero, pPipeCollider->right.v, rightDist, pPipeCollider->forward.v, fwdDist);
		float tangentDist = sphereTraceVector3Length(tangent);
		if (tangentDist <= pPipeCollider->radius && tangentDist >= (pPipeCollider->radius - imposedRadius))
		{
			pContact->collisionType = ST_COLLISION_INWARD_CIRCULAR;
			pContact->pOtherCollider = pPipeCollider;
			pContact->otherColliderType = COLLIDER_PIPE;
			pContact->point = sphereTraceVector3AddAndScale2(pPipeCollider->position, pPipeCollider->up.v, upDist, tangent, pPipeCollider->radius / tangentDist);
			pContact->normal = sphereTraceDirectionConstruct(sphereTraceVector3Scale(tangent, -1.0f / tangentDist), 1);
			pContact->penetrationDistance = imposedRadius - pPipeCollider->radius + tangentDist;
			pContact->radiusOfCurvature = pPipeCollider->radius;
			pContact->bitangent = pPipeCollider->up;
			return 1;
		}
		else if (tangentDist > pPipeCollider->radius && tangentDist <= (pPipeCollider->radius + imposedRadius))
		{
			pContact->collisionType = ST_COLLISION_OUTWARD_CIRCULAR;
			pContact->pOtherCollider = pPipeCollider;
			pContact->otherColliderType = COLLIDER_PIPE;
			pContact->point = sphereTraceVector3AddAndScale2(pPipeCollider->position, pPipeCollider->up.v, upDist, tangent, pPipeCollider->radius / tangentDist);
			pContact->normal = sphereTraceDirectionConstruct(sphereTraceVector3Scale(tangent, 1.0f / tangentDist), 1);
			pContact->penetrationDistance = imposedRadius - (tangentDist - pPipeCollider->radius);
			pContact->radiusOfCurvature = pPipeCollider->radius;
			pContact->bitangent = pPipeCollider->up;
			return 1;
		}
	}
	else if (upDist > pPipeCollider->length * 0.5f && upDist <= (pPipeCollider->length * 0.5f + imposedRadius))
	{
		float rightDist = sphereTraceVector3Dot(dp, pPipeCollider->right.v);
		float fwdDist = sphereTraceVector3Dot(dp, pPipeCollider->forward.v);
		ST_Vector3 tangent = sphereTraceVector3AddAndScale2(gVector3Zero, pPipeCollider->right.v, rightDist, pPipeCollider->forward.v, fwdDist);
		float tangentDist = sphereTraceVector3Length(tangent);

		ST_Vector3 closestPointOnCircle = sphereTraceVector3AddAndScale2(pPipeCollider->position, pPipeCollider->up.v, pPipeCollider->length * 0.5f,
			tangent, pPipeCollider->radius / tangentDist);
		ST_Vector3 dEdge = sphereTraceVector3Subtract(imposedPosition, closestPointOnCircle);
		float dist = sphereTraceVector3Length(dEdge);
		if (dist <= imposedRadius)
		{
			pContact->collisionType = ST_COLLISION_EDGE;
			pContact->pOtherCollider = pPipeCollider;
			pContact->otherColliderType = COLLIDER_PIPE;
			pContact->normal = sphereTraceDirectionConstruct(sphereTraceVector3Scale(dEdge, 1.0f / dist), 1);
			pContact->point = closestPointOnCircle;
			pContact->penetrationDistance = imposedRadius - dist;
			return 1;
		}
	}
	else if (upDist < -pPipeCollider->length * 0.5f && upDist >= (-pPipeCollider->length * 0.5f - imposedRadius))
	{
		float rightDist = sphereTraceVector3Dot(dp, pPipeCollider->right.v);
		float fwdDist = sphereTraceVector3Dot(dp, pPipeCollider->forward.v);
		ST_Vector3 tangent = sphereTraceVector3AddAndScale2(gVector3Zero, pPipeCollider->right.v, rightDist, pPipeCollider->forward.v, fwdDist);
		float tangentDist = sphereTraceVector3Length(tangent);

		ST_Vector3 closestPointOnCircle = sphereTraceVector3AddAndScale2(pPipeCollider->position, pPipeCollider->up.v, -pPipeCollider->length * 0.5f,
			tangent, pPipeCollider->radius / tangentDist);
		ST_Vector3 dEdge = sphereTraceVector3Subtract(imposedPosition, closestPointOnCircle);
		float dist = sphereTraceVector3Length(dEdge);
		if (dist <= imposedRadius)
		{
			pContact->collisionType = ST_COLLISION_EDGE;
			pContact->pOtherCollider = pPipeCollider;
			pContact->otherColliderType = COLLIDER_PIPE;
			pContact->normal = sphereTraceDirectionConstruct(sphereTraceVector3Scale(dEdge, 1.0f / dist), 1);
			pContact->point = closestPointOnCircle;
			pContact->penetrationDistance = imposedRadius - dist;
			return 1;
		}
	}
	return 0;
}

b32 sphereTraceColliderPipeSphereCollisionTest(ST_PipeCollider* const pPipeCollider, ST_SphereCollider* const pSphere, ST_SphereContact* pContact)
{
	if (sphereTraceColliderPipeImposedSphereCollisionTest(pPipeCollider, pSphere->rigidBody.position, pSphere->radius, pContact))
	{
		pContact->pSphereCollider = pSphere;
		return 1;
	}
	return 0;
}

void sphereTraceColliderPipeSetAABB(ST_PipeCollider* const pPipeCollider)
{
	ST_Vector3 startPoint = sphereTraceVector3AddAndScale(pPipeCollider->position, pPipeCollider->up.v, pPipeCollider->length * -0.5f);
	ST_Vector3 endPoint = sphereTraceVector3AddAndScale(pPipeCollider->position, pPipeCollider->up.v, pPipeCollider->length * 0.5f);
	ST_SphereTraceData stdDummy;
	stdDummy.radius = pPipeCollider->radius;
	stdDummy.rayTraceData.startPoint = startPoint;
	stdDummy.sphereCenter = endPoint;
	sphereTraceColliderResizeAABBWithSpherecast(&stdDummy, &pPipeCollider->collider.aabb);
	pPipeCollider->collider.aabb.center = sphereTraceVector3Average(startPoint, endPoint);
}