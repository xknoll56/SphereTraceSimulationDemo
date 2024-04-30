#pragma once
#include "SphereTraceMath.h"
#include "SphereTraceLists.h"
#include <stdarg.h>


typedef struct ST_HashTable
{
	ST_Index count;
	ST_IndexList* lists;
	ST_Index (*hashFunction)();
	ST_IndexList activeLists;
} ST_HashTable;


ST_HashTable sphereTraceHashTableConstruct(ST_Index count, ST_Index (*hashFunction)());

void sphereTraceHashTableAddUnique(ST_HashTable* const pHashTable, ST_Index id, ...);

void sphereTraceHashTableRemove(ST_HashTable* const pHashTable, ST_Index id);