#pragma once
#include "SphereTraceCollider.h"


ST_BoxCollider sphereTraceColliderBoxConstruct(ST_Vector3 halfExtents);

void sphereTraceColliderBoxSetAABB(ST_BoxCollider* const pBoxCollider);

void sphereTraceColliderBoxUpdateLocalAxis(ST_BoxCollider* const pBoxCollider);

void sphereTraceColliderBoxUpdateTransformedVertices(ST_BoxCollider* const pBoxCollider);

void sphereTraceColliderBoxSetAABB(ST_BoxCollider* const pBoxCollider);

void sphereTraceColliderBoxUpdateTransform(ST_BoxCollider* const pBoxCollider);

ST_Direction sphereTraceColliderBoxFaceGetNormal(ST_BoxCollider* const pBoxCollider, ST_BoxFace* const pBoxFace);

ST_Index sphereTraceColliderBoxGetVertexIndexClosestToDirection(const ST_Direction* pRight, const ST_Direction* pUp, const ST_Direction* pForward, const ST_Direction dir);

ST_Vector3 sphereTraceColliderBoxGetUnitVertexClosestToDirection(const ST_BoxCollider* const pBoxCollider, const ST_Direction dir);

void sphereTraceColliderBoxSetPosition(ST_BoxCollider* const pBoxCollider, ST_Vector3 position);

//void sphereTraceColliderBoxSetEdges(ST_BoxCollider* const pBoxCollider);

b32 sphereTraceColliderPlaneBoxCollisionTest(ST_BoxCollider* const pBoxCollider, ST_PlaneCollider* const pPlaneCollider, ST_BoxContact* const pContact);

b32 sphereTraceColliderPlaneBoxTrace(ST_Vector3 start, ST_Vector3 halfExtents, ST_Quaternion rotation, ST_Direction dir, ST_PlaneCollider* const pPlaneCollider, ST_BoxTraceData* const pBoxTraceData);

b32 sphereTraceColliderPlaneBoxTrace1(ST_Vector3* const pStart, ST_Vector3* const pHalfExtents, ST_Direction* const pLocalRight, ST_Direction* const pLocalUp,
	ST_Direction* const pLocalForward, ST_Direction* const pDir, ST_PlaneCollider* const pPlaneCollider, ST_BoxTraceData* const pBoxTraceData);

b32 sphereTraceColliderBoxPointCollisionTest(const ST_Vector3 point, ST_BoxCollider* const pBoxCollider, ST_ContactPoint* const pContactPoint);

b32 sphereTraceColliderBoxRayTrace(ST_Vector3 start, ST_Direction dir, ST_BoxCollider* const pBoxCollider, ST_RayTraceData* const prtd);

b32 sphereTraceColliderBoxEdgeCollisionTest(ST_Edge* const pEdge, ST_BoxCollider* const pBoxCollider, ST_BoxContact* const pContact);

b32 sphereTraceColliderBoxRayTraceClosestEdge(ST_Vector3 start, ST_Direction dir, ST_Edge* pClosestEdge, ST_Direction* pInward, ST_Direction* pInward1, ST_BoxCollider* const pBoxCollider, ST_RayTraceData* const prtd);

b32 sphereTraceColliderEdgeEdgeCollisionTest(ST_Edge* const pEdge, ST_Edge* const pOtherEdge, ST_ContactPoint* const pContact);

void sphereTraceColliderBoxForceSolvePenetration(ST_BoxCollider* const pBoxCollider, ST_BoxContact* const pContact);

b32 sphereTraceColliderBoxPlaneEdgeCollisionTest(ST_Edge* const pEdge, ST_Direction dirRestriction, ST_BoxCollider* const pBoxCollider, ST_Direction normal, ST_BoxFace* const pClosestFaceToPlane, ST_BoxContact* const pContact);

ST_BoxFace sphereTraceColliderBoxGetFaceClosestToDirection(const ST_BoxCollider* const pBoxCollider, const ST_Direction dir);

b32 sphereTraceColliderBoxBoxCollisionTest(ST_BoxCollider* const pBoxColliderA, ST_BoxCollider* const pBoxColliderB, ST_BoxContact* const pContact);

b32 sphereTraceColliderPlaneBoxTraceOut1(ST_BoxCollider* const pBoxCollider, ST_Direction clipoutDir, ST_PlaneCollider* const pPlaneCollider, ST_BoxTraceData* const pBoxTraceData);

b32 sphereTraceColliderBoxBoxTraceOut1(ST_BoxCollider* const pBoxCollider, ST_Direction clipoutDir, ST_BoxCollider* const pOtherBox, ST_DirectionType faceDir, ST_BoxTraceData* const pBoxTraceData);

b32 sphereTraceColliderBoxBoxTraceOut2(ST_BoxCollider* const pBoxCollider, ST_Direction clipoutDir, ST_BoxCollider* const pOtherBox, ST_BoxTraceData* const pBoxTraceData);