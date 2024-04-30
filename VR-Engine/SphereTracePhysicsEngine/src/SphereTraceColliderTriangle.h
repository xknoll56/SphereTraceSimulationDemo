#include "SphereTraceCollider.h"

//
ST_TriangleCollider sphereTraceColliderTriangleConstruct(ST_Vector3 v1, ST_Vector3 v2, ST_Vector3 v3);
//
int sphereTraceColliderTriangleGetClosestTransformedEdgeIndexToPoint(const ST_TriangleCollider* const pTriangleCollider, ST_Vector3 point);
//
int sphereTraceColliderTriangleGetClosestTransformedVertexIndexToPoint(const ST_TriangleCollider* const pTriangleCollider, ST_Vector3 point);
//
void sphereTraceColliderTriangleSetAABB(ST_TriangleCollider* const pTriangleCollider);
//
void sphereTraceColliderTriangleTranslate(ST_TriangleCollider* const pTriangleCollider, ST_Vector3 translation);
//
void sphereTraceColliderTriangleSetPosition(ST_TriangleCollider* const pTriangleCollider, ST_Vector3 position);
//
void sphereTraceColliderTriangleSetVertexAndEdgeData(ST_TriangleCollider* const pTriangleCollider, ST_Vector3 v1, ST_Vector3 v2, ST_Vector3 v3);
//
void sphereTraceColliderTriangleUpdateVertexData(ST_TriangleCollider* const pTriangleCollider, ST_Vector3 v1, ST_Vector3 v2, ST_Vector3 v3);
//
b32 sphereTraceColliderTriangleIsProjectedPointContained(ST_Vector3 projectedPoint, const ST_TriangleCollider* const pTriangleCollider);
//
b32 sphereTraceColliderTriangleRayTrace(ST_Vector3 from, ST_Direction dir, const ST_TriangleCollider* const pTriangleCollider, ST_RayTraceData* const pRaycastData);
//
b32 sphereTraceColliderTriangleSphereTrace(ST_Vector3 from, ST_Direction dir, float radius, ST_TriangleCollider* const pTriangleCollider, ST_SphereTraceData* const pSphereCastData);
//
b32 sphereTraceColliderTriangleSphereTraceOut(ST_Vector3 spherePos, float sphereRadius, ST_Direction clipoutDir, ST_TriangleCollider* const pTriangleCollider, ST_SphereTraceData* const pSphereCastData);