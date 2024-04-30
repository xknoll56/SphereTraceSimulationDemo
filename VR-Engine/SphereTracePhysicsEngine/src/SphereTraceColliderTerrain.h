#pragma once
#include "SphereTraceCollider.h"


ST_UniformTerrainCollider sphereTraceColliderUniformTerrainConstruct(int xCells, int zCells, float cellSize);

void sphereTraceColliderUniformTerrainFree(ST_UniformTerrainCollider* const pTerrainCollider);

void sphereTraceColliderUniformTerrainSetTransform(ST_UniformTerrainCollider* const pTerrainCollider, float angle, ST_Vector3 position);

void sphereTraceColliderUniformTerrainFillTrianglesWithFunction(ST_UniformTerrainCollider* const terrainCollider, float (*fxz)(float, float));

void sphereTraceColliderUniformTerrainFillTrianglesWithFunctionAndConditionalFunction(ST_UniformTerrainCollider* const terrainCollider, float (*fxz)(float, float, ST_UniformTerrainCollider* const terrainContext), b32(*conditionalFunc)(float (*fxz)(float, float), float, float, ST_UniformTerrainCollider* const terrainContext));

int sphereTraceColliderUniformTerrainSampleFirstTriangleIndex(const ST_UniformTerrainCollider* const terrainCollider, ST_Vector3 samplePosition);

int sphereTraceColliderUniformTerrainSampleTriangleIndex(const ST_UniformTerrainCollider* const terrainCollider, ST_Vector3 samplePosition);

ST_IndexList sphereTraceColliderUniformTerrainSampleTriangleIndicesForSphere(const ST_UniformTerrainCollider* const terrainCollider, ST_Vector3 spherePosition, float radius);


b32 sphereTraceColliderUniformTerrainImposedSphereFindMaxPenetratingTriangle(const ST_UniformTerrainCollider* const pTerrainCollider, ST_Vector3 imposedPosition, float imposedRadius, ST_SphereContact* const pContactInfo);

b32 sphereTraceColliderUniformTerrainSphereFindMaxPenetratingTriangle(const ST_UniformTerrainCollider* const pTerrainCollider, ST_SphereCollider* const pSphereCollider, ST_SphereContact* const pContactInfo);




b32 sphereTraceColliderUniformTerrainRayTrace(const ST_UniformTerrainCollider* const pTerrainCollider, ST_Vector3 from, ST_Direction dir, ST_RayTraceData* const pRayTraceData);

b32 sphereTraceColliderUniformTerrainSphereTraceDown(const ST_UniformTerrainCollider* const pTerrainCollider, ST_Vector3 from, float radius, ST_SphereTraceData* const pSphereTraceData);

ST_IndexList sphereTraceColliderUniformTerrainSampleTrianglesIndicesForSphereTrace(const ST_UniformTerrainCollider* const terrainCollider, ST_SphereTraceData* const pSphereTraceData);

b32 sphereTraceColliderUniformTerrainSphereTraceByStartEndPoint(const ST_UniformTerrainCollider* const pTerrainCollider, ST_Vector3 startPoint, ST_Vector3 endPoint, float radius, ST_SphereTraceData* const pSphereTraceData, ST_TriangleCollider** ppTriangleCollider);


b32 sphereTraceColliderUniformTerrainSphereTrace(const ST_UniformTerrainCollider* const pTerrainCollider, ST_Vector3 from, ST_Direction dir, float radius, ST_SphereTraceData* const pSphereTraceData);

ST_UniformTerrainSpherePrecomputedSampler sphereTraceColliderUniformTerrainSpherePrecomputedSamplerConstruct(ST_UniformTerrainCollider* pTerrainCollider, float sphereRadius, ST_Index subIncrements);

b32 sphereTraceColliderUniformTerrainSpherePrecomputedSamplerSphereCollisionTest(ST_UniformTerrainSpherePrecomputedSampler* const pPrecompSampler, ST_SphereCollider* const pSphereCollider, ST_SphereContact* const pContact);

void sphereTraceColliderUniformTerrainSpherePrecomputedSamplerWriteToFile(ST_UniformTerrainSpherePrecomputedSampler* const pPrecompSampler, const char* path);