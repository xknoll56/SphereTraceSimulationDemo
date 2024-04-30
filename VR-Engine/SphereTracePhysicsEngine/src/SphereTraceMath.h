#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include <float.h>
#include <stdio.h>
#include <stdint.h>

typedef int b32;

typedef char b8;

typedef struct ST_Vector2
{
	float x;
	float y;
} ST_Vector2;

typedef struct ST_Vector2Integer
{
	int x;
	int y;
} ST_Vector2Integer;

typedef struct ST_Vector3
{
	float x;
	float y;
	float z;
} ST_Vector3;

typedef struct ST_Vector4
{
	float x;
	float y;
	float z;
	float w;
} ST_Vector4;


typedef struct ST_Matrix4
{
	float m00, m01, m02, m03;
	float m10, m11, m12, m13;
	float m20, m21, m22, m23;
	float m30, m31, m32, m33;
} ST_Matrix4;

typedef struct ST_Quaternion
{
	float w;
	float x;
	float y;
	float z;
} ST_Quaternion;

typedef struct ST_Direction
{
	ST_Vector3 v;
	b32 normalized;
}ST_Direction;

typedef struct ST_Color
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
} ST_Color;


float sphereTraceLerp(float x0, float x1, float t);

float sphereTraceAbs(float f);

float sphereTraceMin(float a, float b);

float sphereTraceMax(float a, float b);

float sphereTraceSign(float f);

float sphereTraceRound(float f);

ST_Vector2 sphereTraceVector2Construct(float x, float y);

ST_Vector2 sphereTraceVector2ConstructWithVector3(ST_Vector3 v);

ST_Vector2Integer sphereTraceVector2IntegerConstruct(int x, int y);

ST_Vector3 sphereTraceVector3Construct(float x, float y, float z);

ST_Vector4 sphereTraceVector4Construct(float x, float y, float z, float w);

ST_Vector4 sphereTraceVector4ConstructWithVector3(ST_Vector3 v, float w);

ST_Vector3 sphereTraceVector3ConstructWithVector2(ST_Vector2 v, float z);

ST_Matrix4 sphereTraceMatrixConstruct(float m00, float m01, float m02, float m03,
	float m10, float m11, float m12, float m13,
	float m20, float m21, float m22, float m23,
	float m30, float m31, float m32, float m33);


ST_Vector4 sphereTraceMatrixRow(ST_Matrix4 const matrix, int i);

ST_Vector4 sphereTraceMatrixCol(ST_Matrix4 const matrix, int i);

ST_Vector2 sphereTraceVector2Add(ST_Vector2 v1, ST_Vector2 v2);

ST_Vector2 sphereTraceVector2Subtract(ST_Vector2 v1, ST_Vector2 v2);

float sphereTraceVector2Dot(ST_Vector2 v1, ST_Vector2 v2);

ST_Vector2 sphereTraceVector2Lerp(ST_Vector2 v1, ST_Vector2 v2, float t);

float sphereTraceVector2Length(ST_Vector2 v);

//ST_Vector2 sphereTraceVector2Cross()



ST_Vector2Integer sphereTraceVector2IntegerAdd(ST_Vector2Integer v1, ST_Vector2Integer v2);

ST_Vector2Integer sphereTraceVector2IntegerSubtract(ST_Vector2Integer v1, ST_Vector2Integer v2);


float sphereTraceDegreesToRadians(float degs);

b32 sphereTraceVector3Equal(ST_Vector3 v1, ST_Vector3 v2);

float sphereTraceVector3Length(ST_Vector3 v);

float sphereTraceVector3Dot(ST_Vector3 v1, ST_Vector3 v2);

float sphereTraceVector3AbsoluteDot(ST_Vector3 v1, ST_Vector3 v2);

float sphereTraceVector4Dot(ST_Vector4 v1, ST_Vector4 v2);

ST_Vector3 sphereTraceVector3Scale(ST_Vector3 v, float f);

ST_Vector3 sphereTraceVector3Mult(ST_Vector3 v1, ST_Vector3 v2);

void sphereTraceVector3ScaleByRef(ST_Vector3* const pRef, float f);

ST_Vector3 sphereTraceVector3Normalize(ST_Vector3 v);

void sphereTraceVector3NormalizeByRef(ST_Vector3* const pRef);

ST_Vector3 sphereTraceVector3Cross(ST_Vector3 v1, ST_Vector3 v2);

ST_Vector3 sphereTraceVector3Add(ST_Vector3 v1, ST_Vector3 v2);

void sphereTraceVector3AddByRef(ST_Vector3* const pRef, ST_Vector3 v);

ST_Vector3 sphereTraceVector3Subtract(ST_Vector3 v1, ST_Vector3 v2);

ST_Vector3 sphereTraceVector3SubtractAbsolute(ST_Vector3 v1, ST_Vector3 v2);

void sphereTraceVector3SubtractByRef(ST_Vector3* const pRef, ST_Vector3 v);

ST_Vector3 sphereTraceVector3Negative(ST_Vector3 v);

void sphereTraceVector3NegativeByRef(ST_Vector3* pRef);

void sphereTraceVector3Print(ST_Vector3 v);

void sphereTraceVector2Print(ST_Vector2 v);

b32 sphereTraceEpsilonEqual(float f1, float f2, float epsilon);

b32 sphereTraceVector3EpsilonEquals(ST_Vector3 v1, ST_Vector3 v2, float epsilon);

b32 sphereTraceVector4EpsilonEquals(ST_Vector4 v1, ST_Vector4 v2, float epsilon);

ST_Vector4 sphereTraceVector4ColorSetAlpha(ST_Vector4 color, float alpha);

ST_Vector3 sphereTraceVector3CopySign(ST_Vector3 v, float f);

float sphereTraceVector3Length2(ST_Vector3 vec);

ST_Vector3 sphereTraceVector3AddAndScale(ST_Vector3 toAdd, ST_Vector3 toScale, float scale);

ST_Vector3 sphereTraceVector3AddAndScale2(ST_Vector3 toAdd, ST_Vector3 toScale1, float scale1, ST_Vector3 toScale2, float scale2);

ST_Vector3 sphereTraceVector3AddAndScale3(ST_Vector3 toAdd, ST_Vector3 toScale1, float scale1, ST_Vector3 toScale2, float scale2, ST_Vector3 toScale3, float scale3);

void sphereTraceVector3AddAndScaleByRef(ST_Vector3* const pRef, ST_Vector3 toScale, float scale);

void sphereTraceVector3AddAndScale2ByRef(ST_Vector3* const pRef, ST_Vector3 toScale1, float scale1, ST_Vector3 toScale2, float scale2);

ST_Vector3 sphereTraceVector3Average(ST_Vector3 v1, ST_Vector3 v2);

b32 sphereTraceVector3Nan(ST_Vector3 vec);

b32 sphereTraceVector3NanAny(ST_Vector3 vec);

b32 sphereTraceVector3AnyGreaterThan(ST_Vector3 vec, float val);

ST_Vector3 sphereTraceClosestPointOnLineBetweenTwoLines(ST_Vector3 point, ST_Vector3 lineDir, ST_Vector3 otherPoint, ST_Vector3 otherLineDir);

b32 sphereTraceVector3ClosestPointOnLineBetweenTwoLinesIsGreaterThanZeroAndLessThanMaxDist(ST_Vector3 point, ST_Vector3 normalizedLineDir, ST_Vector3 otherPoint, ST_Vector3 otherNormalizedLineDir, float maxDist);

ST_Vector3 sphereTraceMathClosestPointOnLineNearestToPoint(ST_Vector3 linePoint, ST_Vector3 lineDir, ST_Vector3 point);

void sphereTraceMathClosestPointOnLineNearestToPointExtractData(ST_Vector3 linePoint, ST_Vector3 lineDir, ST_Vector3 point, ST_Vector3* pPoint, ST_Vector3* pDir, float* pClosestDist);

float sphereTraceMathCircleIntersectLine(float circleRadius, float slope, float xIntersept);

float sphereTraceMathCircleIntersectPoint(float circleRadius, float pointDistance, float pointHeight);

float sphereTraceMathCircleIntersectLineWithPoints(float circleRadius, float yIntersect, float x1, float y1, float x2, float y2);

float sphereTraceVector3ClosestPointOnLineBetweenTwoLinesDistance(ST_Vector3 point, ST_Vector3 normalizedLineDir, ST_Vector3 otherPoint, ST_Vector3 otherNormalizedLineDir);

void sphereTraceVector3ClosestPointOnLineBetweenTwoLinesDistancesOnLines(ST_Vector3 point, ST_Vector3 normalizedLineDir, ST_Vector3 otherPoint, ST_Vector3 otherNormalizedLineDir, float* dist1, float* dist2);

void sphereTraceVector3ClosestPointsOnLineBetweenTwoLines(ST_Vector3 point, ST_Vector3 lineDir, ST_Vector3 otherPoint, ST_Vector3 otherLineDir, ST_Vector3* result1, ST_Vector3* result2);

float sphereTraceVector3Distance(ST_Vector3 point1, ST_Vector3 point2);

float sphereTraceVector3HorizontalDistance(ST_Vector3 point1, ST_Vector3 point2);

ST_Vector3 sphereTraceVector3Lerp(ST_Vector3 point1, ST_Vector3 point2, float t);

ST_Vector3 sphereTraceVector3UniformSize(float size);

ST_Vector4 sphereTraceVector4Lerp(ST_Vector4 point1, ST_Vector4 point2, float t);

ST_Vector3 sphereTraceNormalizeBetweenPoints(ST_Vector3 to, ST_Vector3 from);

ST_Vector3 sphereTraceVector3ProjectVector3OntoPlane(ST_Vector3 vec, ST_Direction planeNormal);

b32 sphereTraceVector3IsVectorPositiveInDirection(ST_Vector3 vec, ST_Direction direction);

ST_Matrix4 sphereTraceMatrixIdentity();

ST_Matrix4 sphereTraceMatrixRotateX(float rad);

ST_Matrix4 sphereTraceMatrixRotateY(float rad);

ST_Matrix4 sphereTraceMatrixRotateZ(float rad);

ST_Matrix4 sphereTraceMatrixTranspose(ST_Matrix4 mat);

ST_Matrix4 sphereTraceMatrixTranslation(ST_Vector3 trans);

ST_Matrix4 sphereTraceMatrixScale(ST_Vector3 scale);

ST_Matrix4 sphereTraceMatrixRotate(ST_Vector3 eulerAngles);

ST_Matrix4 sphereTraceMatrixTranspose(ST_Matrix4 mat);

ST_Matrix4 sphereTraceMatrixPerspective(float aspectRatio, float fovYRadians, float zNear, float zFar);

ST_Matrix4 sphereTraceMatrixOrthographic(float left, float right, float top, float bottom, float nearf, float farf, float w);

ST_Matrix4 sphereTraceMatrixLookAt(ST_Vector3 eye, ST_Vector3 at, ST_Vector3 up);

ST_Matrix4 sphereTraceMatrixMult(ST_Matrix4 mat1, ST_Matrix4 mat2);

ST_Matrix4 sphereTraceMatrixMultByRef(const ST_Matrix4* pMat1, const ST_Matrix4* pMat2);

void sphereTraceMatrixPrint(ST_Matrix4 mat);

ST_Vector3 sphereTraceVector3GetLocalXAxisFromRotationMatrix(ST_Matrix4 mat);

ST_Vector3 sphereTraceVector3GetLocalYAxisFromRotationMatrix(ST_Matrix4 mat);

ST_Vector3 sphereTraceVector3GetLocalZAxisFromRotationMatrix(ST_Matrix4 mat);

ST_Direction sphereTraceDirectionGetLocalXAxisFromRotationMatrix(ST_Matrix4 mat);
   						
ST_Direction sphereTraceDirectionGetLocalYAxisFromRotationMatrix(ST_Matrix4 mat);
   						
ST_Direction sphereTraceDirectionGetLocalZAxisFromRotationMatrix(ST_Matrix4 mat);

void sphereTraceMatrixSetLocalXAxisOfRotationMatrix(ST_Matrix4* const mat, ST_Vector3 xAxis);

void sphereTraceMatrixSetLocalYAxisOfRotationMatrix(ST_Matrix4* const mat, ST_Vector3 yAxis);

void sphereTraceMatrixSetLocalZAxisOfRotationMatrix(ST_Matrix4* const mat, ST_Vector3 zAxis);

ST_Vector4 sphereTraceMatrixVector4Mult(ST_Matrix4 mat, ST_Vector4 vec);

ST_Vector3 sphereTraceMatrixVector3Mult(ST_Matrix4 mat, ST_Vector3 vec);

ST_Vector4 sphereTraceVector4MatrixMult(ST_Vector4 vec, ST_Matrix4 mat);

//ST_Vector3 sphereTrace

ST_Quaternion sphereTraceQuaternionConstruct(float w, float x, float y, float z);


ST_Quaternion sphereTraceQuaternionConjugate(ST_Quaternion quat);

void sphereTraceQuaternionConjugateByRef(ST_Quaternion* const pRef);

ST_Matrix4 sphereTraceMatrixFromQuaternion(ST_Quaternion quat);

ST_Quaternion sphereTraceQuaternionFromAngleAxis(ST_Vector3 axis, float angle);

ST_Quaternion sphereTraceQuaternionFromEulerAngles(ST_Vector3 eulerAngles);

ST_Quaternion sphereTraceQuaternionNormalize(ST_Quaternion quat);

void sphereTraceQuaternionNormalizeByRef(ST_Quaternion* const pRef);

ST_Quaternion sphereTraceQuaternionMultiply(ST_Quaternion a, ST_Quaternion b);

ST_Quaternion sphereTraceQuaternionAdd(ST_Quaternion a, ST_Quaternion b);

ST_Quaternion sphereTraceQuaternionSubtract(ST_Quaternion a, ST_Quaternion b);

ST_Quaternion sphereTraceQuaternionScale(float f, ST_Quaternion a);

ST_Quaternion sphereTraceQuaternionLookAt(ST_Vector3 eye, ST_Vector3 at, ST_Vector3 up);

void sphereTraceQuaternionPrint(ST_Quaternion quat);

ST_Vector3 sphereTraceVector3RotatePoint(ST_Vector3 point, ST_Quaternion rotation);

ST_Direction sphereTraceDirectionRotateDir(ST_Direction dir, ST_Quaternion rotation);

ST_Matrix4 sphereTraceMatrixConstructFromRightForwardUp(ST_Vector3 right, ST_Vector3 up, ST_Vector3 forward);

ST_Quaternion sphereTraceMatrixQuaternionFromRotationMatrix(ST_Matrix4 mat);

ST_Direction sphereTraceDirectionConstruct(ST_Vector3 vec, b32 normalized);

ST_Direction sphereTraceDirectionConstruct1(float x, float y, float z, b32 normalized);

ST_Direction sphereTraceDirectionAdd(ST_Direction dir1, ST_Direction dir2);

ST_Direction sphereTraceDirectionAdd2(ST_Direction dir1, ST_Direction dir2, ST_Direction dir3, b32 normalize);

ST_Direction sphereTraceDirectionConstructNormalized(ST_Vector3 vec);

void sphereTraceDirectionNormalizeIfNotNormalizedByRef(ST_Direction* const dir);

ST_Direction sphereTraceDirectionNormalizeIfNotNormalized(ST_Direction dir);

ST_Direction sphereTraceDirectionNegative(ST_Direction dir);

ST_Direction sphereTraceDirectionProjectDirectionOntoPlane(ST_Direction dir, ST_Direction planeNormal);

float sphereTraceDirectionGetDistanceInDirection(ST_Direction dir, ST_Vector3 from, ST_Vector3 to);

float sphereTraceDirectionGetMagnitudeInDirection(ST_Direction dir, ST_Vector3 v);

ST_Color sphereTraceColorConstruct(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

ST_Vector4 sphereTraceVector4FromColor(ST_Color color);