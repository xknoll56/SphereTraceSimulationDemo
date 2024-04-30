#include "SphereTraceMath.h"

const ST_Vector2 gVector2Zero = { 0.0f, 0.0f };
const ST_Vector2 gVector2One = { 1.0f, 1.0f };
const ST_Vector2 gVector2Right = { 1.0f, 0.0f };
const ST_Vector2 gVector2Left = { -1.0f, 0.0f };
const ST_Vector2 gVector2Up = { 0.0f, 1.0f };
const ST_Vector2 gVector2Down = { 0.0f, -1.0f };
const ST_Vector3 gVector3Up = { 0.0f, 1.0f, 0.0f };
const ST_Vector3 gVector3Right = { 1.0f, 0.0f, 0.0f };
const ST_Vector3 gVector3Forward = { 0.0f, 0.0f, 1.0f };
const ST_Vector3 gVector3Zero = { 0.0f, 0.0f, 0.0f };
const ST_Vector3 gVector3One = { 1.0f, 1.0f, 1.0f };
const ST_Vector3 gVector3Half = { 0.5f, 0.5f, 0.5f };
const ST_Vector3 gVector3Max = { FLT_MAX, FLT_MAX, FLT_MAX };
const ST_Vector3 gVector3Left = { -1.0f, 0.0f, 0.0f };
const ST_Vector3 gVector3Down = { 0.0f, -1.0f, 0.0f };
const ST_Vector3 gVector3Back = { 0.0f, 0.0f, -1.0f };
const ST_Vector4 gVector4Zero = { 0.0f, 0.0f, 0.0f, 0.0f };
const ST_Vector4 gVector4One = { 1.0f, 1.0f, 1.0f, 1.0f };
const ST_Vector4 gVector4ColorRed = { 1.0f, 0.0f, 0.0f, 1.0f };
const ST_Vector4 gVector4ColorGreen = { 0.0f, 1.0f, 0.0f, 1.0f };
const ST_Vector4 gVector4ColorBlue = { 0.0f, 0.0f, 1.0f, 1.0f };
const ST_Quaternion gQuaternionIdentity = { 1.0f, 0.0f, 0.0f, 0.0f };
const ST_Vector4 gVector4ColorWhite = { 1.0f, 1.0f, 1.0f, 1.0f };
const ST_Vector4 gVector4ColorBlack = { 0.0f, 0.0f, 0.0f, 1.0f };
const ST_Vector4 gVector4ColorYellow = { 1.0f, 1.0f, 0.0f, 1.0f };
const ST_Vector4 gVector4ColorMagenta = { 1.0f, 0.0f, 1.0f, 1.0f };
const ST_Vector4 gVector4ColorCyan = { 0.0f, 1.0f, 1.0f, 1.0f };
const ST_Vector4 gVector4ColorGold = { 218.0f / 255.0f, 165.0f / 255.0f, 32.0f / 255.0f, 1.0f };
const ST_Vector4 gVector4ColorSilver = { 192.0f / 255.0f, 192.0f / 255.0f, 192.0f / 255.0f, 1.0f };
const ST_Vector4 gVector4ColorSaphire = { 15.0f / 255.0f, 82.0f / 255.0f, 186.0f / 255.0f, 1.0f };
const ST_Vector4 gVector4ColorEmerald = { 80.0f / 255.0f, 200.0f / 255.0f, 120.0f / 255.0f, 1.0f };
const ST_Direction gDirectionRight = { 1.0f, 0.0f, 0.0f, 1 };
const ST_Direction gDirectionLeft = { -1.0f, 0.0f, 0.0f, 1 };
const ST_Direction gDirectionUp = { 0.0f, 1.0f, 0.0f, 1 };
const ST_Direction gDirectionDown = { 0.0f, -1.0f, 0.0f, 1 };
const ST_Direction gDirectionForward = { 0.0f, 0.0f, 1.0f, 1 };
const ST_Direction gDirectionBack = { 0.0f, 0.0f, -1.0f, 1 };
const float tolerance = 1.0e-6f;

//static ST_Vector3 allocatedv3;
//static ST_Vector2 allocatedv2;
//static ST_Vector2Integer allocatedv2i;
//static ST_Vector4 allocatedv4;
//static ST_Quaternion allocatedq;
//static ST_Matrix4 allocatedm;
//static ST_Direction allocateddir;
//static ST_Color allocatedc;

#define FP_FAST_FMA

const ST_Matrix4 gMatrix4Identity = {
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
};

float sphereTraceLerp(float x0, float x1, float t)
{
	return (x1 - x0) * t + x0;
}

float sphereTraceAbs(float f)
{
	if (f >= 0.0f)
	{
		return f;
	}
	else
	{
		return -f;
	}
}

float sphereTraceMin(float a, float b)
{
	if (a <= b)
	{
		return a;
	}
	else
	{
		return b;
	}
}

float sphereTraceMax(float a, float b)
{
	if (a >= b)
	{
		return a;
	}
	else
	{
		return b;
	}
}

float sphereTraceSign(float f)
{
	if (f >= 0.0f)
		return 1.0f;
	else
		return -1.0f;
}

float sphereTraceRound(float f)
{
	if (f > 0.0f)
	{
		return (int)(f + 0.5f);
	}
	else
	{
		return (int)(f - 0.5f);
	}
}

ST_Vector2 sphereTraceVector2Construct(float x, float y)
{
	ST_Vector2 vr;
	vr.x = x;
	vr.y = y;
	return vr;
}

ST_Vector2 sphereTraceVector2ConstructWithVector3(ST_Vector3 v)
{
	ST_Vector2 vr;
	vr.x = v.x;
	vr.y = v.y;
	return vr;
}

ST_Vector2Integer sphereTraceVector2IntegerConstruct(int x, int y)
{
	ST_Vector2Integer vr;
	vr.x = x;
	vr.y = y;
	return vr;
}

//ST_Vector3 sphereTraceVector3Construct(float x, float y, float z)
//{
//	ST_Vector3 v;
//	v.x = x;
//	v.y = y;
//	v.z = z;
//	return v;
//}

ST_Vector3 sphereTraceVector3Construct(float x, float y, float z)
{
	ST_Vector3 vr;
	vr.x = x;
	vr.y = y;
	vr.z = z;
	return vr;
}

ST_Vector3 sphereTraceVector3UniformSize(float size)
{
	ST_Vector3 vr;
	vr.x = size;
	vr.y = size;
	vr.z = size;
	return vr;
}

ST_Vector4 sphereTraceVector4Construct(float x, float y, float z, float w)
{
	ST_Vector4 vr;
	vr.x = x;
	vr.y = y;
	vr.z = z;
	vr.w = w;
	return vr;
}

ST_Vector4 sphereTraceVector4ConstructWithVector3(ST_Vector3 v, float w)
{
	ST_Vector4 vr;
	vr.x = v.x;
	vr.y = v.y;
	vr.z = v.z;
	vr.w = w;
	return vr;
}

ST_Vector3 sphereTraceVector3ConstructWithVector2(ST_Vector2 v, float z)
{
	ST_Vector3 vr;
	vr.x = v.x;
	vr.y = v.y;
	vr.z = z;
	return vr;
}

ST_Matrix4 sphereTraceMatrixConstruct(float m00, float m01, float m02, float m03,
	float m10, float m11, float m12, float m13,
	float m20, float m21, float m22, float m23,
	float m30, float m31, float m32, float m33)
{
	ST_Matrix4 mr;
	mr.m00 = m00;
	mr.m01 = m01;
	mr.m02 = m02;
	mr.m03 = m03;
	mr.m10 = m10;
	mr.m11 = m11;
	mr.m12 = m12;
	mr.m13 = m13;
	mr.m20 = m20;
	mr.m21 = m21;
	mr.m22 = m22;
	mr.m23 = m23;
	mr.m30 = m30;
	mr.m31 = m31;
	mr.m32 = m32;
	mr.m33 = m33;
	return mr;
}


ST_Vector4 sphereTraceMatrixRow(ST_Matrix4 const matrix, int i)
{
	switch (i)
	{
	case 0:
		return sphereTraceVector4Construct(matrix.m00, matrix.m01, matrix.m02, matrix.m03 );
	case 1:
		return sphereTraceVector4Construct(matrix.m10, matrix.m11, matrix.m12, matrix.m13 );
	case 2:
		return sphereTraceVector4Construct(matrix.m20, matrix.m21, matrix.m22, matrix.m23 );
	case 3:
		return sphereTraceVector4Construct(matrix.m30, matrix.m31, matrix.m32, matrix.m33 );
	}


}

ST_Vector4 sphereTraceMatrixCol(ST_Matrix4 const matrix, int i)
{
	switch (i)
	{
	case 0:
		return sphereTraceVector4Construct(matrix.m00, matrix.m10, matrix.m20, matrix.m30 );
	case 1:
		return sphereTraceVector4Construct(matrix.m01, matrix.m11, matrix.m21, matrix.m31 );
	case 2:
		return sphereTraceVector4Construct(matrix.m02, matrix.m12, matrix.m22, matrix.m32 );
	case 3:
		return sphereTraceVector4Construct(matrix.m03, matrix.m13, matrix.m23, matrix.m33 );
	}
}

ST_Vector2 sphereTraceVector2Add(ST_Vector2 v1, ST_Vector2 v2)
{
	return sphereTraceVector2Construct(v1.x + v2.x, v1.y + v2.y );
}

ST_Vector2 sphereTraceVector2Subtract(ST_Vector2 v1, ST_Vector2 v2)
{
	return sphereTraceVector2Construct(v1.x - v2.x, v1.y - v2.y );
}

ST_Vector2Integer sphereTraceVector2IntegerAdd(ST_Vector2Integer v1, ST_Vector2Integer v2)
{
	ST_Vector2Integer vr;
	vr.x = v1.x + v2.x;
	vr.y = v1.y + v2.y;
	return vr;
}

ST_Vector2Integer sphereTraceVector2IntegerSubtract(ST_Vector2Integer v1, ST_Vector2Integer v2)
{
	ST_Vector2Integer vr;
	vr.x = v1.x - v2.x;
	vr.y = v1.y - v2.y;
	return vr;
}

float sphereTraceVector2Dot(ST_Vector2 v1, ST_Vector2 v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}

ST_Vector2 sphereTraceVector2Lerp(ST_Vector2 v1, ST_Vector2 v2, float t)
{
	ST_Vector2 vr;
	vr.x = (v2.x - v1.x) * t + v1.x;
	vr.y = (v2.y - v1.y) * t + v1.y;
	return vr;
}

float sphereTraceVector2Length(ST_Vector2 v)
{
	return sqrtf(v.x * v.x + v.y * v.y);
}


float sphereTraceDegreesToRadians(float degs)
{
	return degs * ((float)M_PI / 180.0f);
}

b32 sphereTraceVector3Equal(ST_Vector3 v1, ST_Vector3 v2)
{
	return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
}

float sphereTraceVector3Length(ST_Vector3 v)
{
	float result = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	return result;
}

float sphereTraceVector3Dot(ST_Vector3 v1, ST_Vector3 v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

float sphereTraceVector3AbsoluteDot(ST_Vector3 v1, ST_Vector3 v2)
{
	float ret = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	if (ret < 0.0f)
		ret *= -1.0f;
	return ret;
}

float sphereTraceVector4Dot(ST_Vector4 v1, ST_Vector4 v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
}

ST_Vector3 sphereTraceVector3Scale(ST_Vector3 v, float f)
{
	return sphereTraceVector3Construct(v.x* f, v.y* f, v.z* f );
}

ST_Vector3 sphereTraceVector3Mult(ST_Vector3 v1, ST_Vector3 v2)
{
	return sphereTraceVector3Construct(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}

void sphereTraceVector3ScaleByRef(ST_Vector3* const pRef, float f)
{
	pRef->x *= f;
	pRef->y *= f;
	pRef->z *= f;
}

ST_Vector3 sphereTraceVector3Normalize(ST_Vector3 v)
{
	return sphereTraceVector3Scale(v, 1.0f / sphereTraceVector3Length(v));
}

void sphereTraceVector3NormalizeByRef(ST_Vector3* const pRef)
{
	sphereTraceVector3ScaleByRef(pRef, 1.0f / sphereTraceVector3Length(*pRef));
}

ST_Vector3 sphereTraceVector3Cross(ST_Vector3 v1, ST_Vector3 v2)
{
	return sphereTraceVector3Construct(v1.y* v2.z - v1.z * v2.y, v1.z* v2.x - v1.x * v2.z, v1.x* v2.y - v1.y * v2.x );
}

ST_Vector3 sphereTraceVector3Add(ST_Vector3 v1, ST_Vector3 v2)
{
	return sphereTraceVector3Construct(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z );
}

void sphereTraceVector3AddByRef(ST_Vector3* const pRef, ST_Vector3 v)
{
	pRef->x += v.x;
	pRef->y += v.y;
	pRef->z += v.z;
}

ST_Vector3 sphereTraceVector3Subtract(ST_Vector3 v1, ST_Vector3 v2)
{
	return sphereTraceVector3Construct(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z );
}

ST_Vector3 sphereTraceVector3SubtractAbsolute(ST_Vector3 v1, ST_Vector3 v2)
{
	return sphereTraceVector3Construct(sphereTraceAbs(v1.x - v2.x), sphereTraceAbs(v1.y - v2.y), sphereTraceAbs(v1.z - v2.z));
}

void sphereTraceVector3SubtractByRef(ST_Vector3* const pRef, ST_Vector3 v)
{
	pRef->x -= v.x;
	pRef->y -= v.y;
	pRef->z -= v.z;
}

ST_Vector3 sphereTraceVector3Negative(ST_Vector3 v)
{
	return sphereTraceVector3Construct(-v.x, -v.y, -v.z );
}

void sphereTraceVector3NegativeByRef(ST_Vector3* pRef)
{
	pRef->x = -pRef->x;
	pRef->y = -pRef->y;
	pRef->z = -pRef->z;
}

void sphereTraceVector3Print(ST_Vector3 v)
{
	printf("x:%f, y:%f, z:%f\n", v.x, v.y, v.z);
}

void sphereTraceVector2Print(ST_Vector2 v)
{
	printf("x:%f, y:%f\n", v.x, v.y);
}

b32 sphereTraceEpsilonEqual(float f1, float f2, float epsilon)
{
	if (sphereTraceAbs(f1 - f2) < epsilon)
		return 1;
	else
		return 0;
}

b32 sphereTraceVector3EpsilonEquals(ST_Vector3 v1, ST_Vector3 v2, float epsilon)
{
	if (sphereTraceAbs(v1.x - v2.x) < epsilon && sphereTraceAbs(v1.y - v2.y) < epsilon && sphereTraceAbs(v1.z - v2.z) < epsilon)
		return 1;
	else
		return 0;
}

b32 sphereTraceVector4EpsilonEquals(ST_Vector4 v1, ST_Vector4 v2, float epsilon)
{
	if (sphereTraceAbs(v1.x - v2.x) < epsilon && sphereTraceAbs(v1.y - v2.y) < epsilon && sphereTraceAbs(v1.z - v2.z) < epsilon && sphereTraceAbs(v1.w - v2.w) < epsilon)
		return 1;
	else
		return 0;
}

ST_Vector3 sphereTraceVector3CopySign(ST_Vector3 v, float f)
{
	float s = 1.0f;
	s = _copysignf(s, f);
	return sphereTraceVector3Scale(v, s);
}

float sphereTraceVector3Length2(ST_Vector3 vec)
{
	return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
}

ST_Vector3 sphereTraceVector3AddAndScale(ST_Vector3 toAdd, ST_Vector3 toScale, float scale)
{
	//return sphereTraceVector3Add(toAdd, sphereTraceVector3Construct(toScale.x* scale, toScale.y* scale, toScale.z* scale ));
	ST_Vector3 v;
	v.x = fmaf(toScale.x, scale, toAdd.x);
	v.y = fmaf(toScale.y, scale, toAdd.y);
	v.z = fmaf(toScale.z, scale, toAdd.z);
	return v;
}

ST_Vector3 sphereTraceVector3AddAndScale2(ST_Vector3 toAdd, ST_Vector3 toScale1, float scale1, ST_Vector3 toScale2, float scale2)
{
	//return sphereTraceVector3Add(toAdd, sphereTraceVector3Construct(toScale.x* scale, toScale.y* scale, toScale.z* scale ));
	ST_Vector3 v;
	v.x = fmaf(toScale2.x, scale2,fmaf(toScale1.x, scale1, toAdd.x));
	v.y = fmaf(toScale2.y, scale2,fmaf(toScale1.y, scale1, toAdd.y));
	v.z = fmaf(toScale2.z, scale2,fmaf(toScale1.z, scale1, toAdd.z));
	return v;
}

ST_Vector3 sphereTraceVector3AddAndScale3(ST_Vector3 toAdd, ST_Vector3 toScale1, float scale1, ST_Vector3 toScale2, float scale2, ST_Vector3 toScale3, float scale3)
{
	ST_Vector3 v;
	v.x = fmaf(toScale3.x, scale3, fmaf(toScale2.x, scale2, fmaf(toScale1.x, scale1, toAdd.x)));
	v.y = fmaf(toScale3.y, scale3, fmaf(toScale2.y, scale2, fmaf(toScale1.y, scale1, toAdd.y)));
	v.z = fmaf(toScale3.z, scale3, fmaf(toScale2.z, scale2, fmaf(toScale1.z, scale1, toAdd.z)));
	return v;
}

void sphereTraceVector3AddAndScaleByRef(ST_Vector3* const pRef, ST_Vector3 toScale, float scale)
{
	pRef->x += toScale.x * scale;
	pRef->y += toScale.y * scale;
	pRef->z += toScale.z * scale;
}

void sphereTraceVector3AddAndScale2ByRef(ST_Vector3* const pRef, ST_Vector3 toScale1, float scale1, ST_Vector3 toScale2, float scale2)
{
	pRef->x += toScale1.x * scale1 + toScale2.x * scale2;
	pRef->y += toScale1.y * scale1 + toScale2.y * scale2;
	pRef->z += toScale1.z * scale1 + toScale2.z * scale2;
}

ST_Vector3 sphereTraceVector3Average(ST_Vector3 v1, ST_Vector3 v2)
{
	return sphereTraceVector3Construct(0.5f * (v1.x + v2.x), 0.5f * (v1.y + v2.y), 0.5f * (v1.z + v2.z) );
}

b32 sphereTraceVector3Nan(ST_Vector3 vec)
{
	return fpclassify(vec.x) == FP_NAN && fpclassify(vec.y) == FP_NAN && fpclassify(vec.z) == FP_NAN;
}

b32 sphereTraceVector3NanAny(ST_Vector3 vec)
{
	return fpclassify(vec.x) == FP_NAN || fpclassify(vec.y) == FP_NAN || fpclassify(vec.z) == FP_NAN;
}

b32 sphereTraceVector3AnyGreaterThan(ST_Vector3 vec, float val)
{
	return  (sphereTraceAbs(vec.x) > val || sphereTraceAbs(vec.y) > val || sphereTraceAbs(vec.z) > val);
}

ST_Vector3 sphereTraceClosestPointOnLineBetweenTwoLines(ST_Vector3 point, ST_Vector3 lineDir, ST_Vector3 otherPoint, ST_Vector3 otherLineDir)
{
	ST_Vector3 cross = sphereTraceVector3Cross(lineDir, otherLineDir);
		float crossSquared = sphereTraceVector3Length2(cross);
		float s = sphereTraceVector3Dot(sphereTraceVector3Cross(sphereTraceVector3Subtract(otherPoint, point), otherLineDir), cross) / crossSquared;
		return sphereTraceVector3AddAndScale(point, lineDir, s);
}

b32 sphereTraceVector3ClosestPointOnLineBetweenTwoLinesIsGreaterThanZeroAndLessThanMaxDist(ST_Vector3 point, ST_Vector3 normalizedLineDir, ST_Vector3 otherPoint, ST_Vector3 otherNormalizedLineDir, float maxDist)
{
	ST_Vector3 cross = sphereTraceVector3Cross(normalizedLineDir, otherNormalizedLineDir);
	float crossSquared = sphereTraceVector3Length2(cross);
	float t = sphereTraceVector3Dot(sphereTraceVector3Cross(sphereTraceVector3Subtract(otherPoint, point), normalizedLineDir), cross) / crossSquared;
	return (t>=0.0f && t<=maxDist);
}

ST_Vector3 sphereTraceMathClosestPointOnLineNearestToPoint(ST_Vector3 linePoint, ST_Vector3 lineDir, ST_Vector3 point)
{
	ST_Vector3 dp = sphereTraceVector3Subtract(point, linePoint);
	ST_Vector3 dir = sphereTraceVector3Normalize(sphereTraceVector3Cross(sphereTraceVector3Cross(dp, lineDir), lineDir));
	float dist = sphereTraceVector3Dot(dir, dp);
	return sphereTraceVector3AddAndScale(point, dir, -dist);
}

void sphereTraceMathClosestPointOnLineNearestToPointExtractData(ST_Vector3 linePoint, ST_Vector3 lineDir, ST_Vector3 point, ST_Vector3* pPoint, ST_Vector3* pDir,  float* pClosestDist)
{
	ST_Vector3 dp = sphereTraceVector3Subtract(point, linePoint);
	ST_Vector3 dir = sphereTraceVector3Normalize(sphereTraceVector3Cross( lineDir, sphereTraceVector3Cross(dp, lineDir)));
	//dir towards the point from the closest point on line
	*pDir = dir;
	//distance from the line point
	*pClosestDist = sphereTraceVector3Dot(dp, dir);
	//closest point on line
	*pPoint = sphereTraceVector3AddAndScale(point, dir, -*pClosestDist);
}

//line must have a negative slope, and positive intersept
float sphereTraceMathCircleIntersectLine(float circleRadius, float slope, float xIntersept)
{
	if (isinf(slope) || slope== -FLT_MAX)
	{
		return xIntersept - circleRadius;
	}
	float theta = atanf(-1.0f / slope);
	float beta = M_PI * 0.5 - theta;
	float sBeta = sinf(beta);
	return (xIntersept + xIntersept * sBeta / slope) / (sBeta + 1);
}

float sphereTraceMathCircleIntersectPoint(float circleRadius, float pointDistance, float pointHeight)
{
	if (pointHeight > circleRadius)
		return FLT_MAX;
	return pointDistance - cosf(asinf(pointHeight / circleRadius)) * circleRadius;
}

float sphereTraceMathCircleIntersectLineWithPoints(float circleRadius, float yIntersect, float x1, float y1, float x2, float y2)
{
	float slope = (y2 - y1) / (x2 - x1);
	float theta = atanf(-1.0f / slope);
	float beta = M_PI * 0.5 - theta;
	float sBeta = sinf(beta);
	float xIntersept = -yIntersect / slope;
	float dist = xIntersept - (circleRadius / sBeta);
	if (dist > x2)
		dist = sphereTraceMathCircleIntersectPoint(circleRadius, x2, y2);
	return dist;
}

float sphereTraceVector3ClosestPointOnLineBetweenTwoLinesDistance(ST_Vector3 point, ST_Vector3 normalizedLineDir, ST_Vector3 otherPoint, ST_Vector3 otherNormalizedLineDir)
{
	ST_Vector3 cross = sphereTraceVector3Cross(normalizedLineDir, otherNormalizedLineDir);
	float crossSquared = sphereTraceVector3Length2(cross);
	float t = sphereTraceVector3Dot(sphereTraceVector3Cross(sphereTraceVector3Subtract(otherPoint, point), otherNormalizedLineDir), cross) / crossSquared;
	float s = sphereTraceVector3Dot(sphereTraceVector3Cross(sphereTraceVector3Subtract(otherPoint, point), normalizedLineDir), cross) / crossSquared;
	ST_Vector3 p1 = sphereTraceVector3AddAndScale(point, normalizedLineDir, t);
	ST_Vector3 p2 = sphereTraceVector3AddAndScale(otherPoint, otherNormalizedLineDir, s);

	return sphereTraceVector3Distance(p1, p2);
}

void sphereTraceVector3ClosestPointOnLineBetweenTwoLinesDistancesOnLines(ST_Vector3 point, ST_Vector3 normalizedLineDir, ST_Vector3 otherPoint, ST_Vector3 otherNormalizedLineDir, float* dist1, float* dist2)
{
	ST_Vector3 cross = sphereTraceVector3Cross(normalizedLineDir, otherNormalizedLineDir);
	float crossSquared = sphereTraceVector3Length2(cross);
	*dist1 = sphereTraceVector3Dot(sphereTraceVector3Cross(sphereTraceVector3Subtract(otherPoint, point), otherNormalizedLineDir), cross) / crossSquared;
	*dist2 = sphereTraceVector3Dot(sphereTraceVector3Cross(sphereTraceVector3Subtract(otherPoint, point), normalizedLineDir), cross) / crossSquared;
}

void sphereTraceVector3ClosestPointsOnLineBetweenTwoLines(ST_Vector3 point, ST_Vector3 lineDir, ST_Vector3 otherPoint, ST_Vector3 otherLineDir, ST_Vector3* result1, ST_Vector3* result2)
{
	ST_Vector3 cross = sphereTraceVector3Cross(lineDir, otherLineDir);
	float crossSquared = sphereTraceVector3Length2(cross);
	float s = sphereTraceVector3Dot(sphereTraceVector3Cross(sphereTraceVector3Subtract(otherPoint, point), otherLineDir), cross) / crossSquared;
	float t = sphereTraceVector3Dot(sphereTraceVector3Cross(sphereTraceVector3Subtract(otherPoint, point), lineDir), cross) / crossSquared;
	*result1 = sphereTraceVector3AddAndScale(point, lineDir, s);
	*result2 = sphereTraceVector3AddAndScale(otherPoint, otherLineDir, t);
}

float sphereTraceVector3Distance(ST_Vector3 point1, ST_Vector3 point2)
{
	float dx, dy, dz;
	dx = point2.x - point1.x;
	dy = point2.y - point1.y;
	dz = point2.z - point1.z;
	return sqrtf(dx * dx + dy * dy + dz * dz);
}

float sphereTraceVector3HorizontalDistance(ST_Vector3 point1, ST_Vector3 point2)
{
	float dx, dz;
	dx = point2.x - point1.x;
	dz = point2.z - point1.z;
	return sqrtf(dx * dx + dz * dz);
}

ST_Vector3 sphereTraceVector3Lerp(ST_Vector3 point1, ST_Vector3 point2, float t)
{
	ST_Vector3 p;
	p.x = fmaf((point2.x - point1.x), t, point1.x);
	p.y = fmaf((point2.y - point1.y), t, point1.y);
	p.z = fmaf((point2.z - point1.z), t, point1.z);
	return p;
}

ST_Vector4 sphereTraceVector4Lerp(ST_Vector4 point1, ST_Vector4 point2, float t)
{
	ST_Vector4 p;
	p.x = fmaf((point2.x - point1.x), t, point1.x);
	p.y = fmaf((point2.y - point1.y), t, point1.y);
	p.z = fmaf((point2.z - point1.z), t, point1.z);
	p.w = fmaf((point2.w - point1.w), t, point1.w);
	return p;
}

ST_Vector3 sphereTraceNormalizeBetweenPoints(ST_Vector3 to, ST_Vector3 from)
{
	float dist = sphereTraceVector3Distance(from, to);
	ST_Vector3 v;
	v.x = (to.x - from.x) / dist;
	v.y = (to.y - from.y) / dist;
	v.z = (to.z - from.z) / dist;
	return v;
}

ST_Vector3 sphereTraceVector3ProjectVector3OntoPlane(ST_Vector3 vec, ST_Direction planeNormal)
{
	float dnm = sphereTraceVector3Dot(vec, planeNormal.v);
	ST_Vector3 dnv = sphereTraceVector3Scale(planeNormal.v, dnm);
	ST_Vector3 dt = sphereTraceVector3Subtract(vec, dnv);
	return dt;
}

b32 sphereTraceVector3IsVectorPositiveInDirection(ST_Vector3 vec, ST_Direction direction)
{
	return sphereTraceVector3Dot(vec, direction.v) >= 0.0f;
}

ST_Matrix4 sphereTraceMatrixIdentity()
{
	return sphereTraceMatrixConstruct(
		1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
	);
}

ST_Matrix4 sphereTraceMatrixRotateX(float rad)
{
	float sinTheta = sinf(rad);
	float cosTheta = cosf(rad);
	return sphereTraceMatrixConstruct(
		1, 0, 0, 0,
			0, cosTheta, -sinTheta, 0,
			0, sinTheta, cosTheta, 0,
			0, 0, 0, 1
	);
}

ST_Matrix4 sphereTraceMatrixRotateY(float rad)
{
	float sinTheta = sinf(rad);
	float cosTheta = cosf(rad);
	return sphereTraceMatrixConstruct(
		cosTheta, 0, sinTheta, 0,
			0, 1, 0, 0,
			-sinTheta, 0, cosTheta, 0,
			0, 0, 0, 1
	);
}

ST_Matrix4 sphereTraceMatrixRotateZ(float rad)
{
	float sinTheta = sinf(rad);
	float cosTheta = cosf(rad);
	return sphereTraceMatrixConstruct(
		cosTheta, -sinTheta, 0, 0,
			sinTheta, cosTheta, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
	);
}

ST_Vector4 sphereTraceMatrixVector4Mult(ST_Matrix4 mat, ST_Vector4 vec)
{
	ST_Vector4 vr;
	vr.x = mat.m00 * vec.x + mat.m01 * vec.y + mat.m02 * vec.z + mat.m03 * vec.w;
	vr.y = mat.m10 * vec.x + mat.m11 * vec.y + mat.m12 * vec.z + mat.m13 * vec.w;
	vr.z = mat.m20 * vec.x + mat.m21 * vec.y + mat.m22 * vec.z + mat.m23 * vec.w;
	vr.w = mat.m30 * vec.x + mat.m31 * vec.y + mat.m32 * vec.z + mat.m33 * vec.w;
	return vr;
}

ST_Vector3 sphereTraceMatrixVector3Mult(ST_Matrix4 mat, ST_Vector3 vec)
{
	ST_Vector3 vr;
	vr.x = mat.m00 * vec.x + mat.m01 * vec.y + mat.m02 * vec.z;
	vr.y = mat.m10 * vec.x + mat.m11 * vec.y + mat.m12 * vec.z;
	vr.z = mat.m20 * vec.x + mat.m21 * vec.y + mat.m22 * vec.z;
	return vr;
}

ST_Vector4 sphereTraceVector4MatrixMult(ST_Vector4 vec, ST_Matrix4 mat)
{
	ST_Vector4 vr;
	vr.x = mat.m00 * vec.x + mat.m10 * vec.y + mat.m20 * vec.z + mat.m30 * vec.w;
	vr.y = mat.m01 * vec.x + mat.m11 * vec.y + mat.m21 * vec.z + mat.m31 * vec.w;
	vr.z = mat.m02 * vec.x + mat.m12 * vec.y + mat.m22 * vec.z + mat.m32 * vec.w;
	vr.w = mat.m03 * vec.x + mat.m13 * vec.y + mat.m23 * vec.z + mat.m33 * vec.w;
	return vr;
}

//ST_Matrix4 sphereTraceMatrixTranspose(ST_Matrix4 mat)
//{
//	return sphereTraceMatrixConstruct(
//		mat.m00, mat.m10, mat.m20, mat.m30,
//		mat.m01, mat.m11, mat.m21, mat.m31,
//		mat.m02, mat.m12, mat.m22, mat.m32,
//		mat.m03, mat.m13, mat.m23, mat.m33
//	);
//}

ST_Matrix4 sphereTraceMatrixTranslation(ST_Vector3 trans)
{
	return sphereTraceMatrixConstruct(
		1, 0, 0, trans.x,
			0, 1, 0, trans.y,
			0, 0, 1, trans.z,
			0, 0, 0, 1
	);
}

ST_Matrix4 sphereTraceMatrixScale(ST_Vector3 scale)
{
	return sphereTraceMatrixConstruct(
		scale.x, 0, 0, 0,
			0, scale.y, 0, 0,
			0, 0, scale.z, 0,
			0, 0, 0, 1
	);
}

ST_Matrix4 sphereTraceMatrixRotate(ST_Vector3 eulerAngles)
{
	float cAlpha = cos(eulerAngles.z);
	float sAlpha = sin(eulerAngles.z);
	float cBeta = cos(eulerAngles.y);
	float sBeta = sin(eulerAngles.y);
	float cGamma = cos(eulerAngles.x);
	float sGamma = sin(eulerAngles.x);
	return sphereTraceMatrixConstruct(
		cAlpha*cBeta, cAlpha*sBeta*sGamma-sAlpha*cGamma, cAlpha* sBeta* cGamma + sAlpha * sGamma, 0.0f,
			sAlpha* cBeta, sAlpha* sBeta* sGamma + cAlpha * cGamma, sAlpha* sBeta* cGamma - cAlpha * sGamma, 0.0f,
			-sBeta, cBeta*sGamma, cBeta*cGamma, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
	);
}

ST_Matrix4 sphereTraceMatrixTranspose(ST_Matrix4 mat)
{
	return sphereTraceMatrixConstruct(
		mat.m00, mat.m10, mat.m20, mat.m30,
		mat.m01, mat.m11, mat.m21, mat.m31,
		mat.m02, mat.m12, mat.m22, mat.m32,
		mat.m03, mat.m13, mat.m23, mat.m33
	);
}

ST_Matrix4 sphereTraceMatrixPerspective(float aspectRatio, float fovYRadians, float zNear, float zFar)
{
	float yScale = tanf(0.5f * ((float)M_PI - fovYRadians));
	float xScale = yScale / aspectRatio;
	float zRangeInverse = 1.0f / (zNear - zFar);
	float zScale = zFar * zRangeInverse;
	float zTranslation = zFar * zNear * zRangeInverse;
	return sphereTraceMatrixConstruct(
		xScale, 0, 0, 0,
			0, yScale, 0, 0,
			0, 0, zScale, zTranslation,
			0, 0, -1, 0
	);
}

ST_Matrix4 sphereTraceMatrixOrthographic(float left, float right, float top, float bottom, float nearf, float farf, float w)
{
	float rl = right - left;
	float tb = top - bottom;
	float fn = farf - nearf;
	return sphereTraceMatrixConstruct(
		2.0f/rl, 0, 0, -(right+left)/rl,
		0, 2.0f/tb, 0, -(top+bottom)/tb,
		0, 0, -2.0f/fn, -(farf+nearf)/fn,
		0, 0, 0, w
	);
}

//inline ST_Matrix4 sphereTraceMatrixLookAt(ST_Vector3 eye, ST_Vector3 at, ST_Vector3 up)
//{
//	ST_Vector3 zAxis = sphereTraceVector3Normalize(sphereTraceVector3Subtract(at, eye));
//	ST_Vector3 xAxis = sphereTraceVector3Normalize(sphereTraceVector3Cross(up, zAxis));
//	ST_Vector3 yAxis = sphereTraceVector3Cross(zAxis, xAxis);
//
//	return (ST_Matrix4) {
//		xAxis.x, yAxis.x, zAxis.x, 0,
//		xAxis.y, yAxis.y, zAxis.y, 0,
//		xAxis.z, yAxis.z, zAxis.z, 0,
//		sphereTraceVector3Dot(xAxis, eye), sphereTraceVector3Dot(yAxis, eye), sphereTraceVector3Dot(zAxis, eye), 1
//	};
//}

ST_Matrix4 sphereTraceMatrixLookAt(ST_Vector3 eye, ST_Vector3 at, ST_Vector3 up)
{
	ST_Vector3 zAxis = sphereTraceVector3Normalize(sphereTraceVector3Subtract(eye, at));
	ST_Vector3 xAxis = sphereTraceVector3Normalize(sphereTraceVector3Cross(up, zAxis));
	ST_Vector3 yAxis = sphereTraceVector3Cross(zAxis, xAxis);

	return sphereTraceMatrixConstruct(
		xAxis.x, xAxis.y, xAxis.z, -sphereTraceVector3Dot(xAxis, eye),
			yAxis.x, yAxis.y, yAxis.z, -sphereTraceVector3Dot(yAxis, eye),
			zAxis.x, zAxis.y, zAxis.z, -sphereTraceVector3Dot(zAxis, eye),
			0, 0, 0, 1
	);
}

ST_Matrix4 sphereTraceMatrixMult(ST_Matrix4 mat1, ST_Matrix4 mat2)
{
	ST_Matrix4 ret;
	ret.m00 = mat1.m00 * mat2.m00 + mat1.m01 * mat2.m10 + mat1.m02 * mat2.m20 + mat1.m03 * mat2.m30;
	ret.m01 = mat1.m00 * mat2.m01 + mat1.m01 * mat2.m11 + mat1.m02 * mat2.m21 + mat1.m03 * mat2.m31;
	ret.m02 = mat1.m00 * mat2.m02 + mat1.m01 * mat2.m12 + mat1.m02 * mat2.m22 + mat1.m03 * mat2.m32;
	ret.m03 = mat1.m00 * mat2.m03 + mat1.m01 * mat2.m13 + mat1.m02 * mat2.m23 + mat1.m03 * mat2.m33;			  
	ret.m10 = mat1.m10 * mat2.m00 + mat1.m11 * mat2.m10 + mat1.m12 * mat2.m20 + mat1.m13 * mat2.m30;
	ret.m11 = mat1.m10 * mat2.m01 + mat1.m11 * mat2.m11 + mat1.m12 * mat2.m21 + mat1.m13 * mat2.m31;
	ret.m12 = mat1.m10 * mat2.m02 + mat1.m11 * mat2.m12 + mat1.m12 * mat2.m22 + mat1.m13 * mat2.m32;
	ret.m13 = mat1.m10 * mat2.m03 + mat1.m11 * mat2.m13 + mat1.m12 * mat2.m23 + mat1.m13 * mat2.m33;			  
	ret.m20 = mat1.m20 * mat2.m00 + mat1.m21 * mat2.m10 + mat1.m22 * mat2.m20 + mat1.m23 * mat2.m30;
	ret.m21 = mat1.m20 * mat2.m01 + mat1.m21 * mat2.m11 + mat1.m22 * mat2.m21 + mat1.m23 * mat2.m31;
	ret.m22 = mat1.m20 * mat2.m02 + mat1.m21 * mat2.m12 + mat1.m22 * mat2.m22 + mat1.m23 * mat2.m32;
	ret.m23 = mat1.m20 * mat2.m03 + mat1.m21 * mat2.m13 + mat1.m22 * mat2.m23 + mat1.m23 * mat2.m33;			  
	ret.m30 = mat1.m30 * mat2.m00 + mat1.m31 * mat2.m10 + mat1.m32 * mat2.m20 + mat1.m33 * mat2.m30;
	ret.m31 = mat1.m30 * mat2.m01 + mat1.m31 * mat2.m11 + mat1.m32 * mat2.m21 + mat1.m33 * mat2.m31;
	ret.m32 = mat1.m30 * mat2.m02 + mat1.m31 * mat2.m12 + mat1.m32 * mat2.m22 + mat1.m33 * mat2.m32;
	ret.m33 = mat1.m30 * mat2.m03 + mat1.m31 * mat2.m13 + mat1.m32 * mat2.m23 + mat1.m33 * mat2.m33;
	return ret;
}

ST_Matrix4 sphereTraceMatrixMultByRef(const ST_Matrix4* pMat1, const ST_Matrix4* pMat2)
{
	ST_Matrix4 ret;
	ret.m00 = pMat1->m00 * pMat2->m00 + pMat1->m01 * pMat2->m10 + pMat1->m02 * pMat2->m20 + pMat1->m03 * pMat2->m30;
	ret.m01 = pMat1->m00 * pMat2->m01 + pMat1->m01 * pMat2->m11 + pMat1->m02 * pMat2->m21 + pMat1->m03 * pMat2->m31;
	ret.m02 = pMat1->m00 * pMat2->m02 + pMat1->m01 * pMat2->m12 + pMat1->m02 * pMat2->m22 + pMat1->m03 * pMat2->m32;
	ret.m03 = pMat1->m00 * pMat2->m03 + pMat1->m01 * pMat2->m13 + pMat1->m02 * pMat2->m23 + pMat1->m03 * pMat2->m33;
	ret.m10 = pMat1->m10 * pMat2->m00 + pMat1->m11 * pMat2->m10 + pMat1->m12 * pMat2->m20 + pMat1->m13 * pMat2->m30;
	ret.m11 = pMat1->m10 * pMat2->m01 + pMat1->m11 * pMat2->m11 + pMat1->m12 * pMat2->m21 + pMat1->m13 * pMat2->m31;
	ret.m12 = pMat1->m10 * pMat2->m02 + pMat1->m11 * pMat2->m12 + pMat1->m12 * pMat2->m22 + pMat1->m13 * pMat2->m32;
	ret.m13 = pMat1->m10 * pMat2->m03 + pMat1->m11 * pMat2->m13 + pMat1->m12 * pMat2->m23 + pMat1->m13 * pMat2->m33;
	ret.m20 = pMat1->m20 * pMat2->m00 + pMat1->m21 * pMat2->m10 + pMat1->m22 * pMat2->m20 + pMat1->m23 * pMat2->m30;
	ret.m21 = pMat1->m20 * pMat2->m01 + pMat1->m21 * pMat2->m11 + pMat1->m22 * pMat2->m21 + pMat1->m23 * pMat2->m31;
	ret.m22 = pMat1->m20 * pMat2->m02 + pMat1->m21 * pMat2->m12 + pMat1->m22 * pMat2->m22 + pMat1->m23 * pMat2->m32;
	ret.m23 = pMat1->m20 * pMat2->m03 + pMat1->m21 * pMat2->m13 + pMat1->m22 * pMat2->m23 + pMat1->m23 * pMat2->m33;
	ret.m30 = pMat1->m30 * pMat2->m00 + pMat1->m31 * pMat2->m10 + pMat1->m32 * pMat2->m20 + pMat1->m33 * pMat2->m30;
	ret.m31 = pMat1->m30 * pMat2->m01 + pMat1->m31 * pMat2->m11 + pMat1->m32 * pMat2->m21 + pMat1->m33 * pMat2->m31;
	ret.m32 = pMat1->m30 * pMat2->m02 + pMat1->m31 * pMat2->m12 + pMat1->m32 * pMat2->m22 + pMat1->m33 * pMat2->m32;
	ret.m33 = pMat1->m30 * pMat2->m03 + pMat1->m31 * pMat2->m13 + pMat1->m32 * pMat2->m23 + pMat1->m33 * pMat2->m33;
	return ret;
}

void sphereTraceMatrixPrint(ST_Matrix4 mat)
{
	printf("%f, %f, %f, %f\n%f, %f, %f, %f\n%f, %f, %f, %f\n%f, %f, %f, %f\n", mat.m00, mat.m01, mat.m02, mat.m03,
		mat.m10, mat.m11, mat.m12, mat.m13,
		mat.m20, mat.m21, mat.m22, mat.m23,
		mat.m30, mat.m31, mat.m32, mat.m33);
}

ST_Vector3 sphereTraceVector3GetLocalXAxisFromRotationMatrix(ST_Matrix4 mat)
{
	return sphereTraceVector3Construct( mat.m00, mat.m10, mat.m20 );
}

ST_Vector3 sphereTraceVector3GetLocalYAxisFromRotationMatrix(ST_Matrix4 mat)
{
	return sphereTraceVector3Construct(mat.m01, mat.m11, mat.m21 );
}

ST_Vector3 sphereTraceVector3GetLocalZAxisFromRotationMatrix(ST_Matrix4 mat)
{
	return sphereTraceVector3Construct(mat.m02, mat.m12, mat.m22 );
}


ST_Direction sphereTraceDirectionGetLocalXAxisFromRotationMatrix(ST_Matrix4 mat)
{
	return sphereTraceDirectionConstruct1(mat.m00, mat.m10, mat.m20, 1);
}

ST_Direction sphereTraceDirectionGetLocalYAxisFromRotationMatrix(ST_Matrix4 mat)
{
	return sphereTraceDirectionConstruct1(mat.m01, mat.m11, mat.m21, 1);
}

ST_Direction sphereTraceDirectionGetLocalZAxisFromRotationMatrix(ST_Matrix4 mat)
{
	return sphereTraceDirectionConstruct1(mat.m02, mat.m12, mat.m22, 1);
}

void sphereTraceMatrixSetLocalXAxisOfRotationMatrix(ST_Matrix4* const mat, ST_Vector3 xAxis)
{
	mat->m00 = xAxis.x;
	mat->m10 = xAxis.y;
	mat->m20 = xAxis.z;
}

void sphereTraceMatrixSetLocalYAxisOfRotationMatrix(ST_Matrix4* const mat, ST_Vector3 yAxis)
{
	mat->m01 = yAxis.x;
	mat->m11 = yAxis.y;
	mat->m21 = yAxis.z;
}

void sphereTraceMatrixSetLocalZAxisOfRotationMatrix(ST_Matrix4* const mat, ST_Vector3 zAxis)
{
	mat->m02 = zAxis.x;
	mat->m12 = zAxis.y;
	mat->m22 = zAxis.z;
}

ST_Quaternion sphereTraceQuaternionConstruct(float w, float x, float y, float z)
{
	ST_Quaternion qr;
	qr.w = w;
	qr.x = x;
	qr.y = y;
	qr.z = z;
	return qr;
}


ST_Quaternion sphereTraceQuaternionConjugate(ST_Quaternion quat)
{
	return sphereTraceQuaternionConstruct(quat.w, -quat.x, -quat.y, -quat.z );
}

void sphereTraceQuaternionConjugateByRef(ST_Quaternion* const pRef)
{
	pRef->w = -pRef->w;
	pRef->x = -pRef->x;
	pRef->y = -pRef->y;
	pRef->z = -pRef->z;
}

ST_Matrix4 sphereTraceMatrixFromQuaternion(ST_Quaternion quat)
{
	ST_Matrix4 mr;
	mr.m00 = -2.0f * (quat.y * quat.y + quat.z * quat.z) + 1.0f;
	mr.m01 = 2.0f * (quat.x * quat.y - quat.w * quat.z);
	mr.m02 = 2.0f * (quat.x * quat.z + quat.w * quat.y);
	mr.m03 = 0.0f;
	mr.m10 = 2.0f * (quat.x * quat.y + quat.w * quat.z);
	mr.m11 = -2.0f * (quat.x * quat.x + quat.z * quat.z) + 1.0f;
	mr.m12 = 2.0f * (quat.y * quat.z - quat.w * quat.x);
	mr.m13 = 0.0f;
	mr.m20 = 2.0f * (quat.x * quat.z - quat.w * quat.y);
	mr.m21 = 2.0f * (quat.y * quat.z + quat.w * quat.x);
	mr.m22 = -2.0f * (quat.x * quat.x + quat.y * quat.y) + 1.0f;
	mr.m23 = 0.0f;
	mr.m30 = 0.0f;
	mr.m31 = 0.0f;
	mr.m32 = 0.0f;
	mr.m33 = 1.0f;
	//ST_Vector3 right = sphereTraceVector3GetLocalXAxisFromRotationMatrix(allocatedm);
	//ST_Vector3 Up = sphereTraceVector3GetLocalYAxisFromRotationMatrix(allocatedm);
	//ST_Vector3 fwd = sphereTraceVector3Cross(right, Up);
	//sphereTraceMatrixSetLocalZAxisOfRotationMatrix(&allocatedm, fwd);
	return mr;
}

ST_Quaternion sphereTraceQuaternionFromAngleAxis(ST_Vector3 axis, float angle)
{
	//axis = sphereTraceVector3Normalize(axis);
	float sTheta = sinf(angle * 0.5f);
	float cTheta = cosf(angle * 0.5f);
	return sphereTraceQuaternionConstruct(cTheta, axis.x* sTheta, axis.y* sTheta, axis.z* sTheta );
}

ST_Quaternion sphereTraceQuaternionFromEulerAngles(ST_Vector3 eulerAngles)
{
	float cr = cos(eulerAngles.x * 0.5f);
	float sr = sin(eulerAngles.x * 0.5f);
	float cp = cos(eulerAngles.y * 0.5f);
	float sp = sin(eulerAngles.y * 0.5f);
	float cy = cos(eulerAngles.z * 0.5f);
	float sy = sin(eulerAngles.z * 0.5f);

	ST_Quaternion qr;
	qr.w = cr * cp * cy + sr * sp * sy;
	qr.x = sr * cp * cy - cr * sp * sy;
	qr.y = cr * sp * cy + sr * cp * sy;
	qr.z = cr * cp * sy - sr * sp * cy;

	return qr;
}

ST_Quaternion sphereTraceQuaternionNormalize(ST_Quaternion quat)
{
	float mag = sqrtf(quat.w * quat.w + quat.x * quat.x + quat.y * quat.y + quat.z * quat.z);
	return sphereTraceQuaternionConstruct(quat.w / mag, quat.x / mag, quat.y / mag, quat.z / mag );
}

void sphereTraceQuaternionNormalizeByRef(ST_Quaternion* const pRef)
{
	float mag = sqrtf(pRef->w * pRef->w + pRef->x * pRef->x + pRef->y * pRef->y + pRef->z * pRef->z);
	pRef->w = pRef->w / mag;
	pRef->x = pRef->x / mag;
	pRef->y = pRef->y / mag;
	pRef->z = pRef->z / mag;
}

ST_Quaternion sphereTraceQuaternionMultiply(ST_Quaternion a, ST_Quaternion b)
{
	ST_Quaternion qr;
	qr.w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z;
	qr.x = a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y;
	qr.y = a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x;
	qr.z = a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w;
	return qr;
}

ST_Quaternion sphereTraceQuaternionAdd(ST_Quaternion a, ST_Quaternion b)
{
	ST_Quaternion qr;
	qr.w = a.w + b.w;
	qr.x = a.x + b.x;
	qr.y = a.y + b.y;
	qr.z = a.z + b.z;
	return qr;
}

ST_Quaternion sphereTraceQuaternionSubtract(ST_Quaternion a, ST_Quaternion b)
{
	ST_Quaternion qr;
	qr.w = a.w - b.w;
	qr.x = a.x - b.x;
	qr.y = a.y - b.y;
	qr.z = a.z - b.z;
	return qr;
}

ST_Quaternion sphereTraceQuaternionScale(float f, ST_Quaternion a)
{
	ST_Quaternion qr;
	qr.w = a.w * f;
	qr.x = a.x * f;
	qr.y = a.y * f;
	qr.z = a.z * f;
	return qr;
}

ST_Quaternion sphereTraceQuaternionLookAt(ST_Vector3 eye, ST_Vector3 at, ST_Vector3 up)
{
	ST_Vector3 forward = sphereTraceVector3Normalize(sphereTraceVector3Subtract( at, eye));
	float dot = sphereTraceVector3Dot(forward, up);
	ST_Vector3 right;
	if (sphereTraceAbs(dot - 1.0f) < tolerance)
	{
		right = gVector3Left;
		up = gVector3Forward;
	}
	else if (sphereTraceAbs(dot + 1.0f) < tolerance)
	{
		right = gVector3Left;
		up = gVector3Back;
	}
	else
	{
		right = sphereTraceVector3Normalize(sphereTraceVector3Cross(up, forward));
		up = sphereTraceVector3Cross(forward, right);
	}

	return sphereTraceMatrixQuaternionFromRotationMatrix(sphereTraceMatrixConstructFromRightForwardUp(right, up, forward));

}

//ST_Quaternion sphereTraceQuaternionLookFromUp(ST_Vector3 eye, ST_Vector3 at)
//{
//	ST_Vector3 forward = sphereTraceVector3Normalize(sphereTraceVector3Subtract(at, eye));
//	float dot = sphereTraceVector3Dot(forward, gVector3Up);
//	ST_Vector3 right;
//	if (sphereTraceAbs(dot - 1.0f) < tolerance)
//	{
//		//right = gVector3Left;
//		//up = gVector3Forward;
//	}
//	else if (sphereTraceAbs(dot + 1.0f) < tolerance)
//	{
//		//right = gVector3Left;
//		//up = gVector3Back;
//	}
//	else
//	{
//		right = sphereTraceVector3Normalize(sphereTraceVector3Cross(up, forward));
//		up = sphereTraceVector3Cross(forward, right);
//	}
//
//	return sphereTraceMatrixQuaternionFromRotationMatrix(sphereTraceMatrixConstructFromRightForwardUp(right, up, forward));
//
//}
//ST_Quaternion sphereTraceQuaternionLookAt(ST_Vector3 eye, ST_Vector3 at, ST_Vector3 up)
//{
//	ST_Vector3 forward = sphereTraceVector3Normalize(sphereTraceVector3Subtract(eye, at));
//	float dot = sphereTraceVector3Dot(forward, up);
//	ST_Vector3 right;
//	if (sphereTraceAbs(dot - 1.0f) < tolerance)
//	{
//		right = gVector3Left;
//		up = gVector3Forward;
//	}
//	else if (sphereTraceAbs(dot + 1.0f) < tolerance)
//	{
//		right = gVector3Left;
//		up = gVector3Back;
//	}
//	else
//	{
//		right = sphereTraceVector3Normalize(sphereTraceVector3Cross(up, forward));
//		up = sphereTraceVector3Cross(forward, right);
//	}
//
//	return sphereTraceMatrixQuaternionFromRotationMatrix(sphereTraceMatrixConstructFromRightForwardUp(right, up, forward));
//
//}

void sphereTraceQuaternionPrint(ST_Quaternion quat)
{
	printf("w: %f, x: %f, y: %f, z: %f\n", quat.w, quat.x, quat.y, quat.z);
}

ST_Vector3 sphereTraceVector3RotatePoint(ST_Vector3 point, ST_Quaternion rotation)
{
	ST_Quaternion rotatedQuat = sphereTraceQuaternionMultiply(sphereTraceQuaternionMultiply(rotation, sphereTraceQuaternionConstruct(0.0f, point.x, point.y, point.z )), sphereTraceQuaternionConjugate(rotation));
	return sphereTraceVector3Construct(rotatedQuat.x, rotatedQuat.y, rotatedQuat.z );
}

ST_Direction sphereTraceDirectionRotateDir(ST_Direction dir, ST_Quaternion rotation)
{
	ST_Quaternion rotatedQuat = sphereTraceQuaternionMultiply(sphereTraceQuaternionMultiply(rotation, sphereTraceQuaternionConstruct(0.0f, dir.v.x, dir.v.y, dir.v.z)), sphereTraceQuaternionConjugate(rotation));
	return sphereTraceDirectionConstruct(sphereTraceVector3Construct(rotatedQuat.x, rotatedQuat.y, rotatedQuat.z), dir.normalized);
}

ST_Matrix4 sphereTraceMatrixConstructFromRightForwardUp(ST_Vector3 right, ST_Vector3 up, ST_Vector3 forward)
{
	return sphereTraceMatrixConstruct(
		right.x, up.x, forward.x, 0.0f,
		right.y, up.y, forward.y, 0.0f,
		right.z, up.z, forward.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}


ST_Quaternion sphereTraceMatrixQuaternionFromRotationMatrix(ST_Matrix4 mat)
{
	ST_Quaternion qr;
	float tr = mat.m00 + mat.m11 + mat.m22;
		if (tr > 0) {
			float S = sqrt(tr + 1.0) * 2; // S=4*qw 
			qr.w = 0.25 * S;
			qr.x = (mat.m21 - mat.m12) / S;
			qr.y = (mat.m02 - mat.m20) / S;
			qr.z = (mat.m10 - mat.m01) / S;
		}
		else if ((mat.m00 > mat.m11) & (mat.m00 > mat.m22)) {
			float S = sqrt(1.0 + mat.m00 - mat.m11 - mat.m22) * 2; // S=4*qx 
			qr.w = (mat.m21 - mat.m12) / S;
			qr.x = 0.25 * S;
			qr.y = (mat.m01 + mat.m10) / S;
			qr.z = (mat.m02 + mat.m20) / S;
		}
		else if (mat.m11 > mat.m22) {
			float S = sqrt(1.0 + mat.m11 - mat.m00 - mat.m22) * 2; // S=4*qy
			qr.w = (mat.m02 - mat.m20) / S;
			qr.x = (mat.m01 + mat.m10) / S;
			qr.y = 0.25 * S;
			qr.z = (mat.m12 + mat.m21) / S;
		}
		else {
			float S = sqrt(1.0 + mat.m22 - mat.m00 - mat.m11) * 2; // S=4*qz
			qr.w = (mat.m10 - mat.m01) / S;
			qr.x = (mat.m02 + mat.m20) / S;
			qr.y = (mat.m12 + mat.m21) / S;
			qr.z = 0.25 * S;
		}
		return qr;
}

ST_Vector4 sphereTraceVector4ColorSetAlpha(ST_Vector4 color, float alpha)
{
	return sphereTraceVector4Construct(color.x, color.y, color.z, alpha);
}


ST_Direction sphereTraceDirectionConstruct(ST_Vector3 vec, b32 normalized)
{
	ST_Direction dr;
	dr.v = vec;
	dr.normalized = normalized;
	return dr;
}
ST_Direction sphereTraceDirectionConstruct1(float x, float y, float z, b32 normalized)
{
	ST_Direction dr;
	dr.v.x = x;
	dr.v.y = y;
	dr.v.z = z;
	dr.normalized = normalized;
	return dr;
}



ST_Direction sphereTraceDirectionConstructNormalized(ST_Vector3 vec)
{
	ST_Direction dr;
	sphereTraceVector3NormalizeByRef(&vec);
	dr.v = vec;
	dr.normalized = 1;
	return dr;
}

ST_Direction sphereTraceDirectionAdd(ST_Direction dir1, ST_Direction dir2)
{
	ST_Direction dr;
	dr.v = sphereTraceVector3Add(dir1.v, dir2.v);
	sphereTraceVector3NormalizeByRef(&dr.v);
	dr.normalized = 1;
	return dr;
}

ST_Direction sphereTraceDirectionAdd2(ST_Direction dir1, ST_Direction dir2, ST_Direction dir3, b32 normalize)
{
	ST_Direction dr;
	dr.v = sphereTraceVector3Add(sphereTraceVector3Add(dir1.v, dir2.v), dir3.v);
	if (normalize)
	{
		sphereTraceVector3NormalizeByRef(&dr.v);
	}
	dr.normalized = normalize;
	return dr;
}

void sphereTraceDirectionNormalizeIfNotNormalizedByRef(ST_Direction* const dir)
{
	if (!dir->normalized)
	{
		sphereTraceVector3NormalizeByRef(&dir->v);
		dir->normalized = 1;
	}
}

ST_Direction sphereTraceDirectionNegative(ST_Direction dir)
{
	ST_Direction dr;
	dr.v.x = -dir.v.x;
	dr.v.y = -dir.v.y;
	dr.v.z = -dir.v.z;
	dr.normalized = dir.normalized;
	return dr;
}

ST_Direction sphereTraceDirectionNormalizeIfNotNormalized(ST_Direction dir)
{
	if (!dir.normalized)
	{
		sphereTraceVector3NormalizeByRef(&dir.v);
		dir.normalized = 1;
	}
	return dir;
}

ST_Direction sphereTraceDirectionProjectDirectionOntoPlane(ST_Direction dir, ST_Direction planeNormal)
{
	float dnm = sphereTraceVector3Dot(dir.v, planeNormal.v);
	ST_Vector3 dnv = sphereTraceVector3Scale(planeNormal.v, dnm);
	ST_Vector3 dt = sphereTraceVector3Subtract(dir.v, dnv);
	return sphereTraceDirectionConstructNormalized(dt);
}

float sphereTraceDirectionGetDistanceInDirection(ST_Direction dir, ST_Vector3 from, ST_Vector3 to)
{
	sphereTraceDirectionNormalizeIfNotNormalizedByRef(&dir);
	return sphereTraceVector3Dot(dir.v, sphereTraceVector3Subtract(to, from));
}

float sphereTraceDirectionGetMagnitudeInDirection(ST_Direction dir, ST_Vector3 v)
{
	sphereTraceDirectionNormalizeIfNotNormalizedByRef(&dir);
	return sphereTraceVector3Dot(dir.v, v);
}

ST_Color sphereTraceColorConstruct(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	ST_Color cr;
	cr.r = r;
	cr.g = g;
	cr.b = b;
	cr.a = a;
	return cr;
}

ST_Vector4 sphereTraceVector4FromColor(ST_Color color)
{
	return sphereTraceVector4Construct(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
}