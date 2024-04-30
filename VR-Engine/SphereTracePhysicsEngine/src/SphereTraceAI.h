#pragma once
#include "SphereTraceMath.h"
#include "SphereTraceLists.h"

typedef struct ST_StateConnection
{
	int stateIndex;
	int nextStateIndex;
	b32 (*conditionalFunction)(void* machineContext);
} ST_StateConnection;

typedef struct ST_State
{
	void (*onUpdate)(void* machineContext);
	void (*onEnter)(void* machineContext);
	void (*onExit)(void* machineContext);
	int stateIndex;
	int numConnections;
	ST_StateConnection* connections;
} ST_State;


typedef struct ST_StateMachine
{
	ST_State* states;
	int numStates;
	int currentStateIndex;
	int startStateIndex;
	void* machineContext;
} ST_StateMachine;

ST_State sphereTraceStateMachineStateConstruct(int stateIndex, int numConnections);

ST_State sphereTraceStateMachineStateConstructWithStateFunctions(int stateIndex, int numConnections, void (*onUpdate)(void* machineContext), void (*onEnter)(void* machineContext), void (*onExit)(void* machineContext), ST_StateConnection* connections);

void sphereTraceStateMachineStateSetConnection(ST_State* const pState, const ST_StateConnection* const pStateConnections);

ST_StateConnection sphereTraceStateMachineStateConnectionConstruct(int stateIndex, int nextStateIndex, b32(*conditionalFunction)(void* machineContext));

ST_StateConnection sphereTraceStateMachineStateConnectionConstructWithStates(const ST_State* pState, const ST_State* pNextState, b32(*conditionalFunction)(void* machineContext));

ST_StateMachine sphereTraceStateMachineConstruct(int numStates, void* machineContext);

void sphereTraceStateMachineInsertState(ST_StateMachine* const pStateMachine, const ST_State* const pState);

void sphereTraceStateMachineStart(ST_StateMachine* const pStateMachine);

void sphereTraceStateMachineUpdate(ST_StateMachine* const pStateMachine);

typedef struct ST_StateMachineLinearWaypointFollower
{
	ST_Vector3List wayPoints;
	ST_Vector3ListData currentWayPoint;
	ST_Vector3ListData prevWayPoint;
	ST_Vector3 currentPosition;
	float speed;
	float dt;
	int wayPointIndex;
	ST_StateMachine stateMachine;
} ST_StateMachineLinearWaypointFollower;


void moveStateStart(ST_StateMachineLinearWaypointFollower* context);

void moveStateEnd(ST_StateMachineLinearWaypointFollower* context);

void moveStateUpdate(ST_StateMachineLinearWaypointFollower* context);

b32 moveStateEndCond(ST_StateMachineLinearWaypointFollower* context);

ST_StateMachineLinearWaypointFollower sphereTraceStateMachineLinearWaypointFollowerConstruct(ST_Vector3List waypoints,float speed);

void sphereTraceStateMachineLinearWaypointFollowerStart(ST_StateMachineLinearWaypointFollower* const context, int startIndex);

void sphereTraceStateMachineLinearWaypointFollowerUpdate(ST_StateMachineLinearWaypointFollower* context, float dt);