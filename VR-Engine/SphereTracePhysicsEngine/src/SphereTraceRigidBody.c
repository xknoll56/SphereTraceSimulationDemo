#include "SphereTraceLists.h"
#include "SphereTraceRigidBody.h"
#include "SphereTraceMath.h"
#include "SphereTraceGlobals.h"

ST_RigidBody sphereTraceRigidBodyConstruct(float mass, float inertia)
{
    ST_RigidBody rigidBody;
    rigidBody.mass = mass;
    rigidBody.inertia = inertia;
    rigidBody.position = sphereTraceVector3Construct(0.0f, 0.0f, 0.0f);
    rigidBody.prevPosition = sphereTraceVector3Construct(0.0f, 0.0f, 0.0f);
    rigidBody.rotation = sphereTraceQuaternionFromEulerAngles(sphereTraceVector3Construct(0.0f, 0.0f, 0.0f));
    rigidBody.rotationMatrix = sphereTraceMatrixFromQuaternion(rigidBody.rotation);
    rigidBody.linearMomentum = sphereTraceVector3Construct(0.0f, 0.0f, 0.0f);
    rigidBody.velocity = sphereTraceVector3Construct(0.0f, 0.0f, 0.0f);
    rigidBody.angularMomentum = sphereTraceVector3Construct(0.0f, 0.0f, 0.0f);
    rigidBody.angularVelocity = sphereTraceVector3Construct(0.0f, 0.0f, 0.0f);
    rigidBody.appliedForces = sphereTraceVector3ListConstruct();
    rigidBody.appliedDeltaMomentums = sphereTraceVector3ListConstruct();
    rigidBody.appliedTorques = sphereTraceVector3ListConstruct();
    rigidBody.appliedDeltaAngularMomentums = sphereTraceVector3ListConstruct();
    rigidBody.isAsleep = ST_FALSE;
    return rigidBody;
}

//ST_Vector3 sphereTraceRigidBodyInertialInverse(ST_RigidBody* const pRigidBody)
//{
//    return sphereTraceVector3Construct(1.0f / pRigidBody->inertia.x, 1.0f / pRigidBody->inertia.y, 1.0f / pRigidBody->inertia.z);
//}

void sphereTraceRigidBodyAddForce(ST_RigidBody* const pRigidBody, const ST_Vector3 force)
{
    pRigidBody->isAsleep = ST_FALSE;
    sphereTraceVector3ListAddFirst(&pRigidBody->appliedForces, force);
}

void sphereTraceRigidBodyApplyForces(ST_RigidBody* const pRigidBody, float dt)
{
    //apply forces
    int count = pRigidBody->appliedForces.count;
    for (int i = 0; i < count; i++)
    {
        pRigidBody->linearMomentum = sphereTraceVector3AddAndScale(pRigidBody->linearMomentum, pRigidBody->appliedForces.pFirst->value, dt);
        sphereTraceVector3ListRemoveFirst(&pRigidBody->appliedForces);
    }
}

void sphereTraceRigidBodyAddDeltaMomentum(ST_RigidBody* const pRigidBody, const ST_Vector3 dp)
{
    pRigidBody->isAsleep = ST_FALSE;
    sphereTraceVector3ListAddFirst(&pRigidBody->appliedDeltaMomentums, dp);
}

void sphereTraceRigidBodyApplyDeltaMomentums(ST_RigidBody* const pRigidBody)
{
    //apply dps
    int count = pRigidBody->appliedDeltaMomentums.count;
    for (int i = 0; i < count; i++)
    {
        pRigidBody->linearMomentum = sphereTraceVector3Add(pRigidBody->linearMomentum, pRigidBody->appliedDeltaMomentums.pFirst->value);
        sphereTraceVector3ListRemoveFirst(&pRigidBody->appliedDeltaMomentums);
    }
}

void sphereTraceRigidBodyAddTorque(ST_RigidBody* const pRigidBody, const ST_Vector3 torque)
{
    pRigidBody->isAsleep = ST_FALSE;
    sphereTraceVector3ListAddFirst(&pRigidBody->appliedTorques, torque);
}
void sphereTraceRigidBodyApplyTorques(ST_RigidBody* const pRigidBody, float dt)
{
    int count = pRigidBody->appliedTorques.count;
    for (int i = 0; i < count; i++)
    {
        pRigidBody->angularMomentum = sphereTraceVector3Add(pRigidBody->angularMomentum, sphereTraceVector3Scale(pRigidBody->appliedTorques.pFirst->value, dt));
        sphereTraceVector3ListRemoveFirst(&pRigidBody->appliedTorques);
    }

}

void sphereTraceRigidBodyAddDeltaAngularMomentum(ST_RigidBody* const pRigidBody, const ST_Vector3 dl)
{
    pRigidBody->isAsleep = ST_FALSE;
    sphereTraceVector3ListAddFirst(&pRigidBody->appliedDeltaAngularMomentums, dl);
}

void sphereTraceRigidBodyApplyDeltaAngularMomentums(ST_RigidBody* const pRigidBody)
{
    int count = pRigidBody->appliedDeltaAngularMomentums.count;
    for (int i = 0; i < count; i++)
    {
        pRigidBody->angularMomentum = sphereTraceVector3Add(pRigidBody->angularMomentum, pRigidBody->appliedDeltaAngularMomentums.pFirst->value);
        sphereTraceVector3ListRemoveFirst(&pRigidBody->appliedDeltaAngularMomentums);
    }
}


void sphereTraceRigidBodySetVelocity(ST_RigidBody* const pRigidBody, const ST_Vector3 velocity)
{
    pRigidBody->linearMomentum = sphereTraceVector3Scale(velocity, pRigidBody->mass);
    pRigidBody->velocity = velocity;
}

void sphereTraceRigidBodySetAngularVelocity(ST_RigidBody* const pRigidBody, const ST_Vector3 angularVelocity)
{
    pRigidBody->angularMomentum = sphereTraceVector3Scale(angularVelocity, pRigidBody->inertia);
    pRigidBody->angularVelocity = angularVelocity;
}

float sphereTraceRigidBodyGetSpeed(ST_RigidBody* const pRigidBody)
{
    return sqrtf(sphereTraceVector3Dot(pRigidBody->velocity, pRigidBody->velocity));
}

float sphereTraceRigidBodySetSpeed(ST_RigidBody* const pRigidBody, float speed)
{
    sphereTraceRigidBodySetVelocity(pRigidBody, 
        sphereTraceVector3Scale(pRigidBody->velocity, speed / sphereTraceRigidBodyGetSpeed(pRigidBody)));
}

void sphereTraceRigidBodySetRotation(ST_RigidBody* const pRigidBody, ST_Quaternion rotation)
{
    pRigidBody->rotation = rotation;
    pRigidBody->rotationMatrix = sphereTraceMatrixFromQuaternion(rotation);
}

void sphereTraceRigidBodyResetMomentum(ST_RigidBody* const pRigidBody)
{
    pRigidBody->linearMomentum = gVector3Zero;
}

void sphereTraceRigidBodyResetAngularMomentum(ST_RigidBody* const pRigidBody)
{
    pRigidBody->angularMomentum = gVector3Zero;
}

void sphereTraceRigidBodyRotate(ST_RigidBody* const pRigidBody, const ST_Quaternion rotation)
{
    pRigidBody->rotation = sphereTraceQuaternionMultiply(rotation, pRigidBody->rotation);
    pRigidBody->rotation = sphereTraceQuaternionNormalize(pRigidBody->rotation);
    pRigidBody->rotationMatrix = sphereTraceMatrixFromQuaternion(pRigidBody->rotation);
}

void sphereTraceRigidBodyRotateAroundPoint(ST_RigidBody* const pRigidBody, ST_Vector3 point, const ST_Quaternion rotation)
{
    ST_Vector3 dp = sphereTraceVector3Subtract(pRigidBody->position, point);
    pRigidBody->position = sphereTraceVector3Add(point, sphereTraceVector3RotatePoint(dp, rotation));
    pRigidBody->rotation = sphereTraceQuaternionMultiply(rotation, pRigidBody->rotation);
    pRigidBody->rotation = sphereTraceQuaternionNormalize(pRigidBody->rotation);
    pRigidBody->rotationMatrix = sphereTraceMatrixFromQuaternion(pRigidBody->rotation);
}

void sphereTraceRigidBodyRotateAroundPointToSetRotation(ST_RigidBody* const pRigidBody, ST_Vector3 point, const ST_Quaternion newRotation)
{
    ST_Quaternion toRotation = sphereTraceQuaternionMultiply(pRigidBody->rotation, sphereTraceQuaternionConjugate(newRotation));
    sphereTraceRigidBodyRotateAroundPoint(pRigidBody, point, toRotation);
}

void sphereTraceRigidBodyClearDPDM(ST_RigidBody* const pRigidBody)
{
    sphereTraceVector3ListFree(&pRigidBody->appliedDeltaAngularMomentums);
    sphereTraceVector3ListFree(&pRigidBody->appliedDeltaMomentums);
}