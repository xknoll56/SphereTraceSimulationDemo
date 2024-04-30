

#include "SphereTraceLists.h"
#include "SphereTraceMath.h"
#include "SphereTraceGlobals.h"
#include "SphereTraceAllocator.h"

ST_IndexList sphereTraceIndexListConstruct()
{
	ST_IndexList intList;
	intList.count = 0;
	intList.pFirst = NULL;
	return intList;
}

void sphereTraceIndexListAddFirst(ST_IndexList* const pIntList, ST_Index value)
{
	ST_IndexListData* pFirst = pIntList->pFirst;
	pIntList->pFirst = sphereTraceAllocatorAllocateIndexListData();
	pIntList->pFirst->value = value;
	pIntList->pFirst->pNext = pFirst;
	pIntList->count++;

}

void sphereTraceIndexListAddLast(ST_IndexList* const pIntList, ST_Index value)
{
	if (pIntList->pFirst == NULL)
	{
		pIntList->pFirst = sphereTraceAllocatorAllocateIndexListData();
		pIntList->pFirst->value = value;
		pIntList->pFirst->pNext = NULL;
		pIntList->count++;
	}
	else
	{
		ST_IndexListData* pData = pIntList->pFirst;
		for (int i = 0; i < pIntList->count - 1; i++)
		{
			pData = pData->pNext;
		}
		pData->pNext = sphereTraceAllocatorAllocateIndexListData();
		pData->pNext->value = value;
		pData->pNext->pNext = NULL;
		pIntList->count++;
	}
}

b32 sphereTraceIndexListAddUnique(ST_IndexList* const pIntList, ST_Index value)
{
	if (pIntList->pFirst == NULL)
	{
		pIntList->pFirst = sphereTraceAllocatorAllocateIndexListData();
		pIntList->pFirst->value = value;
		pIntList->pFirst->pNext = NULL;
		pIntList->count++;
		return 1;
	}
	else
	{
		ST_IndexListData* pData = pIntList->pFirst;
		for (int i = 0; i < pIntList->count - 1; i++)
		{
			if (pData != NULL)
			{
				if (pData->value == value)
					return 0;
			}
			pData = pData->pNext;
		}
		if (pData != NULL)
		{
			if (pData->value == value)
				return 0;
		}
		pData->pNext = sphereTraceAllocatorAllocateIndexListData();
		pData->pNext->value = value;
		pData->pNext->pNext = NULL;
		pIntList->count++;
		return 1;
	}
}

void sphereTraceIndexListRemoveFirstInstance(ST_IndexList* const pIntList, ST_Index value)
{
	if (pIntList->count == 1)
	{
		if (pIntList->pFirst->value == value)
		{
			sphereTraceAllocatorFreeIndexListData(pIntList->pFirst);
			pIntList->pFirst = NULL;
			pIntList->count--;
			return;
		}
	}
	else if (pIntList->count > 1)
	{
		ST_IndexListData* pData = pIntList->pFirst;
		ST_IndexListData* pNext = pIntList->pFirst->pNext;
		if (pIntList->pFirst->value == value)
		{
			sphereTraceAllocatorFreeIndexListData(pIntList->pFirst);
			pIntList->pFirst = pNext;
			pIntList->count--;
			return;
		}
		for (int i = 0; i < pIntList->count - 1; i++)
		{
			if (pNext->value == value)
			{
				pData->pNext = pNext->pNext;
				sphereTraceAllocatorFreeIndexListData(pNext);
				pIntList->count--;
				return;
			}
			pData = pNext;
			pNext = pNext->pNext;
		}
	}
}

void sphereTraceIndexListFree(ST_IndexList* const pIntList)
{
	ST_IndexListData* pData = pIntList->pFirst;
	ST_IndexListData* pNext;
	for (int i = 0; i < pIntList->count; i++)
	{
		if (pData != NULL)
		{
			pNext = pData->pNext;
			sphereTraceAllocatorFreeIndexListData(pData);
			pData = pNext;
		}
	}
	pIntList->pFirst = NULL;
	pIntList->count = 0;
}

void sphereTraceIndexListReset(ST_IndexList* const pIntList)
{
	sphereTraceIndexListFree(pIntList);
	*pIntList = sphereTraceIndexListConstruct();
}

b32 sphereTraceIndexListContains(const ST_IndexList* const pIntList, ST_Index value)
{
	ST_IndexListData* data = pIntList->pFirst;
	for (int i = 0; i < pIntList->count; i++)
	{
		if (data != NULL)
		{
			if (data->value == value)
				return 1;
			data = data->pNext;
		}
	}
	return 0;
}

ST_IndexList sphereTraceIndexListConstructForDeletedValues(const ST_IndexList* const pOldIntList, const ST_IndexList* const pNewIntList)
{
	ST_IndexList deletedIntList = sphereTraceIndexListConstruct();
	ST_IndexListData* data = pOldIntList->pFirst;
	for (int i = 0; i < pOldIntList->count; i++)
	{
		if (data != NULL)
		{
			if (!sphereTraceIndexListContains(pNewIntList, data->value))
				sphereTraceIndexListAddUnique(&deletedIntList, data->value);
			data = data->pNext;
		}
	}
	return deletedIntList;
}

void sphereTraceIndexListPrint(const ST_IndexList* const pIntList)
{
	ST_IndexListData* data = pIntList->pFirst;
	for (int i = 0; i < pIntList->count; i++)
	{
		printf("%i:%i,", i, data->value);
		data = data->pNext;
	}
	printf("\n");
}

b32 sphereTraceIndexListEqual(ST_IndexList* const pIntListA, ST_IndexList* const pIntListB)
{
	if (pIntListA->count == pIntListB->count)
	{
		ST_IndexListData* pild = pIntListA->pFirst;
		for (int i = 0; i < pIntListA->count; i++)
		{
			if (!sphereTraceIndexListContains(pIntListB, pild->value))
				return ST_FALSE;
			pild = pild->pNext;
		}
		return ST_TRUE;
	}
	return ST_FALSE;
}

void sphereTraceSortedIndexListAdd(ST_IndexList* const pIntList, ST_Index value)
{
	if (pIntList->pFirst == NULL)
	{
		pIntList->pFirst = sphereTraceAllocatorAllocateIndexListData();
		pIntList->pFirst->value = value;
		pIntList->pFirst->pNext = NULL;
		pIntList->count++;
	}
	else
	{
		ST_IndexListData* pData = pIntList->pFirst;
		ST_IndexListData* pDataPrev = pIntList->pFirst;
		int index;
		for (index=0; index < pIntList->count; index++)
		{
			if (pData->value > value)
			{
				break;
			}
			pDataPrev = pData;
			pData = pData->pNext;
		}
		if (index == 0)
		{
			pIntList->pFirst = sphereTraceAllocatorAllocateIndexListData();
			pIntList->pFirst->value = value;
			pIntList->pFirst->pNext = pData;
		}
		else
		{
			pDataPrev->pNext = sphereTraceAllocatorAllocateIndexListData();
			pDataPrev->pNext->value = value;
			pDataPrev->pNext->pNext = pData;
		}
		pIntList->count++;
	}
}
b32 sphereTraceSortedIndexListAddUnique(ST_IndexList* const pIntList, ST_Index value)
{
	if (pIntList->pFirst == NULL)
	{
		pIntList->pFirst = sphereTraceAllocatorAllocateIndexListData();
		pIntList->pFirst->value = value;
		pIntList->pFirst->pNext = NULL;
		pIntList->count++;
		return 1;
	}
	else
	{
		ST_IndexListData* pData = pIntList->pFirst;
		ST_IndexListData* pDataPrev = pIntList->pFirst;
		int index;
		for (index = 0; index < pIntList->count; index++)
		{
			if (pData->value == value)
			{
				return 0;
			}
			else if (pData->value > value)
			{
				break;
			}
			pDataPrev = pData;
			pData = pData->pNext;
		}
		if (index == 0)
		{
			pIntList->pFirst = sphereTraceAllocatorAllocateIndexListData();
			pIntList->pFirst->value = value;
			pIntList->pFirst->pNext = pData;
		}
		else
		{
			pDataPrev->pNext = sphereTraceAllocatorAllocateIndexListData();
			pDataPrev->pNext->value = value;
			pDataPrev->pNext->pNext = pData;
		}
		pIntList->count++;
		return 1;
	}
}

void sphereTraceSortedIndexListMergeUnique(ST_IndexList* const pIntListMergeFrom, ST_IndexList* const pIntListMergeTo)
{
	ST_IndexListData* pild = pIntListMergeFrom->pFirst;
	for (int i = 0; i < pIntListMergeFrom->count; i++)
	{
		sphereTraceSortedIndexListAddUnique(pIntListMergeTo, pild->value);
		pild = pild->pNext;
	}
}
void sphereTraceSortedIndexListRemove(ST_IndexList* const pIntList, ST_Index value)
{
	if (pIntList->count == 1)
	{
		if (pIntList->pFirst->value == value)
		{
			sphereTraceAllocatorFreeIndexListData(pIntList->pFirst);
			pIntList->pFirst = NULL;
			pIntList->count--;
			return;
		}
	}
	else if (pIntList->count > 1)
	{
		ST_IndexListData* pData = pIntList->pFirst;
		ST_IndexListData* pNext = pIntList->pFirst->pNext;
		if (pIntList->pFirst->value == value)
		{
			sphereTraceAllocatorFreeIndexListData(pIntList->pFirst);
			pIntList->pFirst = pNext;
			pIntList->count--;
			return;
		}
		else if (pIntList->pFirst->value > value)
		{
			return;
		}
		for (int i = 0; i < pIntList->count-1; i++)
		{
			if (pNext->value == value)
			{
				pData->pNext = pNext->pNext;
				sphereTraceAllocatorFreeIndexListData(pNext);
				pIntList->count--;
				return;
			}
			else if (pNext->value > value)
			{
				return;
			}
			pData = pNext;
			pNext = pNext->pNext;
		}
	}
}
b32 sphereTraceSortedIndexListContains(const ST_IndexList* const pIntList, ST_Index value)
{
	ST_IndexListData* pild = pIntList->pFirst;
	for (int i = 0; i < pIntList->count; i++)
	{
		if (pild->value == value)
		{
			return 1;
		}
		else if (pild->value > value)
		{
			return 0;
		}
		pild = pild->pNext;
	}
	return 0;
}

/// <summary>
/// key value list
/// </summary>
/// <returns></returns>
ST_KeyValueList sphereTraceKeyValueListConstruct()
{
	ST_KeyValueList list;
	list.count = 0;
	list.pFirst = NULL;
	return list;
}


b32 sphereTraceKeyValueListAdd(ST_KeyValueList* const pIntList, ST_Index key, ST_Index value)
{
	if (pIntList->pFirst == NULL)
	{
		pIntList->pFirst = sphereTraceAllocatorAllocateKeyValueListData();
		pIntList->pFirst->key = key;
		pIntList->pFirst->value = value;
		pIntList->pFirst->pNext = NULL;
		pIntList->count++;
		return 1;
	}
	else
	{
		ST_KeyValueListData* pData = pIntList->pFirst;
		for (int i = 0; i < pIntList->count - 1; i++)
		{
			if (pData != NULL)
			{
				if (pData->key == key)
					return 0;
			}
			pData = pData->pNext;
		}
		if (pData != NULL)
		{
			if (pData->key == key)
				return 0;
		}
		pData->pNext = sphereTraceAllocatorAllocateKeyValueListData();
		pData->pNext->key = key;
		pData->pNext->value = value;
		pData->pNext->pNext = NULL;
		pIntList->count++;
		return 1;
	}
}

void sphereTraceKeyValueListRemoveKey(ST_KeyValueList* const pIntList, ST_Index key)
{
	if (pIntList->count == 1)
	{
		if (pIntList->pFirst->key == key)
		{
			sphereTraceAllocatorFreeKeyValueListData(pIntList->pFirst);
			pIntList->pFirst = NULL;
			pIntList->count--;
			return;
		}
	}
	else if (pIntList->count > 1)
	{
		ST_KeyValueListData* pData = pIntList->pFirst;
		ST_KeyValueListData* pNext = pIntList->pFirst->pNext;
		if (pIntList->pFirst->key == key)
		{
			sphereTraceAllocatorFreeKeyValueListData(pIntList->pFirst);
			pIntList->pFirst = pNext;
			pIntList->count--;
			return;
		}
		for (int i = 0; i < pIntList->count - 1; i++)
		{
			if (pNext->key == key)
			{
				pData->pNext = pNext->pNext;
				sphereTraceAllocatorFreeKeyValueListData(pNext);
				pIntList->count--;
				return;
			}
			pData = pNext;
			pNext = pNext->pNext;
		}
	}
}

void sphereTraceKeyValueListFree(ST_KeyValueList* const pIntList)
{
	ST_KeyValueListData* pData = pIntList->pFirst;
	ST_KeyValueListData* pNext;
	for (int i = 0; i < pIntList->count; i++)
	{
		if (pData != NULL)
		{
			pNext = pData->pNext;
			sphereTraceAllocatorFreeKeyValueListData(pData);
			pData = pNext;
		}
	}
	pIntList->pFirst = NULL;
	pIntList->count = 0;
}

b32 sphereTraceKeyValueListContainsKey(const ST_KeyValueList* const pIntList, ST_Index key)
{
	ST_KeyValueListData* data = pIntList->pFirst;
	for (int i = 0; i < pIntList->count; i++)
	{
		if (data != NULL)
		{
			if (data->key == key)
				return 1;
			data = data->pNext;
		}
	}
	return 0;
}

void sphereTraceKeyValueListPrint(const ST_KeyValueList* const pIntList)
{
	ST_KeyValueListData* data = pIntList->pFirst;
	for (int i = 0; i < pIntList->count; i++)
	{
		printf("%i, key: %i, value: %i", i, data->key, data->value);
		data = data->pNext;
	}
	printf("\n");
}


b32 sphereTraceSortedKeyValueListAddUnique(ST_KeyValueList* const pIntList, ST_Index key, ST_Index value)
{
	if (pIntList->pFirst == NULL)
	{
		pIntList->pFirst = sphereTraceAllocatorAllocateKeyValueListData();
		pIntList->pFirst->key = key;
		pIntList->pFirst->value = value;
		pIntList->pFirst->pNext = NULL;
		pIntList->count++;
		return 1;
	}
	else
	{
		ST_KeyValueListData* pData = pIntList->pFirst;
		ST_KeyValueListData* pDataPrev = pIntList->pFirst;
		int index;
		for (index = 0; index < pIntList->count; index++)
		{
			if (pData->key == key)
			{
				return 0;
			}
			else if (pData->key > key)
			{
				break;
			}
			pDataPrev = pData;
			pData = pData->pNext;
		}
		if (index == 0)
		{
			pIntList->pFirst = sphereTraceAllocatorAllocateKeyValueListData();
			pIntList->pFirst->key = key;
			pIntList->pFirst->value = value;
			pIntList->pFirst->pNext = pData;
		}
		else
		{
			pDataPrev->pNext = sphereTraceAllocatorAllocateKeyValueListData();
			pDataPrev->pNext->key = key;
			pDataPrev->pNext->value = value;
			pDataPrev->pNext->pNext = pData;
		}
		pIntList->count++;
		return 1;
	}
}
void sphereTraceSortedKeyValueListRemoveKey(ST_KeyValueList* const pIntList, ST_Index key)
{
	if (pIntList->count == 1)
	{
		if (pIntList->pFirst->key == key)
		{
			sphereTraceAllocatorFreeKeyValueListData(pIntList->pFirst);
			pIntList->pFirst = NULL;
			pIntList->count--;
			return;
		}
	}
	else if (pIntList->count > 1)
	{
		ST_KeyValueListData* pData = pIntList->pFirst;
		ST_KeyValueListData* pNext = pIntList->pFirst->pNext;
		if (pIntList->pFirst->key == key)
		{
			sphereTraceAllocatorFreeKeyValueListData(pIntList->pFirst);
			pIntList->pFirst = pNext;
			pIntList->count--;
			return;
		}
		else if (pIntList->pFirst->key > key)
		{
			return;
		}
		for (int i = 0; i < pIntList->count - 1; i++)
		{
			if (pNext->key == key)
			{
				pData->pNext = pNext->pNext;
				sphereTraceAllocatorFreeKeyValueListData(pNext);
				pIntList->count--;
				return;
			}
			else if (pNext->key > key)
			{
				return;
			}
			pData = pNext;
			pNext = pNext->pNext;
		}
	}
}
b32 sphereTraceSortedKeyValueListContainsKey(const ST_KeyValueList* const pIntList, ST_Index key)
{
	ST_KeyValueListData* pild = pIntList->pFirst;
	for (int i = 0; i < pIntList->count; i++)
	{
		if (pild->key == key)
		{
			return 1;
		}
		else if (pild->key > key)
		{
			return 0;
		}
		pild = pild->pNext;
	}
	return 0;
}


ST_Vector3List sphereTraceVector3ListConstruct()
{
	ST_Vector3List vector3List;
	vector3List.count = 0;
	vector3List.pFirst = NULL;
	return vector3List;
}

void sphereTraceVector3ListAddLast(ST_Vector3List* const pVector3List, ST_Vector3 value)
{
	if (pVector3List->pFirst == NULL)
	{
		pVector3List->pFirst = sphereTraceAllocatorAllocateVector3ListData();
		pVector3List->pFirst->value = value;
		pVector3List->pFirst->pNext = NULL;
		pVector3List->count++;
	}
	else
	{
		ST_Vector3ListData* pData = pVector3List->pFirst;
		for (int i = 0; i < pVector3List->count - 1; i++)
		{
			pData = pData->pNext;
		}
		pData->pNext = sphereTraceAllocatorAllocateVector3ListData();
		pData->pNext->value = value;
		pData->pNext->pNext = NULL;
		pVector3List->count++;
	}
}

void sphereTraceVector3ListAddFirst(ST_Vector3List* const pVector3List, ST_Vector3 value)
{
	ST_Vector3ListData* pFirst = pVector3List->pFirst;
	pVector3List->pFirst = sphereTraceAllocatorAllocateVector3ListData();
	pVector3List->pFirst->value = value;
	pVector3List->pFirst->pNext = pFirst;
	pVector3List->count++;
}

void sphereTraceVector3ListRemoveFirst(ST_Vector3List* const pVector3List)
{
	if (pVector3List->pFirst != NULL)
	{
		ST_Vector3ListData* pNext = pVector3List->pFirst->pNext;
		sphereTraceAllocatorFreeVector3ListData(pVector3List->pFirst);
		pVector3List->pFirst = pNext;
		pVector3List->count--;
	}
}

void sphereTraceVector3ListRemoveLast(ST_Vector3List* const pVector3List)
{
	if (pVector3List->count>1)
	{
		ST_Vector3ListData* pLast = pVector3List->pFirst;
		ST_Vector3ListData* pPrev = pVector3List->pFirst;
		for (int i = 0; i < pVector3List->count - 1; i++)
		{
			if (i > 0)
				pPrev = pLast;
			pLast = pLast->pNext;
		}
		sphereTraceAllocatorFreeVector3ListData(pLast);
		pPrev->pNext = NULL;
		pVector3List->count--;
	}
}

b32 sphereTraceVector3ListAddUnique(ST_Vector3List* const pVector3List, ST_Vector3 value)
{
	if (pVector3List->pFirst == NULL)
	{
		pVector3List->pFirst = sphereTraceAllocatorAllocateVector3ListData();
		pVector3List->pFirst->value = value;
		pVector3List->pFirst->pNext = NULL;
		pVector3List->count++;
		return 1;
	}
	else
	{
		ST_Vector3ListData* pData = pVector3List->pFirst;
		for (int i = 0; i < pVector3List->count - 1; i++)
		{
			if (pData != NULL)
			{
				if (sphereTraceVector3Equal(pData->value, value))
					return 0;
			}
			pData = pData->pNext;
		}
		if (pData != NULL)
		{
			if (sphereTraceVector3Equal(pData->value, value))
				return 0;
		}
		pData->pNext = sphereTraceAllocatorAllocateVector3ListData();
		pData->pNext->value = value;
		pData->pNext->pNext = NULL;
		pVector3List->count++;
		return 1;
	}
}

void sphereTraceVector3ListRemoveFirstInstance(ST_Vector3List* const pVector3List, ST_Vector3 value)
{
	if (pVector3List->count == 1)
	{
		if (sphereTraceVector3Equal(pVector3List->pFirst->value, value))
			sphereTraceAllocatorFreeVector3ListData(pVector3List->pFirst);
		pVector3List->pFirst = NULL;
		pVector3List->count--;
	}
	else if (pVector3List->count > 1)
	{
		ST_Vector3ListData* pData = pVector3List->pFirst;
		ST_Vector3ListData* pNext = pVector3List->pFirst->pNext;
		if (sphereTraceVector3Equal(pVector3List->pFirst->value, value))
		{
			sphereTraceAllocatorFreeVector3ListData(pVector3List->pFirst);
			pVector3List->pFirst = pNext;
			pVector3List->count--;
			return;
		}
		for (int i = 0; i < pVector3List->count - 1; i++)
		{
			if (sphereTraceVector3Equal(pNext->value, value))
			{
				pData->pNext = pNext->pNext;
				sphereTraceAllocatorFreeVector3ListData(pNext);
				pVector3List->count--;
				return;
			}
			pData = pNext;
			pNext = pNext->pNext;
		}
	}
}

void sphereTraceVector3ListFree(ST_Vector3List* const pVector3List)
{
	ST_Vector3ListData* pData = pVector3List->pFirst;
	ST_Vector3ListData* pNext;
	for (int i = 0; i < pVector3List->count; i++)
	{
		if (pData != NULL)
		{
			pNext = pData->pNext;
			sphereTraceAllocatorFreeVector3ListData(pData);
			pData = pNext;
		}
	}
	pVector3List->pFirst = NULL;
	pVector3List->count = 0;
}

b32 sphereTraceVector3ListContains(const ST_Vector3List* const pVector3List, ST_Vector3 value)
{
	ST_Vector3ListData* data = pVector3List->pFirst;
	for (int i = 0; i < pVector3List->count; i++)
	{
		if (data != NULL)
		{
			if (sphereTraceVector3Equal(data->value, value))
				return 1;
			data = data->pNext;
		}
	}
	return 0;
}


void sphereTraceVector3ListPrint(const ST_Vector3List* const pVector3List)
{
	ST_Vector3ListData* data = pVector3List->pFirst;
	for (int i = 0; i < pVector3List->count; i++)
	{
		printf("%i:%i,", i, data->value);
		data = data->pNext;
	}
	printf("\n");
}

ST_Vector3 sphereTraceVector3ListAverage(const ST_Vector3List* const pVector3List)
{
	ST_Vector3 avg = gVector3Zero;
	ST_Vector3ListData* pild = pVector3List->pFirst;
	for (int i = 0; i < pVector3List->count; i++)
	{
		avg = sphereTraceVector3Add(avg, pild->value);
		pild = pild->pNext;
	}
	return sphereTraceVector3Scale(avg, 1.0f/pVector3List->count);
}

void sphereTraceVector3ListMoveOffset(const ST_Vector3List* const pVector3List, ST_Vector3 offset)
{
	ST_Vector3ListData* pild = pVector3List->pFirst;
	for (int i = 0; i < pVector3List->count; i++)
	{
		sphereTraceVector3AddByRef(&pild->value, offset);
		pild = pild->pNext;
	}
}