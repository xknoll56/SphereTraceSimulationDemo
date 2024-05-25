#pragma once


void sphereTraceSimulationGlobalSolveDiscreteFirstComeFirstServe(ST_SimulationSpace* const pSimulationSpace, float dt, ST_Index iterations);

void sphereTraceSimulationGlobalSolveDiscrete(ST_SimulationSpace* const pSimulationSpace, float dt);

void sphereTraceBoxSimulationGlobalSolveDiscrete(ST_SimulationSpace* const pSimulationSpace, float dt);

void sphereTraceSimulationGlobalSolveImposedPosition(ST_SimulationSpace* const pSimulationSpace, float dt);