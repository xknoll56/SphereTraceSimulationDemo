#pragma once

#include "SphereTraceAllocator.h"
#include "SphereTraceLists.h"
#include "SphereTraceCollider.h"
#include "SphereTraceSpacialPartition.h"



#define ST_INDEX_POOL_DEFAULT_SIZE 1000
#define ST_OCTTREENODE_POOL_DEFAULT_SIZE 1000
#define ST_FUNCTION_POOL_DEFAULT_SIZE 20
#define ST_INDEX_ALLOCATOR_DEFAULT_SIZE 5
#define ST_CONTACT_LINEAR_ALLOCATOR_SIZE 10
#define ST_COLLIDER_ALLOCATOR_SIZE 50
#define ST_LIST_ARRAY_SIZE 125
ST_Index listArraySize;
ST_Index bytesAllocated;


static ST_Allocator indexListAllocator;
static ST_Allocator listArrayAllocator;
static ST_Allocator keyValueListAllocator;
static ST_Allocator vector3ListAllocator;
static ST_Allocator vector3Allocator;
static ST_Allocator callbackFunctionAllocator;
static ST_Allocator contactEntryAllocator;
static ST_Allocator sphereContactLinearAllocator;
static ST_Allocator boxContactLinearAllocator;
static ST_Allocator octTreeNodeAllocator;
static ST_Allocator sphereColliderAllocator;
static ST_Allocator planeColliderAllocator;
static ST_Allocator triangleColliderAllocator;

ST_FreeStack sphereTraceAllocatorFreeStackConstruct(ST_Index capacity, void* pArr, ST_Index objectSize)
{
	ST_FreeStack stack;
	stack.pStack = (ST_Index*)malloc(capacity * sizeof(ST_Index));
	for (ST_Index i = 0; i < capacity; i++)
	{
		stack.pStack[i] = (ST_Index)pArr+i*objectSize;
	}
	stack.head = 0;
	stack.capacity = capacity;
	return stack;
}

ST_Index* sphereTraceAllocatorFreeStackPurchase(ST_FreeStack* const pFreeStack)
{
	if (pFreeStack->head < pFreeStack->capacity)
	{
		ST_Index* freeIndex = pFreeStack->pStack[pFreeStack->head];
		pFreeStack->head++;
		return freeIndex;
	}
}


void sphereTraceAllocatorFreeStackSell(ST_FreeStack* const pFreeStack, ST_Index* pIndex)
{
	if (pFreeStack->head > 0)
	{
		pFreeStack->head--;
		pFreeStack->pStack[pFreeStack->head] = pIndex;
	}
}

void sphereTraceAllocatorFreeStackDelete(ST_FreeStack* const pFreeStack)
{
	free(pFreeStack->pStack);
}


ST_ObjectPool sphereTraceAllocatorObjectPoolConstruct(ST_Index capacity, ST_Index objectSize)
{
	ST_ObjectPool objectPool;
	objectPool.objectSize = objectSize;
	objectPool.data = malloc(capacity * objectSize);
	objectPool.freeStack = sphereTraceAllocatorFreeStackConstruct(capacity, objectPool.data, objectSize);
	objectPool.capacity = capacity;
	objectPool.occupied = 0;
	return objectPool;
}

void* sphereTraceAllocatorObjectPoolAllocateObject(ST_ObjectPool* const pObjectPool)
{
	if (pObjectPool->occupied < pObjectPool->capacity)
	{
#ifdef ST_DEBUG
		bytesAllocated += pObjectPool->objectSize;
#endif
		ST_Index* pIndex = sphereTraceAllocatorFreeStackPurchase(&pObjectPool->freeStack);
		pObjectPool->occupied++;
		return pIndex;
	}
	else
		return NULL;
}

void sphereTraceAllocatorObjectPoolFreeObject(ST_ObjectPool* const pObjectPool, void* pObject)
{
	if (pObjectPool->occupied > 0)
	{
#ifdef ST_DEBUG
		bytesAllocated -= pObjectPool->objectSize;
#endif
		sphereTraceAllocatorFreeStackSell(&pObjectPool->freeStack, pObject);
		pObjectPool->occupied--;
	}
}

void sphereTraceAllocatorObjectPoolDelete(ST_ObjectPool* const pObjectPool)
{
#ifdef ST_DEBUG
	bytesAllocated -= pObjectPool->occupied * pObjectPool->objectSize;
#endif
	sphereTraceAllocatorFreeStackDelete(&pObjectPool->freeStack);
	free(pObjectPool->data);
}

ST_ArrayPool sphereTraceAllocatorArrayPoolConstruct(ST_Index capacity, ST_Index objectSize, ST_Index arraySize)
{
	ST_ArrayPool arrayPool;
	arrayPool.objectSize = objectSize;
	arrayPool.arraySize = arraySize;
	arrayPool.data = malloc(capacity * sizeof(ST_Index));
	for (int i = 0; i < capacity; i++)
	{
		arrayPool.data[i] = malloc(objectSize * arraySize);
	}
	arrayPool.freeStack = sphereTraceAllocatorFreeStackConstruct(capacity, arrayPool.data, sizeof(ST_Index));
	arrayPool.capacity = capacity;
	arrayPool.occupied = 0;
	return arrayPool;
}
void* sphereTraceAllocatorArrayPoolAllocateObject(ST_ArrayPool* const pArrayPool)
{
	if (pArrayPool->occupied < pArrayPool->capacity)
	{
		ST_Index* pIndex = sphereTraceAllocatorFreeStackPurchase(&pArrayPool->freeStack);
		pArrayPool->occupied++;
		return pIndex;
	}
	else
		return NULL;
}
void sphereTraceAllocatorArrayPoolFreeObject(ST_ArrayPool* const pArrayPool, void* pObject)
{
	if (pArrayPool->occupied > 0)
	{
		sphereTraceAllocatorFreeStackSell(&pArrayPool->freeStack, pObject);
		pArrayPool->occupied--;
	}
}

void sphereTraceAllocatorLinearObjectPoolReset(ST_ObjectPool* const pLinearObjectPool)
{
#ifdef ST_DEBUG
	bytesAllocated -= pLinearObjectPool->occupied * pLinearObjectPool->objectSize;
#endif
	pLinearObjectPool->occupied = 0;
	pLinearObjectPool->freeStack.head = 0;
}

ST_Allocator sphereTraceAllocatorConstruct(ST_Index poolCapacity, ST_Index objectSize, ST_Index maxPools)
{
	ST_Allocator allocator;
	allocator.curPoolIndex = 0;
	allocator.numObjects = 0;
	allocator.maxPools = maxPools;
	allocator.objectPools = (ST_ObjectPool*)malloc(allocator.maxPools * sizeof(ST_ObjectPool));
	for (int i = 0; i < maxPools; i++)
	{
		//allocator.objectPools[i] = (ST_ObjectPool*)malloc(sizeof(ST_ObjectPool));
		allocator.objectPools[i] = sphereTraceAllocatorObjectPoolConstruct(poolCapacity, objectSize);
	}
	allocator.pCurrentObjectPool = &allocator.objectPools[0];
	return allocator;
}


//If the allocator failes to reallocate, the program is closed because we
//are out of memory
b32 sphereTraceAllocatorIncrementObject(ST_Allocator* const pAllocator)
{
	pAllocator->numObjects++;
	if (pAllocator->numObjects % pAllocator->objectPools[0].capacity == 0)
	{
		pAllocator->curPoolIndex++;
		pAllocator->pCurrentObjectPool = &pAllocator->objectPools[pAllocator->curPoolIndex];
	}
	if (pAllocator->curPoolIndex >= pAllocator->maxPools)
	{
		ST_Index newMax = pAllocator->maxPools * 2;
		void* dataPointers = (ST_ObjectPool*)realloc(pAllocator->objectPools, newMax * sizeof(ST_ObjectPool));
		if (dataPointers)
		{
			pAllocator->objectPools = dataPointers;
			for (int i = pAllocator->maxPools; i < newMax; i++)
			{
				//pAllocator->objectPools[i] = (ST_ObjectPool*)malloc(sizeof(ST_ObjectPool));
				pAllocator->objectPools[i] = sphereTraceAllocatorObjectPoolConstruct(pAllocator->objectPools[0].capacity,
					pAllocator->objectPools[0].objectSize);
			}
			pAllocator->maxPools = newMax;
			return 1;
		}
		else
		{
			return 0;
		}
	}
}

void* sphereTraceAllocatorAllocateObject(ST_Allocator* const pAllocator)
{
	if (!sphereTraceAllocatorIncrementObject(pAllocator))
		exit(0);
	return sphereTraceAllocatorObjectPoolAllocateObject(&pAllocator->objectPools[pAllocator->curPoolIndex]);
}

void sphereTraceAllocatorFreeObject(ST_Allocator* const pAllocator, void* pObject)
{
	if (pAllocator->numObjects > 0)
	{
		pAllocator->numObjects--;
		sphereTraceAllocatorObjectPoolFreeObject(&pAllocator->objectPools[pAllocator->curPoolIndex], pObject);
		if (pAllocator->objectPools[pAllocator->curPoolIndex].occupied == 0 && pAllocator->curPoolIndex >0)
			pAllocator->curPoolIndex--;
	}
}

void sphereTraceAllocatorDelete(ST_Allocator* const pAllocator)
{
	for (int i = 0; i < pAllocator->maxPools; i++)
		sphereTraceAllocatorObjectPoolDelete(&pAllocator->objectPools[i]);
	free(pAllocator->objectPools);
}

ST_Allocator sphereTraceLinearAllocatorConstruct(ST_Index poolCapacity, ST_Index objectSize, ST_Index maxPools)
{
	ST_Allocator linearAllocator;
	linearAllocator.maxPools = maxPools;
	linearAllocator.curPoolIndex = 0;
	linearAllocator.numObjects = 0;
	linearAllocator.objectPools = (ST_ObjectPool*)malloc(linearAllocator.maxPools * sizeof(ST_ObjectPool));
	for (int i = 0; i < maxPools; i++)
	{
		//linearAllocator.objectPools[i] = (ST_ObjectPool*)malloc(sizeof(ST_ObjectPool));
		linearAllocator.objectPools[i] = sphereTraceAllocatorObjectPoolConstruct(poolCapacity, objectSize);
	}
	linearAllocator.pCurrentObjectPool = &linearAllocator.objectPools[0];
	return linearAllocator;
}

void* sphereTraceLinearAllocatorAllocateObject(ST_Allocator* const pAllocator)
{
	ST_Index* pObject = pAllocator->pCurrentObjectPool->freeStack.pStack[pAllocator->pCurrentObjectPool->occupied++];
	if (!sphereTraceAllocatorIncrementObject(pAllocator))
		exit(0);
#ifdef ST_DEBUG
	bytesAllocated += pAllocator->objectPools[0].objectSize;
#endif
	return pObject;
}

void sphereTraceLinearAllocatorReset(ST_Allocator* const pLinearAllocator)
{
	for (int i = 0; i < pLinearAllocator->curPoolIndex +1; i++)
	{
		sphereTraceAllocatorLinearObjectPoolReset(&pLinearAllocator->objectPools[i]);
	}
	pLinearAllocator->curPoolIndex = 0;
	pLinearAllocator->pCurrentObjectPool = &pLinearAllocator->objectPools[0];
	pLinearAllocator->numObjects = 0;
}

void* sphereTraceLinearAllocatorGetByIndex(ST_Allocator* const pLinearAllocator, ST_Index index)
{
	if (index < pLinearAllocator->numObjects)
	{
		ST_Index poolIndex = index / pLinearAllocator->objectPools[0].capacity;
		ST_ObjectPool* curPool = &pLinearAllocator->objectPools[poolIndex];
		return (ST_Index)curPool->data + (index % curPool->capacity) * curPool->objectSize;
	}
	return NULL;
}


void sphereTraceAllocatorInitialize()
{
	indexListAllocator = sphereTraceAllocatorConstruct(ST_INDEX_POOL_DEFAULT_SIZE, sizeof(ST_IndexListData), ST_INDEX_ALLOCATOR_DEFAULT_SIZE);
	keyValueListAllocator = sphereTraceAllocatorConstruct(ST_INDEX_POOL_DEFAULT_SIZE, sizeof(ST_KeyValueListData), ST_INDEX_ALLOCATOR_DEFAULT_SIZE);
	vector3ListAllocator = sphereTraceAllocatorConstruct(ST_INDEX_POOL_DEFAULT_SIZE, sizeof(ST_Vector3ListData), ST_INDEX_ALLOCATOR_DEFAULT_SIZE);
	vector3Allocator = sphereTraceAllocatorConstruct(ST_INDEX_POOL_DEFAULT_SIZE, sizeof(ST_Vector3), ST_INDEX_ALLOCATOR_DEFAULT_SIZE);
	callbackFunctionAllocator = sphereTraceAllocatorConstruct(ST_FUNCTION_POOL_DEFAULT_SIZE, sizeof(ST_CallbackFunction), ST_INDEX_ALLOCATOR_DEFAULT_SIZE);
	contactEntryAllocator = sphereTraceAllocatorConstruct(ST_INDEX_POOL_DEFAULT_SIZE, sizeof(ST_SphereContactEntry), ST_INDEX_ALLOCATOR_DEFAULT_SIZE);
	sphereContactLinearAllocator = sphereTraceLinearAllocatorConstruct(ST_CONTACT_LINEAR_ALLOCATOR_SIZE, sizeof(ST_SphereContact), ST_INDEX_ALLOCATOR_DEFAULT_SIZE);
	boxContactLinearAllocator = sphereTraceLinearAllocatorConstruct(ST_CONTACT_LINEAR_ALLOCATOR_SIZE, sizeof(ST_BoxContact), ST_INDEX_ALLOCATOR_DEFAULT_SIZE);
	//aabbContactLinearAllocator = sphereTraceLinearAllocatorConstruct(ST_CONTACT_LINEAR_ALLOCATOR_SIZE, sizeof(ST_AABB), ST_INDEX_ALLOCATOR_DEFAULT_SIZE);
	octTreeNodeAllocator = sphereTraceAllocatorConstruct(ST_OCTTREENODE_POOL_DEFAULT_SIZE, sizeof(ST_OctTreeNode), ST_INDEX_ALLOCATOR_DEFAULT_SIZE);
	//octTreeObjectEntryAllocator = sphereTraceAllocatorConstruct(ST_OCTTREENODE_POOL_DEFAULT_SIZE, sizeof(ST_SpaceObjectEntry), ST_INDEX_ALLOCATOR_DEFAULT_SIZE);
	sphereColliderAllocator = sphereTraceAllocatorConstruct(ST_COLLIDER_ALLOCATOR_SIZE, sizeof(ST_SphereCollider), ST_INDEX_ALLOCATOR_DEFAULT_SIZE);
	planeColliderAllocator = sphereTraceAllocatorConstruct(ST_COLLIDER_ALLOCATOR_SIZE, sizeof(ST_PlaneCollider), ST_INDEX_ALLOCATOR_DEFAULT_SIZE);
	triangleColliderAllocator = sphereTraceAllocatorConstruct(ST_COLLIDER_ALLOCATOR_SIZE, sizeof(ST_TriangleCollider), ST_INDEX_ALLOCATOR_DEFAULT_SIZE);
	listArrayAllocator = sphereTraceAllocatorConstruct(ST_COLLIDER_ALLOCATOR_SIZE, sizeof(ST_IndexList) * ST_LIST_ARRAY_SIZE, ST_INDEX_ALLOCATOR_DEFAULT_SIZE);
	listArraySize = ST_LIST_ARRAY_SIZE;
}

ST_OctTreeNode* sphereTraceAllocatorAllocateOctTreeNode()
{
	return sphereTraceAllocatorAllocateObject(&octTreeNodeAllocator);
}

void sphereTraceAllocatorFreeOctTreeNode(void* pIndex)
{
	sphereTraceAllocatorFreeObject(&octTreeNodeAllocator, pIndex);
}

//ST_SpaceObjectEntry* sphereTraceAllocatorAllocateOctTreeObjectEntry()
//{
//	return sphereTraceAllocatorAllocateObject(&octTreeObjectEntryAllocator);
//}
//void sphereTraceAllocatorFreeOctTreeObjectEntry(void* pIndex)
//{
//	sphereTraceAllocatorFreeObject(&octTreeObjectEntryAllocator, pIndex);
//}

ST_IndexListData* sphereTraceAllocatorAllocateIndexListData()
{
	return sphereTraceAllocatorAllocateObject(&indexListAllocator);
}

void sphereTraceAllocatorFreeIndexListData(void* pIndex)
{
	sphereTraceAllocatorFreeObject(&indexListAllocator, pIndex);
}

ST_KeyValueListData* sphereTraceAllocatorAllocateKeyValueListData()
{
	return sphereTraceAllocatorAllocateObject(&keyValueListAllocator);
}

void sphereTraceAllocatorFreeKeyValueListData(void* pIndex)
{
	sphereTraceAllocatorFreeObject(&keyValueListAllocator, pIndex);
}

ST_Vector3ListData* sphereTraceAllocatorAllocateVector3ListData()
{
	return sphereTraceAllocatorAllocateObject(&vector3ListAllocator);
}

void sphereTraceAllocatorFreeVector3ListData(void* pIndex)
{
	sphereTraceAllocatorFreeObject(&vector3ListAllocator, pIndex);
}

ST_Vector3 *sphereTraceAllocatorAllocateVector3()
{
	return sphereTraceAllocatorObjectPoolAllocateObject(&vector3Allocator);
}

void sphereTraceAllocatorFreeVector3(void* pIndex)
{
	sphereTraceAllocatorFreeObject(&vector3Allocator, pIndex);
}


ST_CallbackFunction* sphereTraceAllocatorAllocateCallbackFunction()
{
	return sphereTraceAllocatorAllocateObject(&callbackFunctionAllocator);
}

void sphereTraceAllocatorFreeCallbackFunction(void* pIndex)
{
	sphereTraceAllocatorFreeObject(&callbackFunctionAllocator, pIndex);
}

ST_SphereContactEntry* sphereTraceAllocatorAllocateContactEntry()
{
	return sphereTraceAllocatorAllocateObject(&contactEntryAllocator);
}

ST_Collider* sphereTraceAllocatorAllocateCollider(ST_ColliderType colliderType)
{
	switch (colliderType)
	{
	case COLLIDER_SPHERE:
		return sphereTraceAllocatorAllocateObject(&sphereColliderAllocator);
		break;
	case COLLIDER_PLANE:
		return sphereTraceAllocatorAllocateObject(&planeColliderAllocator);
		break;
	case COLLIDER_TRIANGLE:
		return sphereTraceAllocatorAllocateObject(&triangleColliderAllocator);
		break;
	}
}

void sphereTraceAllocatorFreeCollider(ST_ColliderType colliderType, void* pIndex)
{
	sphereTraceColliderFree(pIndex);
	switch (colliderType)
	{
	case COLLIDER_SPHERE:
		sphereTraceAllocatorFreeObject(&sphereColliderAllocator, pIndex);
		break;
	case COLLIDER_PLANE:
		sphereTraceAllocatorFreeObject(&planeColliderAllocator, pIndex);
		break;
	case COLLIDER_TRIANGLE:
		sphereTraceAllocatorFreeObject(&triangleColliderAllocator, pIndex);
		break;
	}
}


void sphereTraceAllocatorFreeContactEntry(void* pIndex)
{
	sphereTraceAllocatorFreeObject(&contactEntryAllocator, pIndex);
}

ST_SphereContact* sphereTraceLinearAllocatorAllocateSphereContact()
{
	return sphereTraceLinearAllocatorAllocateObject(&sphereContactLinearAllocator);
}

ST_Index sphereTraceLinearAllocatorGetSphereContactCount()
{
	return sphereContactLinearAllocator.numObjects;
}

ST_SphereContact* sphereTraceLinearAllocatorGetSphereContactByIndex(ST_Index index)
{
	return sphereTraceLinearAllocatorGetByIndex(&sphereContactLinearAllocator, index);
}

void sphereTraceLinearAllocatorResetSphereContacts()
{
	sphereTraceLinearAllocatorReset(&sphereContactLinearAllocator);
}

ST_BoxContact* sphereTraceLinearAllocatorAllocateBoxContact()
{
	return sphereTraceLinearAllocatorAllocateObject(&boxContactLinearAllocator);
}

ST_Index sphereTraceLinearAllocatorGetBoxContactCount()
{
	return boxContactLinearAllocator.numObjects;
}

ST_BoxContact* sphereTraceLinearAllocatorGetBoxContactByIndex(ST_Index index)
{
	return sphereTraceLinearAllocatorGetByIndex(&boxContactLinearAllocator, index);
}

void sphereTraceLinearAllocatorResetBoxContacts()
{
	sphereTraceLinearAllocatorReset(&boxContactLinearAllocator);
}


ST_IndexList* sphereTraceAllocatorAllocateIndexListArray()
{
 	ST_IndexList* pil = sphereTraceAllocatorAllocateObject(&listArrayAllocator);
	for (int i = 0; i < listArraySize; i++)
		pil[i] = sphereTraceIndexListConstruct();
	return pil;
}
void sphereTraceAllocatorFreeIndexListArray(void* pArr)
{
	sphereTraceAllocatorFreeObject(&listArrayAllocator, pArr);
}

void sphereTraceAllocatorIndexListArrayResize(ST_Index size)
{
	sphereTraceAllocatorDelete(&listArrayAllocator);
	listArrayAllocator = sphereTraceAllocatorConstruct(ST_COLLIDER_ALLOCATOR_SIZE, sizeof(ST_IndexList) * size, ST_INDEX_ALLOCATOR_DEFAULT_SIZE);
	listArraySize = size;
}

#ifdef ST_DEBUG
ST_Index sphereTraceAllocatorGetBytesAllocated()
{
	return bytesAllocated;
}
#endif