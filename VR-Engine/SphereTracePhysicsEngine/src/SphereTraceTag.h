#pragma once
#include <stdlib.h>

typedef uintptr_t ST_Index;

#define ST_TAG_SIZE 64
typedef struct ST_Tag
{
	char data[ST_TAG_SIZE];
	ST_Index tagLength;
} ST_Tag;


ST_Tag sphereTraceTagConstruct(const char* tag);
