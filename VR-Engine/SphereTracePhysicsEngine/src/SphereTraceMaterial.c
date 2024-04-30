#include "SphereTraceMaterial.h"

ST_Material sphereTraceMaterialConstruct(float restitution, float staticFriction, float kineticFriction)
{
	ST_Material material;
	material.restitution = restitution;
	material.staticFriction = staticFriction;
	material.kineticFriction = kineticFriction;
	return material;
}