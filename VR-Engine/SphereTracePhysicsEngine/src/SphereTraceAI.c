#include "SphereTraceMath.h"
#include "SphereTraceAI.h"
#include <stdlib.h>

void sphereTraceStateMachineDefault()
{
	return;
}

b32 sphereTraceStateMachineDefaultConditional()
{
	return 0;
}


ST_State sphereTraceStateMachineStateConstruct(int stateIndex, int numConnections)
{
	ST_State state;
	state.numConnections = numConnections;
	state.connections = (ST_StateConnection*)malloc(sizeof(ST_StateConnection) * numConnections);
	state.stateIndex = stateIndex;
	state.onEnter = sphereTraceStateMachineDefault;
	state.onUpdate = sphereTraceStateMachineDefault;
	state.onExit = sphereTraceStateMachineDefault;
	return state;
}

ST_State sphereTraceStateMachineStateConstructWithStateFunctions(int stateIndex, int numConnections,  void (*onEnter)(void* machineContext), void (*onUpdate)(void* machineContext), void (*onExit)(void* machineContext), ST_StateConnection* connections)
{
	ST_State state;
	state.numConnections = numConnections;
	state.connections = (ST_StateConnection*)malloc(sizeof(ST_StateConnection) * numConnections);
	state.stateIndex = stateIndex;
	state.onUpdate = onUpdate;
	state.onEnter = onEnter;
	state.onExit = onExit;
	if (connections)
	{
		for (int i = 0; i < numConnections; i++)
		{
			state.connections[i] = connections[i];
		}
	}
	return state;
}

void sphereTraceStateMachineStateSetConnection(ST_State* const pState, const ST_StateConnection* const pStateConnections)
{
	for (int i = 0; i < pState->numConnections; i++)
	{
		pState->connections[i] = pStateConnections[i];
	}
}

ST_StateConnection sphereTraceStateMachineStateConnectionConstruct(int stateIndex, int nextStateIndex, b32(*conditionalFunction)(void* machineContext))
{
	ST_StateConnection stateConnection;
	stateConnection.stateIndex = stateIndex;
	stateConnection.nextStateIndex = nextStateIndex;
	stateConnection.conditionalFunction = conditionalFunction;
	return stateConnection;
}

ST_StateConnection sphereTraceStateMachineStateConnectionConstructWithStates(const ST_State* pState, const ST_State* pNextState, b32(*conditionalFunction)(void* machineContext))
{
	ST_StateConnection stateConnection;
	stateConnection.stateIndex = pState->stateIndex;
	stateConnection.nextStateIndex = pNextState->stateIndex;
	stateConnection.conditionalFunction = conditionalFunction;
	return stateConnection;
}

ST_StateMachine sphereTraceStateMachineConstruct(int numStates, void* machineContext)
{
	ST_StateMachine stateMachine;
	stateMachine.numStates = numStates;
	int sz = sizeof(ST_State);
	stateMachine.states = malloc(sizeof(ST_State) * numStates);
	stateMachine.currentStateIndex = 0;
	stateMachine.startStateIndex = 0;
	stateMachine.machineContext = machineContext;
	return stateMachine;
}

void sphereTraceStateMachineInsertState(ST_StateMachine* const pStateMachine, const ST_State* pState)
{
	pStateMachine->states[pState->stateIndex].onEnter = pState->onEnter;
	pStateMachine->states[pState->stateIndex].onExit = pState->onExit;
	pStateMachine->states[pState->stateIndex].onUpdate = pState->onUpdate;
	pStateMachine->states[pState->stateIndex].stateIndex = pState->stateIndex;
	pStateMachine->states[pState->stateIndex].numConnections = pState->numConnections;
	if (pState->numConnections > 0)
	{
		pStateMachine->states[pState->stateIndex].connections = pState->connections;
	}

}

void sphereTraceStateMachineStart(ST_StateMachine* const pStateMachine)
{
	pStateMachine->currentStateIndex = pStateMachine->startStateIndex;
	pStateMachine->states[pStateMachine->currentStateIndex].onEnter(pStateMachine->machineContext);
}
void sphereTraceStateMachineUpdate(ST_StateMachine* const pStateMachine)
{
	pStateMachine->states[pStateMachine->currentStateIndex].onUpdate(pStateMachine->machineContext);
	b32 nextStateTriggered = 0;
	int nextState;
	for (int i = 0; i < pStateMachine->states[pStateMachine->currentStateIndex].numConnections; i++)
	{
		if (pStateMachine->states[pStateMachine->currentStateIndex].connections[i].conditionalFunction(pStateMachine->machineContext))
		{
			nextStateTriggered = 1;
			nextState = pStateMachine->states[pStateMachine->currentStateIndex].connections[i].nextStateIndex;
			break;
		}
	}
	if (nextStateTriggered)
	{
		pStateMachine->states[pStateMachine->currentStateIndex].onExit(pStateMachine->machineContext);
		pStateMachine->currentStateIndex = nextState;
		pStateMachine->states[pStateMachine->currentStateIndex].onEnter(pStateMachine->machineContext);
	}
}

inline void setNextWayPoint(const ST_Vector3List* const pWayPoints, ST_Vector3ListData* const wayPointTracker)
{
	if (wayPointTracker->pNext != NULL)
	{
		*wayPointTracker = *wayPointTracker->pNext;
	}
	else
	{
		*wayPointTracker = *pWayPoints->pFirst;
	}
}

void moveStateStart(ST_StateMachineLinearWaypointFollower* context)
{
	context->currentPosition = context->prevWayPoint.value;
}

void moveStateEnd(ST_StateMachineLinearWaypointFollower* context)
{
	context->wayPointIndex = (context->wayPointIndex + 1) % context->wayPoints.count;
	setNextWayPoint(&context->wayPoints, &context->prevWayPoint);
	setNextWayPoint(&context->wayPoints, &context->currentWayPoint);
}

void moveStateUpdate(ST_StateMachineLinearWaypointFollower* context)
{
	ST_Vector3 dir = sphereTraceNormalizeBetweenPoints(context->currentWayPoint.value, context->prevWayPoint.value);
	context->currentPosition = sphereTraceVector3AddAndScale(context->currentPosition, dir, context->speed * context->dt);
}

b32 moveStateEndCond(ST_StateMachineLinearWaypointFollower* context)
{
	ST_Vector3 dir = sphereTraceNormalizeBetweenPoints(context->currentWayPoint.value, context->prevWayPoint.value);
	ST_Vector3 dp = sphereTraceNormalizeBetweenPoints(context->currentWayPoint.value, context->currentPosition);
	if (sphereTraceVector3Dot(dir, dp) < 0.0f || sphereTraceVector3Nan(dir))
		return 1;
	return 0;
}

ST_StateMachineLinearWaypointFollower sphereTraceStateMachineLinearWaypointFollowerConstruct(ST_Vector3List waypoints,
	float speed)
{
	ST_StateMachineLinearWaypointFollower lwf;
	lwf.speed = speed;
	lwf.wayPoints = waypoints;
	lwf.currentWayPoint = *waypoints.pFirst->pNext;
	lwf.prevWayPoint = *waypoints.pFirst;
	lwf.currentPosition = waypoints.pFirst->value;
	lwf.wayPointIndex = 0;
	lwf.dt = 0.0f;
	ST_State movestate = sphereTraceStateMachineStateConstructWithStateFunctions(0, 1, moveStateStart, moveStateUpdate, moveStateEnd, NULL);
	ST_StateConnection cs = sphereTraceStateMachineStateConnectionConstructWithStates(&movestate,
		&movestate, moveStateEndCond);
	sphereTraceStateMachineStateSetConnection(&movestate, &cs);
	lwf.stateMachine = sphereTraceStateMachineConstruct(1, NULL);
	sphereTraceStateMachineInsertState(&lwf.stateMachine, &movestate);
	return lwf;
}

void sphereTraceStateMachineLinearWaypointFollowerStart(ST_StateMachineLinearWaypointFollower* const context, int startIndex)
{
	context->stateMachine.machineContext = context;
	context->wayPointIndex = startIndex % context->wayPoints.count;
	for (int i = 0; i < context->wayPointIndex; i++)
	{
		setNextWayPoint(&context->wayPoints, &context->prevWayPoint);
		setNextWayPoint(&context->wayPoints, &context->currentWayPoint);
	}
	context->currentPosition = context->prevWayPoint.value;
}

void sphereTraceStateMachineLinearWaypointFollowerUpdate(ST_StateMachineLinearWaypointFollower* context, float dt)
{
	context->dt = dt;
	sphereTraceStateMachineUpdate(&context->stateMachine);
}
