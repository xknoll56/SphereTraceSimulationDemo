#include "SphereTraceHashTable.h"

ST_HashTable sphereTraceHashTableConstruct(ST_Index count, ST_Index (*hashFunction)())
{
	ST_HashTable table;
	table.count = count;
	table.lists = (ST_IndexList*)malloc(count * sizeof(ST_IndexList));
	table.hashFunction = hashFunction;
	return table;
}

void sphereTraceHashTableAddUnique(ST_HashTable* const pHashTable, ST_Index id)
{

}

void sphereTraceHashTableRemove(ST_HashTable* const pHashTable, ST_Index id)
{

}