#pragma once
#include "SphereTraceCollider.h"

//
ST_SphereCollider sphereTraceColliderSphereConstruct(float radius);
//
void sphereTraceColliderSphereSetPosition(ST_SphereCollider* pSphere, ST_Vector3 position);
//
void sphereTraceColliderSphereSetRadius(ST_SphereCollider* pSphere, float radius);
//
void sphereTraceColliderSphereAABBSetTransformedVertices(ST_SphereCollider* const pSphereCollider);
//
b32 sphereTraceColliderAABBIntersectImposedSphere(const ST_AABB* const aabb, ST_Vector3 imposedPosition, float imposedRadius);
//
b32 sphereTraceColliderAABBIntersectSphere(const ST_AABB* const aabb, const ST_SphereCollider* const pSphereCollider);
//
b32 sphereTraceColliderInfinitePlaneImposedSphereCollisionTest(ST_Vector3 imposedPosition, float imposedRadius, ST_Direction planeNormal, ST_Vector3 pointOnPlane, ST_SphereContact* const pContact);
//
b32 sphereTraceColliderSphereRayTrace(ST_Vector3 start, ST_Direction dir, const ST_SphereCollider* const pSphere, ST_RayTraceData* const pData);
//
b32 sphereTraceColliderImposedSphereRayTrace(ST_Vector3 start, ST_Direction dir, ST_Vector3 imposedPosition, float imposedRadius, ST_RayTraceData* const pData);
//
b32 sphereTraceColliderSphereSphereTrace(ST_Vector3 start, ST_Direction dir, float radius, const ST_SphereCollider* const pSphere, ST_SphereTraceData* const pData);
//
b32 sphereTraceColliderImposedSphereSphereTrace(ST_Vector3 start, ST_Direction dir, float radius, ST_Vector3 imposedPosition, float imposedRadius, ST_SphereTraceData* const pData);
//
b32 sphereTraceColliderImposedSphereRayTraceClampLength(ST_Vector3 start, ST_Direction dir, ST_Vector3 imposedPosition, float imposedRadius, ST_RayTraceData* const pData);
//
b32 sphereTraceColliderPointImposedSphereCollisionTest(ST_Vector3 point, ST_Vector3 imposedPosition, float imposedRadius, ST_SphereContact* const pContact);
//
b32 sphereTraceColliderEdgeImposedSphereCollisionTest(const ST_Edge* const pEdge, ST_Vector3 imposedPosition, float imposedRadius, ST_SphereContact* const pContact);
//
b32 sphereTraceColliderRingImposedSphereCollisionTest(const ST_Ring* const pRing, ST_Vector3 imposedPosition, float imposedRadius, ST_SphereContact* const pContact);
//
b32 sphereTraceColliderPlaneSphereCollisionTest(ST_PlaneCollider* const pPlaneCollider, ST_SphereCollider* const pSphereCollider, ST_SphereContact* const contactInfo);
//
b32 sphereTraceColliderAABBImposedSphereCollisionTest(ST_AABB* const paabb, ST_Vector3 imposedPosition, float imposedRadius, ST_SphereContact* const contactInfo);
//
b32 sphereTraceColliderAABBSphereCollisionTest(ST_Collider* const pCollider, ST_SphereCollider* const pSphereCollider, ST_SphereContact* const contactInfo);
//
b32 sphereTraceColliderTriangleSphereCollisionTest(ST_TriangleCollider* const pTriangleCollider, ST_SphereCollider* const pSphereCollider, ST_SphereContact* const contactInfo);
//
b32 sphereTraceColliderPlaneImposedSphereCollisionTest(ST_PlaneCollider* const pPlaneCollider, ST_Vector3 imposedPosition, float imposedRadius, ST_SphereContact* const contactInfo);
//
b32 sphereTraceColliderTriangleImposedSphereCollisionTest(ST_TriangleCollider* const pTriangleCollider, ST_Vector3 imposedPosition, float imposedRadius, ST_SphereContact* const contactInfo);
//
b32 sphereTraceColliderSphereSphereCollisionTest(ST_SphereCollider* const pSphereColliderA, ST_SphereCollider* const pSphereColliderB, ST_SphereContact* const pContactInfo);
//
b32 sphereTraceColliderSphereSphereTraceOut(ST_Vector3 spherePos, float sphereRadius, ST_Direction clipoutDir, ST_Vector3 encompassingPos, float encompassingRadius, ST_SphereContact* const pContactInfo);
//
ST_SphereCubeCluster sphereTraceColliderSphereCubeClusterConstruct(float width);

ST_SpherePair sphereTraceColliderSpherePairConstruct(float radii, float halfDistance);

void sphereTraceColliderSpherePairGetSpherePositions(const ST_SpherePair* const pSpherePair, ST_Vector3* pLeftSphere, ST_Vector3* pRightSphere);

void sphereTraceColliderSpherePairSetPosition(ST_SpherePair* const pSpherePair, ST_Vector3 position);

void sphereTraceColliderSpherePairSetRotation(ST_SpherePair* const pSpherePair, ST_Quaternion rotation);

void sphereTraceColliderSpherePairRotate(ST_SpherePair* const pSpherePair, ST_Quaternion rotation);

void sphereTraceColliderSpherePairSetAABB(ST_SpherePair* const pSpherePair);