#pragma once

#include "SphereTraceMath.h"
#include "SphereTraceLists.h"
#include "SphereTraceCollider.h"

#define SPACIAL_PARTITION_STATIC_DIMENSION 10
#define SPACIAL_PARTITION_STATIC_SIZE SPACIAL_PARTITION_STATIC_DIMENSION*SPACIAL_PARTITION_STATIC_DIMENSION*SPACIAL_PARTITION_STATIC_DIMENSION
#define ST_OCT_TREE_POINT_RADIUS_SQUARED 1.0f
#define ST_OCT_TREE_SMALLEST_OCTANT_SIZE 1.0f
#define ST_OCT_TREE_SKIN_WIDTH 1.0e-6f
#define ST_OCT_TREE_MAX_DEPTH 7


typedef struct ST_OctTreeNode
{
	ST_AABB aabb;
	b32 hasChildren;
	struct ST_OctTreeNode* children[8];
	struct ST_OctTreeNode* pParant;
	ST_IndexList colliderEntries;
	ST_Index depth;
	float boundingRadius;
} ST_OctTreeNode;


typedef struct ST_OctTree
{
	ST_OctTreeNode* root;
	ST_Index depth;
	ST_Index maxDepth;
	ST_Index gridIndex;
} ST_OctTree;

ST_OctTreeNode sphereTraceOctTreeNodeConstruct(ST_AABB aabb, ST_Index depth, ST_OctTreeNode* pParent);
void sphereTraceOctTreeNodeFree(ST_OctTreeNode* const pNode);
void sphereTraceOctTreeNodeSetChildAABBByIndex(ST_OctTreeNode* const pNode, ST_Index i, ST_AABB* paabb);
void sphereTraceOctTreeNodePopulateChildren(ST_OctTreeNode* const pNode, ST_Index bucketIndex);
ST_Index sphereTraceOctTreeNodeGetColliderCountBelow(ST_OctTreeNode* const pNode, ST_IndexList* pUniqueCollider, ST_IndexList* pNodesBelow);
ST_OctTree sphereTraceOctTreeConstruct(ST_AABB aabb, ST_Index gridIndex);
ST_IndexList sphereTraceOctTreeSampleIntersectionLeafs(ST_OctTree* const pOctTree, ST_AABB* const paabb);
void sphereTraceOctTreeReSampleIntersectionLeafsAndColliders(ST_OctTree* const pOctTree, ST_AABB* const paabb,
	ST_IndexList* const pLeafs, ST_IndexList* const pColliders, b32 sampleDynamicColliders, b32 sampleStaticColliders);
void sphereTraceOctTreeSampleIntersectionLeafsAndCollidersFromPerspective(ST_OctTree* const pOctTree, ST_Vector3 from, ST_Direction dir, float fov, float f, ST_IndexList* const pLeafs, ST_IndexList* const pColliders);
ST_OctTreeNode* sphereTraceOctTreeSampleLeafNode(ST_OctTree* pTree, ST_Vector3 point);
void sphereTraceOctTreeInsertCollider(ST_OctTree* pTree, ST_Collider* pCollider, b32 restructureTree);
void sphereTraceOctTreeRemoveCollider(ST_OctTree* pTree, ST_Collider* pCollider, b32 restructureTree);
void sphereTraceOctTreeReInsertCollider(ST_OctTree* pTree, ST_Collider* pCollider, b32 restructureTree);
b32 sphereTraceOctTreeRayTrace(ST_Vector3 from, ST_Direction dir, float maxDist, const ST_OctTree* const pTree, ST_RayTraceData* const pRayTraceData);
b32 sphereTraceOctTreeRayTraceFromWithin(ST_Vector3 from, ST_Direction dir, float maxDist, const ST_OctTree* const pTree, ST_RayTraceData* const pRayTraceData);

//verification functions
b32 sphereTraceOctTreeVerifyNonLeafsHaveNoColliders(ST_OctTree* pTree);
b32 sphereTraceOctTreeVerifyColliderListHaveProperLeafs(ST_OctTree* const pTree, ST_Index treeIndex, ST_Collider* const pColliders);


typedef struct ST_OctTreeGrid
{
	ST_OctTree* treeBuckets;
	ST_IndexList outsideColliders;
	ST_AABB worldaabb;
	ST_Vector3 bucketHalfExtents;
	float minDim;
	ST_Index xBuckets;
	ST_Index yBuckets;
	ST_Index zBuckets;
	ST_Index capacity;
} ST_OctTreeGrid;

ST_OctTreeGrid sphereTraceOctTreeGridConstruct(ST_AABB worldaabb, ST_Vector3 bucketHalfExtents);

ST_Index sphereTraceOctTreeGridGetBucketIndexFromPosition(const ST_OctTreeGrid* const pOctTreeGrid, ST_Vector3 position);

ST_IndexList sphereTraceOctTreeGridGetBucketIndicesFromAABB(const ST_OctTreeGrid* const pOctTreeGrid, const ST_AABB* const aabb);

ST_IndexList sphereTraceOctTreeGridGetBucketIndicesFromAABBCorners(const ST_OctTreeGrid* const pOctTreeGrid, const ST_AABB* const aabb);

ST_IndexList sphereTraceOctTreeGridGetBucketIndicesFromRayTrace(ST_Vector3 start, ST_Direction dir, float maxDist, const ST_OctTreeGrid* const pGrid, ST_RayTraceData* const pData);

void sphereTraceOctTreeGridInsertCollider(ST_OctTreeGrid* const pGrid, ST_Collider* pCollider, b32 restructureTrees);

void sphereTraceOctTreeGridInsertDynamicCollider(ST_OctTreeGrid* const pGrid, ST_Collider* pCollider, b32 restructureTrees);

void sphereTraceOctTreeGridRemoveCollider(ST_OctTreeGrid* const pGrid, ST_Collider* pCollider, b32 restructureTrees);

void sphereTraceOctTreeGridReInsertCollider(ST_OctTreeGrid* const pGrid, ST_Collider* pCollider, b32 restructureTrees);

void sphereTraceOctTreeGridReSampleIntersectionLeafsAndColliders(ST_OctTreeGrid* const pGrid, ST_AABB* const paabb,
	ST_IndexList* const pLeafs, ST_IndexList* const pColliders, b32 sampleDynamicColliders, b32 sampleStaticColliders);

void sphereTraceOctTreeGridSampleIntersectionLeafsAndCollidersFromPerspective(ST_OctTreeGrid* const pGrid, ST_Vector3 from, 
	ST_Direction dir, float fov, float f, ST_IndexList* const pLeafs, ST_IndexList* const pColliders);

b32 sphereTraceOctTreeGridRayTrace(ST_Vector3 start, ST_Direction dir, float maxDist, const ST_OctTreeGrid* const pGrid, ST_RayTraceData* const pData);

b32 sphereTraceOctTreeGridRayTrace_(ST_Vector3 start, ST_Direction dir, float maxDist, const ST_OctTreeGrid* const pGrid, ST_RayTraceData* const pData);

ST_Index sphereTraceOctTreeGridGetLargestDepth(const ST_OctTreeGrid* const pGrid);

b32 sphereTraceOctTreeGridVerifyColliderListHaveProperLeafs(ST_OctTreeGrid* const pGrid, ST_Collider* const pColliders);