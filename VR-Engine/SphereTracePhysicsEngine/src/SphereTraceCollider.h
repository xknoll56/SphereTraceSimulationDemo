#pragma once
#include "SphereTraceMath.h"
#include "SphereTraceLists.h"
#include "SphereTraceRigidBody.h"
#include "SphereTraceTag.h"


#define ST_COLLIDER_TOLERANCE 1e-5
#define ST_COLLIDER_SKIN 0.05f
#define ST_SPHERE_RESTING_CONTACTS_COUNT 4
#define ST_CONTACT_MAX 6

typedef struct ST_Frame
{
	ST_Direction right;
	ST_Direction up;
	ST_Direction forward;
}ST_Frame;

typedef enum ST_ColliderType
{
	COLLIDER_SPHERE = 0,
	COLLIDER_PLANE = 1,
	COLLIDER_TERRAIN = 2,
	COLLIDER_TRIANGLE = 3,
	COLLIDER_BOWL = 4,
	COLLIDER_PIPE = 5,
	COLLIDER_AABB = 6,
	COLLIDER_BOX = 7,
	COLLIDER_CUBE_CLUSTER = 8,
	COLLIDER_SPHERE_PAIR = 9
} ST_ColliderType;

const char* ST_ColliderStrings[];

//for simplicity dynamic spheres will be considered
//a face.
typedef enum ST_CollisionType
{
	ST_COLLISION_FACE = 0,
	ST_COLLISION_EDGE = 1,
	ST_COLLISION_POINT = 2,
	ST_COLLISION_INWARD_SPHEREICAL = 3,
	ST_COLLISION_OUTWARD_SPHEREICAL = 4,
	ST_COLLISION_INWARD_CIRCULAR = 5,
	ST_COLLISION_OUTWARD_CIRCULAR = 6
} ST_CollisionType;

typedef struct ST_SphereCollider ST_SphereCollider;

typedef struct ST_BoxCollider ST_BoxCollider;

typedef struct ST_Collider ST_Collider;

typedef struct ST_ContactPoint
{
	float penetrationDistance;
	ST_Direction normal;
	ST_Vector3 point;
	enum ST_CollisionType collisionType;
} ST_ContactPoint;

typedef struct ST_SphereContact
{
	float penetrationDistance;
	ST_Direction normal;
	ST_Vector3 point;
	enum ST_CollisionType collisionType;
	ST_SphereCollider* pSphereCollider;
	enum ST_ColliderType otherColliderType;
	void* pOtherCollider;
	float radiusOfCurvature;
	ST_Direction bitangent;
} ST_SphereContact;

typedef enum ST_DirectionType ST_DirectionType;

typedef struct ST_BoxContact
{
	int numContacts;
	int maxPenetrationIndex;
	ST_ContactPoint contactPoints[8];
	ST_BoxCollider* pBoxCollider;
	ST_Collider* pOtherCollider;
	ST_DirectionType otherBoxPlaneDirection;
	ST_ContactPoint lerpPoints[3];
	int numLerpPoints;
} ST_BoxContact;

ST_BoxContact sphereTraceBoxContactInit();


typedef int ST_ColliderIndex;

typedef enum ST_Octant
{
	ST_LEFT_DOWN_BACK = 0,
	ST_RIGHT_DOWN_BACK = 1,
	ST_LEFT_DOWN_FORWARD = 2,
	ST_RIGHT_DOWN_FORWARD = 3,
	ST_LEFT_UP_BACK = 4,
	ST_RIGHT_UP_BACK = 5,
	ST_LEFT_UP_FORWARD = 6,
	ST_RIGHT_UP_FORWARD = 7,
	ST_OCTANT_NONE = 8
} ST_Octant;

typedef struct ST_AABB
{
	//The size of the aabb
	ST_Vector3 halfExtents;
	
	//Transformed vertices
	ST_Vector3 highExtent;
	ST_Vector3 lowExtent;
	
	//center
	ST_Vector3 center;
} ST_AABB;

typedef struct ST_Edge
{
	ST_Vector3 point1;
	ST_Vector3 point2;
	ST_Direction dir;
	float dist;
} ST_Edge;

typedef struct ST_DynamicEdge
{
	ST_Vector3* pPoint1;
	ST_Vector3* pPoint2;
	ST_Direction* pDir;
	float dist;
} ST_DynamicEdge;

typedef struct ST_Ring
{
	ST_Vector3 centroid;
	ST_Direction normal;
	ST_Direction right;
	ST_Direction forward;
	float radius;
} ST_Ring;

typedef struct ST_SubscriberList
{
	b32 hasSubscriber;
	ST_IndexList curFrameContactEntries;
	ST_IndexList contactEntries;
	ST_IndexList onCollisionEnterCallbacks;
	ST_IndexList onCollisionStayCallbacks;
	ST_IndexList onCollisionExitCallbacks;
	void* pSubscriberContext;
} ST_SubscriberList;

//typedef struct ST_SpaceObjectEntry
//{
//	//pointer to the object (for the engine internals will
//	//simply be a collider)
//	ST_Index pObject;
//	//pointer to the aabb corresponding to this object
//	ST_AABB* paabb;
//	//pointer to the objects index list leaf nodes (will be used
//	//when objects are moved
//	ST_IndexList leafNodes;
//	b32 objectIsCollider;
//} ST_SpaceObjectEntry;

typedef struct ST_Collider
{
	ST_ColliderType colliderType;
	//bucket indices for the grid spacial partition
	ST_IndexList bucketIndices;
	//ST_IndexList octTreeLeafs;
	ST_IndexList* pLeafBucketLists;
	ST_Index colliderIndex;
	ST_SubscriberList subscriberList;
	float boundingRadius;
	ST_AABB aabb;
	ST_Tag tag;
	b32 isDynamic;
	ST_Index pWhatever;
	//if the colliders aabb is moved, the collider
	//needs to be reinserted into the tree grid
	b32 reInsertCollider;
} ST_Collider;

//void sphereTraceColliderSetOctTreeEntry(ST_Collider* const pCollider);

typedef struct ST_AABBContact
{
	ST_Collider* pCollider;
	ST_Collider* pOtherCollider;
	ST_AABB intersectionRegion;
} ST_AABBContact;

typedef struct ST_CallbackFunction
{
	void (*callback)(const ST_SphereContact* const contact, ST_Collider* pOtherCollider, void* pContext);
}ST_CallbackFunction;

typedef struct ST_SphereContactEntry
{
	ST_Collider* pOtherCollider;
	ST_SphereContact contact;
} ST_SphereContactEntry;



ST_SubscriberList sphereTraceSubscriberListConstruct();

ST_Collider sphereTraceColliderConstruct(ST_ColliderType colliderType, float boundingRadius);

ST_Collider sphereTraceColliderAABBConstruct(ST_AABB aabb);

void sphereTraceColliderFree(ST_Collider* const pCollider);

typedef enum ST_DirectionType
{
	ST_DIRECTION_RIGHT = 0,
	ST_DIRECTION_LEFT = 1,
	ST_DIRECTION_UP = 2,
	ST_DIRECTION_DOWN = 3,
	ST_DIRECTION_FORWARD = 4,
	ST_DIRECTION_BACK = 5,
	ST_DIRECTION_COMBINATION = 6,
	ST_DIRECTION_NONE = 7,

} ST_DirectionType;

typedef struct ST_PlaneCollider
{
	ST_Collider collider;
	//ST_ColliderType colliderType; 
	ST_Direction normal;
	ST_Direction right;
	ST_Direction forward;
	float xHalfExtent;
	float zHalfExtent;
	ST_Vector3 position;
	ST_Quaternion rotation;
	ST_Vector3 transformedVertices[4];
	ST_Edge transformedEdges[4];
	//ST_IndexList bucketIndices;
	//ST_Index planeColliderIndex;
} ST_PlaneCollider;

typedef struct ST_TriangleCollider
{
	ST_Collider collider;
	//ST_ColliderType colliderType;
	ST_Vector3 transformedVertices[3];
	ST_Edge transformedEdges[3];
	ST_Direction vertexDirs[3];
	ST_Direction edgeOrthogonalDirs[3];
	float vertexDists[3];
	ST_Direction normal;
	ST_Vector3 centroid;

	//members needed for terrain optimizations
	ST_Index index;
	ST_Vector2Integer terrainCoords;
	ST_Index lowestVertIndex;
	ST_Index highestVertIndex;
	float circularRadius;

	b32 ignoreCollisions;
} ST_TriangleCollider;

typedef struct ST_SphereCollider
{
	ST_Collider collider;
	b32 restingContact;
	float radius;
	ST_RigidBody rigidBody;
	b32 ignoreCollisions;
	ST_IndexList prevFrameContacts;
} ST_SphereCollider;


typedef struct ST_SphereCubeCluster
{
	ST_Collider collider;
	ST_Vector3 elementPositions[8];
	ST_RigidBody rigidBody;
	float halfWidth;
} ST_SphereCubeCluster;


typedef struct ST_SpherePair
{
	ST_Collider collider;
	ST_RigidBody rigidBody;
	float radii;
	float halfDistance;
	b32 restingContact;
	ST_Frame frame;
} ST_SpherePair;

typedef struct ST_BoxFace
{
	enum ST_DirectionType dir;
	ST_Octant vertexIndices[4];
} ST_BoxFace;


typedef struct ST_BoxEdgeConnection
{
	ST_DirectionType edgeDirs[3];
	ST_Octant connectingVertices[3];
} ST_BoxEdgeConnection;


typedef struct ST_BoxCollider
{
	ST_Collider collider;
	ST_RigidBody rigidBody;
	b32 restingContact;
	ST_Vector3 halfExtents;
	ST_Direction localRight;
	ST_Direction localUp;
	ST_Direction localForward;
	ST_Vector3 transformedVertices[8];
	b32 ignoreCollisions;
	float minDimension;
} ST_BoxCollider;

typedef struct ST_BowlCollider
{
	ST_Collider collider;
	ST_Vector3 position;
	float radius;
	ST_Direction normal;
} ST_BowlCollider;

typedef struct ST_PipeCollider
{
	ST_Collider collider;
	ST_Vector3 position;
	float radius;
	float length;
	ST_Direction up;
	ST_Direction right;
	ST_Direction forward;
} ST_PipeCollider;

typedef enum ST_PlaneEdgeDirection
{
	PLANE_EDGE_RIGHT = 0,
	PLANE_EDGE_FORWARD = 1,
	PLANE_EDGE_LEFT = 2,
	PLANE_EDGE_BACK = 3
} ST_PlaneEdgeDirection;

typedef enum ST_PlaneVertexDirection
{
	PLANE_VEREX_FORWARD_RIGHT = 0,
	PLANE_VEREX_FORWARD_LEFT = 1,
	PLANE_VEREX_BACK_LEFT = 2,
	PLANE_VEREX_BACK_RIGHT = 3
} ST_PlaneVertexDirection;

typedef struct ST_ImposedPlane
{
	ST_Vector3 position;
	ST_Direction right;
	ST_Direction normal;
	ST_Direction forward;
	float xHalfExtent;
	float zHalfExtent;
	ST_Edge edges[4];
} ST_ImposedPlane;


ST_DirectionType sphereTraceDirectionTypeGetReverse(ST_DirectionType dirType);
ST_DirectionType sphereTraceDirectionTypeCross(ST_DirectionType dir1, ST_DirectionType dir2);

typedef struct ST_RayTraceData
{
	ST_ContactPoint contact;
	void* pOtherCollider;
	ST_Vector3 startPoint;
	float distance;
	ST_DirectionType directionType;
} ST_RayTraceData;

typedef struct ST_EdgeTraceData
{
	ST_ContactPoint contact1;
	ST_ContactPoint contact2;
	b32 usesBothContacts;
	ST_Index hitIndex;
	ST_Collider* pOtherCollider;
	float distance;
	ST_DirectionType directionType;
} ST_EdgeTraceData;

//ST_EdgeTraceData sphereTraceEdgeTraceDataConstruct();
ST_EdgeTraceData sphereTraceEdgeTraceDataFrom1RayTraceData(const ST_RayTraceData* const prtd);
ST_EdgeTraceData sphereTraceEdgeTraceDataFrom2RayTraceData(const ST_RayTraceData* const prtd, const ST_RayTraceData* const prtd1);


ST_ContactPoint sphereTraceContactPointFromSphereContact(const ST_SphereContact* const psc);

typedef struct ST_BoxTraceData
{
	int numContacts;
	ST_ContactPoint contacts[8];
	ST_Collider* pOtherCollider;
	ST_Vector3 boxCenter;
	float traceDistance;
} ST_BoxTraceData;


typedef struct ST_SphereTraceData
{
	ST_RayTraceData rayTraceData;
	ST_Vector3 sphereCenter;
	float radius;
	//the trace distance is the distance from the start sphere to the end sphere
	float traceDistance;
} ST_SphereTraceData;


//typedef struct ST_SpherePlaneContactInfo
//{
//	float penetrationDistance;
//	ST_Direction normal;
//	ST_Vector3 point;
//	ST_SphereCollider* pSphereCollider;
//	ST_PlaneCollider* pPlaneCollider;
//	ST_CollisionType collisionType;
//} ST_SpherePlaneContactInfo;

//typedef struct ST_SphereTriangleContactInfo
//{
//	float penetrationDistance;
//	ST_Direction normal;
//	ST_Vector3 point;
//	ST_SphereCollider* pSphereCollider;
//	ST_TriangleCollider* pTriangleCollider;
//	ST_CollisionType collisionType;
//} ST_SphereTriangleContactInfo;
//
//typedef struct ST_SphereTerrainTriangleContactInfo
//{
//	ST_SphereTriangleContactInfo sphereTriangleContactInfo;
//	ST_SphereTraceData downSphereTraceData;
//} ST_SphereTerrainTriangleContactInfo;
//
//typedef struct ST_SphereSphereContactInfo
//{
//	float penetrationDistance;
//	ST_Direction normal;
//	ST_Vector3 point;
//	ST_SphereCollider* pA;
//	ST_SphereCollider* pB;
//} ST_SphereSphereContactInfo;
ST_Frame sphereTraceFrameConstruct();

void sphereTraceFrameUpdateWithRotationMatrix(ST_Frame* pFrame, ST_Matrix4 rotationMatrix);

ST_Edge sphereTraceEdgeConstruct(ST_Vector3 p1, ST_Vector3 p2);

ST_Edge sphereTraceEdgeConstruct1(ST_Vector3 p1, ST_Vector3 p2, float dist, ST_Direction dir);

ST_DynamicEdge sphereTraceDynamicEdgeConstruct(ST_Vector3* pp1, ST_Vector3* pp2, float dist, ST_Direction* pdir);

ST_Edge sphereTraceEdgeFromDynamicEdge(const ST_DynamicEdge* const pde);

void sphereTraceEdgeTranslate(ST_Edge* pEdge, ST_Vector3 translation);

ST_Ring sphereTraceRingConstruct(ST_Vector3 centroid, ST_Direction normal, float radius);

ST_AABB sphereTraceAABBConstruct1(ST_Vector3 lowExtent, ST_Vector3 highExtent);

ST_AABB sphereTraceAABBConstruct2(ST_Vector3 position, ST_Vector3 halfExtents);

void sphereTraceAABBTranslate(ST_AABB* paabb, ST_Vector3 translation);

b32 sphereTraceAABBAssert(const ST_AABB* const paabb);

float sphereTraceAABBGetVolume(const ST_AABB* const paabb);

float sphereTraceAABBGetSizeMetric(const ST_AABB* const paabb);

void sphereTraceAABBSetCenterAndHalfExtents(ST_AABB* paabb);

void sphereTraceAABBSetHighAndLowExtents(ST_AABB* paabb);

float sphereTraceAABBGetBoundingRadius(ST_AABB* paabb);

float sphereTraceAABBGetBoundingRadiusSquared(ST_AABB* paabb);

b32 sphereTraceColliderAABBContainsPoint(const ST_AABB* const aabb, ST_Vector3 point);

void sphereTraceColliderAABBSetHalfExtents(ST_AABB* const aabb);

void sphereTraceColliderAABBResizeAABBToContainAnotherAABB(ST_AABB* const aabbToResize, const ST_AABB* const aabbToContain);

void sphereTraceColliderAABBResizeAABBToContainPoint(ST_AABB* const aabbToResize, const ST_Vector3 point);

b32 sphereTraceColliderAABBIntersectAABB(const ST_AABB* const aabb1, const ST_AABB* const aabb2);

b32 sphereTraceColliderAABBIntersectAABBIntersectionRegion(const ST_AABB* const aabb1, const ST_AABB* const aabb2, ST_AABB* const intersectionRegion);

b32 sphereTraceColliderAABBIntersectAABBHorizontally(const ST_AABB* const aabb1, const ST_AABB* const aabb2);

b32 sphereTraceColliderAABBIntersectAABBVertically(const ST_AABB* const aabb1, const ST_AABB* const aabb2);

b32 sphereTraceColliderEmptyAABBRayTrace(ST_Vector3 from, ST_Direction dir, const ST_AABB* const paabb, ST_RayTraceData* const pRaycastData);
b32 sphereTraceColliderAABBRayTrace(ST_Vector3 from, ST_Direction dir, const ST_Collider* const pCollider, ST_RayTraceData* const pRaycastData);

//b32 sphereTraceColliderAABBRayTraceThrough(ST_Vector3 contactStart, ST_DirectionType contactDirection, ST_Direction dir, const ST_AABB* const paabb, ST_RayTraceData* const pRaycastData);

b32 sphereTraceColliderAABBRayTraceThrough(ST_Vector3 pointWithin, ST_Direction dir, const ST_AABB* const paabb, ST_RayTraceData* const pRaycastData);

b32 sphereTraceColliderEmptyAABBSphereTrace(ST_Vector3 from, ST_Direction dir, float radius, const ST_AABB* const paabb, ST_SphereTraceData* const pRaycastData);
b32 sphereTraceColliderAABBSphereTrace(ST_Vector3 from, ST_Direction dir, float radius, const ST_Collider* const pCollider, ST_SphereTraceData* const pRaycastData);
b32 sphereTraceColliderAABBSphereTraceOut(ST_Vector3 spherePos, float sphereRadius, ST_Direction clipoutDir, ST_Collider* const pCollider, ST_SphereTraceData* const pSphereCastData);

b32 sphereTraceColliderAABBFrontFaceSphereTrace(ST_Vector3 from, ST_Direction dir, float radius, const ST_AABB* const paabb, ST_SphereTraceData* const pData);
b32 sphereTraceColliderAABBBackFaceSphereTrace(ST_Vector3 from, ST_Direction dir, float radius, const ST_AABB* const paabb, ST_SphereTraceData* const pData);
b32 sphereTraceColliderAABBRightFaceSphereTrace(ST_Vector3 from, ST_Direction dir, float radius, const ST_AABB* const paabb, ST_SphereTraceData* const pData);
b32 sphereTraceColliderAABBLeftFaceSphereTrace(ST_Vector3 from, ST_Direction dir, float radius, const ST_AABB* const paabb, ST_SphereTraceData* const pData);
b32 sphereTraceColliderAABBUpFaceSphereTrace(ST_Vector3 from, ST_Direction dir, float radius, const ST_AABB* const paabb, ST_SphereTraceData* const pData);
b32 sphereTraceColliderAABBDownFaceSphereTrace(ST_Vector3 from, ST_Direction dir, float radius, const ST_AABB* const paabb, ST_SphereTraceData* const pData);

ST_Octant sphereTraceOctantGetNextFromDirection(ST_Octant curOctant, ST_DirectionType dir);

b32 sphereTraceOctantIsOnRightSide(ST_Octant curOctant);

b32 sphereTraceOctantIsOnUpSide(ST_Octant curOctant);

b32 sphereTraceOctantIsOnForwardSide(ST_Octant curOctant);

ST_Vector3 sphereTraceColliderAABBMidPoint(const ST_AABB* const aabb);

ST_Vector3 sphereTraceColliderAABBGetExtentByOctant(const ST_AABB* const paabb, ST_Octant octant);
//ST_Vector3 sphereTraceColliderAABBGetRightBottomBackExtent(const ST_AABB* const paabb);
//
//ST_Vector3 sphereTraceColliderAABBGetLeftTopBackExtent(const ST_AABB* const paabb);
//
//ST_Vector3 sphereTraceColliderAABBGetLeftBottomForwardExtent(const ST_AABB* const paabb);
//
//ST_Vector3 sphereTraceColliderAABBGetRightTopBackExtent(const ST_AABB* const paabb);
//
//ST_Vector3 sphereTraceColliderAABBGetRightBottomForwardExtent(const ST_AABB* const paabb);
//
//ST_Vector3 sphereTraceColliderAABBGetLeftTopForwardExtent(const ST_AABB* const paabb);

void sphereTraceColliderResizeAABBWithSpherecast(const ST_SphereTraceData* const pSphereCastData, ST_AABB* const aabb);


b32 sphereTraceColliderPointSphereTrace(ST_Vector3 from, ST_Direction dir, float radius, ST_Vector3 point, ST_SphereTraceData* const pSphereTraceData);

b32 sphereTraceColliderEdgeSphereTrace(ST_Vector3 from, ST_Direction dir, float radius, ST_Edge* const pEdge, ST_SphereTraceData* const pSphereTraceData);
b32 sphereTraceColliderEdgeEdgeTrace(ST_Edge* const pEdgeTrace, ST_Direction dir, ST_Edge* const pEdge, ST_EdgeTraceData* const pEdgeTraceData);

b32 sphereTraceColliderRingSphereTrace(ST_Vector3 from, ST_Direction dir, float radius, ST_Ring* const pRing, ST_SphereTraceData* const pSphereTraceData);

ST_SphereCollider* sphereTraceColliderSphereGetFromContact(const ST_SphereContact* const pContact);
ST_PlaneCollider* sphereTraceColliderPlaneGetFromContact(const ST_SphereContact* const pContact);
ST_TriangleCollider* sphereTraceColliderTriangleGetFromContact(const ST_SphereContact* const pContact);

const char* sphereTraceColliderGetColliderString(const ST_Collider* const pCollider);

b32 sphereTraceColliderRayTrace(ST_Vector3 from, ST_Direction dir, const ST_Collider* const pCollider, ST_RayTraceData* const pRayTraceData);

b32 sphereTraceColliderSphereTrace(ST_Vector3 from, float radius, ST_Direction dir, const ST_Collider* const pCollider, ST_SphereTraceData* const pSphereTraceData);

b32 sphereTraceColliderListRayTrace(ST_Vector3 from, ST_Direction dir, ST_IndexList* const pColliderList, ST_RayTraceData* const pRayTraceData);

b32 sphereTraceColliderListSphereTrace(ST_Vector3 from, ST_Direction dir, float radius, ST_IndexList* const pColliderList, ST_SphereTraceData* const pSphereTraceData);
//ST_UniformTerrainCollider* sphereTraceColliderTerrainGetFromContact(const ST_SphereContact* const pContact);

typedef struct ST_UniformTerrainCollider
{
	ST_Collider collider;
	//ST_ColliderType colliderType;	
	ST_Index triangleCount;
	ST_TriangleCollider* triangles;
	int xCells;
	int zCells;
	float xSize;
	float zSize;
	float cellSize;
	float angle;
	ST_Vector3 right;
	ST_Vector3 forward;
	ST_Vector3 position;
	ST_Quaternion rotation;
	//ST_IndexList bucketIndices;
	ST_Vector3 halfExtents;

	//needed for quick raytrace
	//replace with obb when implemented
	ST_PlaneCollider rightPlane;
	ST_PlaneCollider topPlane;
	ST_PlaneCollider forwardPlane;
	ST_PlaneCollider leftPlane;
	ST_PlaneCollider bottomPlane;
	ST_PlaneCollider backPlane;
	//ST_Index uniformTerrainColliderIndex;
} ST_UniformTerrainCollider;

typedef struct ST_UniformTerrainSpherePrecomputedSampler
{
	ST_UniformTerrainCollider* pUniformTerrainCollider;
	float sphereRadius;
	int** sampledIndexes;
	ST_Index subIncrements;
	float subCellSize;

}ST_UniformTerrainSpherePrecomputedSampler;

#include "SphereTraceColliderPlane.h"
#include "SphereTraceColliderSphere.h"
#include "SphereTraceColliderTerrain.h"
#include "SphereTraceColliderTriangle.h"