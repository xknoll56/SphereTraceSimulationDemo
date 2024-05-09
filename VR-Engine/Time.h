#pragma once

#include "SphereTracePhysicsEngine/src/SphereTraceMath.h"
#include "SphereTracePhysicsEngine/src/SphereTraceGlobals.h"

typedef struct Time
{
	double currentTimeInSeconds;
	float dt;
	LONGLONG startPerfCount;
	LONGLONG perfCounterFrequency;
	ST_Index cycles;
	b32 timerLocked;
	b32 unlockTimer;


	void init()
	{
		LARGE_INTEGER perfCount;
		QueryPerformanceCounter(&perfCount);
		startPerfCount = perfCount.QuadPart;
		LARGE_INTEGER perfFreq;
		QueryPerformanceFrequency(&perfFreq);
		perfCounterFrequency = perfFreq.QuadPart;
		currentTimeInSeconds = 0.0;
		timerLocked = 0;
		unlockTimer = 0;
		cycles = 0;
	}



	void update()
	{
		if (unlockTimer)
		{
			timerLocked = 0;
			unlockTimer = 0;
		}
		if (!timerLocked)
		{
			double previousTimeInSeconds = currentTimeInSeconds;
			LARGE_INTEGER perfCount;
			QueryPerformanceCounter(&perfCount);

			cycles = perfCount.QuadPart - startPerfCount;
			currentTimeInSeconds = (double)(cycles) / (double)perfCounterFrequency;
			dt = (float)(currentTimeInSeconds - previousTimeInSeconds);
		}
		else
		{
			LARGE_INTEGER perfCount;
			QueryPerformanceCounter(&perfCount);
			startPerfCount = perfCount.QuadPart;
		}
	}
} Time;



//float timeGetRandomFloatBetween0And1()
//{
//	return (float)rand() / RAND_MAX;
//}
//
//ST_Vector3 timeGetPointWithingAABB(ST_AABB* paabb)
//{
//	ST_Vector3 midPoint = sphereTraceVector3Average(paabb->lowExtent, paabb->highExtent);
//	return {
//		2 * (timeGetRandomFloatBetween0And1() - 0.5f) * paabb->halfExtents.x + midPoint.x,
//			2 * (timeGetRandomFloatBetween0And1() - 0.5f) * paabb->halfExtents.y + midPoint.y,
//			2 * (timeGetRandomFloatBetween0And1() - 0.5f) * paabb->halfExtents.z + midPoint.z
//	};
//}
