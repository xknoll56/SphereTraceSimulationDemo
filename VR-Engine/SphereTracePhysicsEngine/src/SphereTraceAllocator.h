#pragma once
#include <stdlib.h>
#include "SphereTraceLists.h"
//#include "SphereTraceCollider.h"

#define ST_DEBUG

typedef uintptr_t ST_Index;
typedef struct ST_CallbackFunction ST_CallbackFunction;
typedef struct ST_SphereContactEntry ST_SphereContactEntry;
typedef struct ST_SphereContact ST_SphereContact;
typedef struct ST_BoxContact ST_BoxContact;
typedef struct ST_OctTreeNode ST_OctTreeNode;
typedef struct ST_AABBContact ST_AABBContact;
typedef enum ST_ColliderType ST_ColliderType;
typedef struct ST_Collider ST_Collider;

typedef struct ST_FreeStack
{
	ST_Index* pStack;
	ST_Index head;
	ST_Index capacity;
} ST_FreeStack;

ST_FreeStack sphereTraceAllocatorFreeStackConstruct(ST_Index capacity, void* pArr, ST_Index objectSize);
ST_Index* sphereTraceAllocatorFreeStackPurchase(ST_FreeStack* const pFreeStack);
void sphereTraceAllocatorFreeStackSell(ST_FreeStack* const pFreeStack, ST_Index* pIndex);
void sphereTraceAllocatorFreeStackDelete(ST_FreeStack* const pFreeStack);


typedef struct ST_ObjectPool
{
	void* data;
	ST_FreeStack freeStack;
	ST_Index occupied;
	ST_Index capacity;
	ST_Index objectSize;
} ST_ObjectPool;

typedef struct ST_ArrayPool
{
	void** data;
	ST_Index arraySize;
	ST_FreeStack freeStack;
	ST_Index occupied;
	ST_Index capacity;
	ST_Index objectSize;
} ST_ArrayPool;

ST_ObjectPool sphereTraceAllocatorObjectPoolConstruct(ST_Index capacity, ST_Index objectSize);
void* sphereTraceAllocatorObjectPoolAllocateObject(ST_ObjectPool* const pObjectPool);
void sphereTraceAllocatorObjectPoolFreeObject(ST_ObjectPool* const pObjectPool, void* pObject);
void sphereTraceAllocatorObjectPoolDelete(ST_ObjectPool* const pObjectPool);

ST_ArrayPool sphereTraceAllocatorArrayPoolConstruct(ST_Index capacity, ST_Index objectSize, ST_Index arraySize);
void* sphereTraceAllocatorArrayPoolAllocateObject(ST_ArrayPool* const pArrayPool);
void sphereTraceAllocatorArrayPoolFreeObject(ST_ArrayPool* const pArrayPool, void* pObject);


//linear object pools are really just pre-allocated arrays 
//they will not make use of the free stack
void sphereTraceAllocatorLinearObjectPoolReset(ST_ObjectPool* const pLinearObjectPool);

typedef struct ST_Allocator
{
	ST_ObjectPool* objectPools;
	ST_ObjectPool* pCurrentObjectPool;
	ST_Index numObjects;
	ST_Index curPoolIndex;
	ST_Index maxPools;
} ST_Allocator;

ST_Allocator sphereTraceAllocatorConstruct(ST_Index poolCapacity, ST_Index objectSize, ST_Index maxPools);
void* sphereTraceAllocatorAllocateObject(ST_Allocator* const pAllocator);
void sphereTraceAllocatorFreeObject(ST_Allocator* const pAllocator, void* pObject);
void sphereTraceAllocatorDelete(ST_Allocator* const pAllocator);

//Linear allocaters are different in that they can only be free'd all at once (ie reset)
//and elements can be accessed by index
ST_Allocator sphereTraceLinearAllocatorConstruct(ST_Index poolCapacity, ST_Index objectSize, ST_Index maxPools);
void* sphereTraceLinearAllocatorAllocateObject(ST_Allocator* const pLinearAllocator);
void sphereTraceLinearAllocatorReset(ST_Allocator* const pLinearAllocator);
void* sphereTraceLinearAllocatorGetByIndex(ST_Allocator* const pLinearAllocator, ST_Index index);

//random allocator functions
void sphereTraceAllocatorInitialize();
ST_OctTreeNode* sphereTraceAllocatorAllocateOctTreeNode();
void sphereTraceAllocatorFreeOctTreeNode(void* pIndex);
//ST_SpaceObjectEntry* sphereTraceAllocatorAllocateOctTreeObjectEntry();
//void sphereTraceAllocatorFreeOctTreeObjectEntry(void* pIndex);
ST_IndexListData* sphereTraceAllocatorAllocateIndexListData();
void sphereTraceAllocatorFreeIndexListData(void* pIndex);
ST_KeyValueListData* sphereTraceAllocatorAllocateKeyValueListData();
void sphereTraceAllocatorFreeKeyValueListData(void* pIndex);
ST_Vector3ListData* sphereTraceAllocatorAllocateVector3ListData();
void sphereTraceAllocatorFreeVector3ListData(void* pIndex);
ST_Vector3* sphereTraceAllocatorAllocateVector3();
void sphereTraceAllocatorFreeVector3(void* pIndex);
ST_CallbackFunction* sphereTraceAllocatorAllocateCallbackFunction();
void sphereTraceAllocatorFreeCallbackFunction(void* pIndex);
ST_SphereContactEntry* sphereTraceAllocatorAllocateContactEntry();
void sphereTraceAllocatorFreeContactEntry(void* pIndex);
ST_Collider* sphereTraceAllocatorAllocateCollider(ST_ColliderType colliderType);
void sphereTraceAllocatorFreeCollider(ST_ColliderType colliderType, void* pIndex);

//linear allocator functions
ST_SphereContact* sphereTraceLinearAllocatorAllocateSphereContact();
ST_Index sphereTraceLinearAllocatorGetSphereContactCount();
ST_SphereContact* sphereTraceLinearAllocatorGetSphereContactByIndex(ST_Index index);
void sphereTraceLinearAllocatorResetSphereContacts();
ST_BoxContact* sphereTraceLinearAllocatorAllocateBoxContact();
ST_Index sphereTraceLinearAllocatorGetBoxContactCount();
ST_BoxContact* sphereTraceLinearAllocatorGetBoxContactByIndex(ST_Index index);
void sphereTraceLinearAllocatorResetBoxContacts();

//array allocators
ST_IndexList* sphereTraceAllocatorAllocateIndexListArray();
void sphereTraceAllocatorFreeIndexListArray(void* pArr);
void sphereTraceAllocatorIndexListArrayResize(ST_Index size);

//debug functions
#ifdef ST_DEBUG
	ST_Index sphereTraceAllocatorGetBytesAllocated();
#endif
