#include "SphereTraceTag.h"
#include <string.h>

ST_Tag sphereTraceTagConstruct(const char* tag)
{
	ST_Tag ret;
	rsize_t len = strlen(tag);
	memcpy(ret.data, tag, len);
	ret.data[len] = '\0';
	ret.tagLength = len;
	return ret;
}
