#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#pragma once

#include "SphereTraceMath.h"
#include "SphereTraceLists.h"
#include "SphereTraceMaterial.h"
#include "SphereTraceRigidBody.h"
#include "SphereTraceAllocator.h"
#include "SphereTraceCollider.h"
#include "SphereTraceColliderPlane.h"
#include "SphereTraceColliderTerrain.h"
#include "SphereTraceColliderSphere.h"
#include "SphereTraceColliderBox.h"
#include "SphereTraceColliderTriangle.h"
#include "SphereTraceSpacialPartition.h"
#include "SphereTraceAI.h"


#define ST_BOUNCE_FORCE_MAX 5000.0f
#define ST_VELOCITY_THRESHOLD 0.5f
#define ST_AUTO_DT_FACTOR 1.0/10.0f
#define ST_SKIN_WIDTH 0.005f
#define ST_RESTING_SPEED_SQUARED 0.05f
#define ST_SPHERE_RESTING_SLOPE 0.005f
#define ST_SLIP_THRESHOLD 0.5f;
#define ST_FRICTION_MODIFIER 1.0f/5.0f
#define ST_KINETIC_FRICTION_MODIFIER 1/2.0f
#define ST_RESTING_CONTACT_LERP_FACTOR 5.0f


typedef struct ST_SimulationSpace
{
	ST_IndexList sphereColliders;
	ST_IndexList boxColliders;
	ST_IndexList planeColliders;
	ST_IndexList triangleColliders;
	ST_IndexList uniformTerrainColliders;
	ST_IndexList spherePairColliders;
	ST_IndexList aabbColliders;
	ST_IndexList callbackColliders;
	//ST_IndexList translatedStaticColliders;
	ST_Vector3 gravitationalAcceleration;
	float minDeltaTime;
	ST_Material defaultMaterial;
	ST_AABB worldAABB;
	ST_OctTreeGrid octTreeGrid;
} ST_SimulationSpace;


//the penetration restriction will start out as none, as the first contact
//will be resolved simply by pushing out of the normal of the contact.
//this will then turn the pentration restriction to be within the plane 
//of the normal.  The second contact normal is then projected onto the plane
//to calculated the direction of the next penetation push out.
typedef enum PenetrationRestrictionType
{
	ST_PENETRATION_RESTRICTION_NONE = 0,
	ST_PENETRATION_RESTRICTION_PLANE = 1,
	ST_PENETRATION_RESTRICTION_DIRECTION = 2
} PenetrationRestrictionType;

typedef struct PenetrationRestriction
{
	//direction to hold none, plane normal, or out dir
	ST_Direction planeNormal;
	ST_Vector3 positionOnNormal;
	ST_Direction tangent;
	ST_Direction restrictedDir;
	PenetrationRestrictionType penetrationRestrictionType;
	//once the restriction direction is set, it cannot be undone
	b32 restrictionDirectionSet;
} PenetrationRestriction;

PenetrationRestriction sphereTracePenetrationRestrictionConstruct();

void sphereTraceSubscriberListAddOnCollisionEnterCallback(ST_SimulationSpace* pSimSpace, ST_Collider* pCollider,
	void (*callback)(const ST_SphereContact* const contact, ST_Collider* pOtherCollider, void* pContext), void* pContext);

void sphereTraceSubscriberListAddOnCollisionStayCallback(ST_SimulationSpace* pSimSpace, ST_Collider* pCollider, 
	void (*callback)(const ST_SphereContact* const contact, ST_Collider* pOtherCollider, void* pContext), void* pContext);

void sphereTraceSubscriberListAddOnCollisionExitCallback(ST_SimulationSpace* pSimSpace, ST_Collider* pCollider, 
	void (*callback)(const ST_SphereContact* const contact, ST_Collider* pOtherCollider, void* pContext), void* pContext);

ST_SimulationSpace sphereTraceSimulationConstruct();

ST_SimulationSpace sphereTraceSimulationConstruct1(ST_AABB worldaabb, ST_Vector3 gridHalfExtents);

void sphereTraceSimulationFree(ST_SimulationSpace* const pSimulationSpace);

void sphereTraceSimulationInsertCollider(ST_SimulationSpace* const pSimulationSpace, ST_Collider* const pCollider, b32 restructureTree);

void sphereTraceSimulationInsertPlaneCollider(ST_SimulationSpace* const pSimulationSpace, ST_PlaneCollider* const pPlaneCollider);

void sphereTraceSimulationInsertTriangleCollider(ST_SimulationSpace* const pSimulationSpace, ST_TriangleCollider* const pTriangleCollider);

void sphereTraceSimulationInsertSphereCollider(ST_SimulationSpace* const pSimulationSpace, ST_SphereCollider* const pSphereCollider);

void sphereTraceSimulationInsertBoxCollider(ST_SimulationSpace* const pSimulationSpace, ST_BoxCollider* const pBoxCollider);

void sphereTraceSimulationInsertUniformTerrainCollider(ST_SimulationSpace* const pSimulationSpace, ST_UniformTerrainCollider* const pTerrainCollider);

void sphereTraceSimulationInsertSpherePairCollider(ST_SimulationSpace* const pSimulationSpace, ST_SpherePair* const pSpherePair);

void sphereTraceSimulationInsertAABBCollider(ST_SimulationSpace* const pSimulationSpace, ST_Collider* const pCollider);

void sphereTraceSimulationRemoveCollider(ST_SimulationSpace* const pSimulationSpace, ST_Collider* const pCollider, b32 restructureTree);

void sphereTraceSimulationSpaceTranslateStaticCollider(ST_SimulationSpace* const pSimulationSpace, ST_Collider* const pStaticCollider, ST_Vector3 translation, b32 restructureTree);

void sphereTraceSimulationSpaceMoveStaticCollider(ST_SimulationSpace* const pSimulationSpace, ST_Collider* const pStaticCollider, ST_Vector3 newPosition, b32 restructureTree);

//void sphereTraceSimulationUpdateSphereColliderBucketIndices(ST_SimulationSpace* const pSimulationSpace, ST_SphereCollider* const pSphereCollider);

void sphereTraceSimulationApplyForcesAndTorques(const ST_SimulationSpace* const pSimulationSpace, ST_RigidBody* const pRigidBody, float dt, b32 incrementGravity);

void sphereTraceSimulationStepQuantity(const ST_SimulationSpace* const pSimulationSpace, ST_RigidBody* const pRigidBody, float dt);

ST_Vector3 sphereTraceSimulationImposedStepPosition(const ST_SimulationSpace* const pSimulationSpace, ST_RigidBody* const pRigidBody, float dt);

void sphereTraceSimulationSphereMultipleContactResponse(const ST_SimulationSpace* const pSimulationSpace, ST_SphereCollider* pSphereCollider, float dt);

void sphereTraceSimulationResolvePenetration(ST_SphereCollider* pSphereCollider, ST_SphereContact* pContact, PenetrationRestriction* pPenetrationRestriction);

void sphereTraceSimulationStepQuantities(const ST_SimulationSpace* const pSimulationSpace, float dt);

void sphereTraceSimulationSpherePlaneResponse(const ST_SimulationSpace* const pSimulationSpace, const ST_SphereContact* const pContactInfo, float dt);

void sphereTraceSimulationSphereContactResponse(const ST_SimulationSpace* const pSimulationSpace, const ST_SphereContact* const pContactInfo, float dt);

void sphereTraceSimulationSphereTriangleResponse(const ST_SimulationSpace* const pSimulationSpace, const ST_SphereContact* const pContactInfo, float dt);

void sphereTraceSimulationSphereTerrainTriangleResponse(const ST_SimulationSpace* const pSimulationSpace, const ST_SphereContact* const pContactInfo, float dt);

void sphereTraceSimulationSphereSphereResponse(const ST_SimulationSpace* const pSimulationSpace, const ST_SphereContact* const pContactInfo, float dt);

void sphereTraceSimulationExecuteCallbacksOnCollider(ST_Collider* const pCollider);

void sphereTraceSimulationOctTreeGridSolveDiscrete(ST_SimulationSpace* const pSimulationSpace, float dt);

b32 sphereTraceSimulationRayTrace(ST_SimulationSpace* const pSimulationSpace, ST_Vector3 start, ST_Direction dir, float maxDist, ST_RayTraceData* const pData);


