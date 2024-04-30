#pragma once
#include <stdlib.h>
#include "SphereTraceMath.h"

typedef uintptr_t ST_Index;

typedef struct ST_IndexListData
{
	ST_Index value;
	struct ST_IndexListData* pNext;
} ST_IndexListData;

typedef struct ST_IndexList
{
	ST_Index count;
	ST_IndexListData* pFirst;
} ST_IndexList;

ST_IndexList sphereTraceIndexListConstruct();
void sphereTraceIndexListAddFirst(ST_IndexList* const pIntList, ST_Index value);
void sphereTraceIndexListAddLast(ST_IndexList* const pIntList, ST_Index value);
b32 sphereTraceIndexListAddUnique(ST_IndexList* const pIntList, ST_Index value);
void sphereTraceIndexListRemoveFirstInstance(ST_IndexList* const pIntList, ST_Index value);
void sphereTraceIndexListFree(ST_IndexList* const pIntList);
void sphereTraceIndexListReset(ST_IndexList* const pIntList);
b32 sphereTraceIndexListEqual(ST_IndexList* const pIntListA, ST_IndexList* const pIntListB);
b32 sphereTraceIndexListContains(const ST_IndexList* const pIntList, ST_Index value);
ST_IndexList sphereTraceIndexListConstructForDeletedValues(const ST_IndexList* const pOldIntList, const ST_IndexList* const pNewIntList);
void sphereTraceIndexListPrint(const ST_IndexList* const pIntList);
//all sorted list functions, assumes sorted in ascending order
void sphereTraceSortedIndexListAdd(ST_IndexList* const pIntList, ST_Index value);
b32 sphereTraceSortedIndexListAddUnique(ST_IndexList* const pIntList, ST_Index value);
void sphereTraceSortedIndexListMergeUnique(ST_IndexList* const pIntListMergeFrom, ST_IndexList* const pIntListMergeTo);
void sphereTraceSortedIndexListRemove(ST_IndexList* const pIntList, ST_Index value);
b32 sphereTraceSortedIndexListContains(const ST_IndexList* const pIntList, ST_Index value);

typedef struct ST_KeyValueListData
{
	ST_Index key;
	ST_Index value;
	struct ST_KeyValueListData* pNext;
} ST_KeyValueListData;

typedef struct ST_KeyValueList
{
	ST_Index count;
	ST_KeyValueListData* pFirst;
} ST_KeyValueList;

ST_KeyValueList sphereTraceKeyValueListConstruct();
b32 sphereTraceKeyValueListAdd(ST_KeyValueList* const pList, ST_Index key, ST_Index value);
void sphereTraceKeyValueListRemoveKey(ST_KeyValueList* const pList, ST_Index key);
void sphereTraceKeyValueListFree(ST_KeyValueList* const pList);
b32 sphereTraceKeyValueListContainsKey(const ST_KeyValueList* const pList, ST_Index key);
void sphereTraceKeyValueListPrint(const ST_KeyValueList* const pList);
//all sorted list functions, assumes sorted in ascending order
b32 sphereTraceSortedKeyValueListAddUnique(ST_KeyValueList* const pList, ST_Index key, ST_Index value);
void sphereTraceSortedKeyValueListRemoveKey(ST_KeyValueList* const pList, ST_Index key);
b32 sphereTraceSortedKeyValueListContainsKey(const ST_KeyValueList* const pList, ST_Index key);

typedef struct ST_Vector3ListData
{
	ST_Vector3 value;
	struct ST_Vector3ListData* pNext;
} ST_Vector3ListData;

typedef struct ST_Vector3List
{
	int count;
	ST_Vector3ListData* pFirst;
} ST_Vector3List;

ST_Vector3List sphereTraceVector3ListConstruct();

void sphereTraceVector3ListAddLast(ST_Vector3List* const pVector3List, ST_Vector3 value);

void sphereTraceVector3ListAddFirst(ST_Vector3List* const pVector3List, ST_Vector3 value);

void sphereTraceVector3ListRemoveFirst(ST_Vector3List* const pVector3List);

void sphereTraceVector3ListRemoveLast(ST_Vector3List* const pVector3List);

b32 sphereTraceVector3ListAddUnique(ST_Vector3List* const pVector3List, ST_Vector3 value);

void sphereTraceVector3ListRemoveFirstInstance(ST_Vector3List* const pVector3List, ST_Vector3 value);

void sphereTraceVector3ListFree(ST_Vector3List* const pVector3List);

b32 sphereTraceVector3ListContains(const ST_Vector3List* const pVector3List, ST_Vector3 value);


void sphereTraceVector3ListPrint(const ST_Vector3List* const pVector3List);

ST_Vector3 sphereTraceVector3ListAverage(const ST_Vector3List* const pVector3List);

void sphereTraceVector3ListMoveOffset(const ST_Vector3List* const pVector3List, ST_Vector3 offset);