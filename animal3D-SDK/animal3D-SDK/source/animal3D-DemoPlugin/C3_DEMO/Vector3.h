#pragma once

class Vector3 {
public:
	Vector3(float x = 0.0f, float y = 0.0f, float z = 0.0f);

	Vector3& operator += (const Vector3& mth);
	Vector3& operator -= (const Vector3& mth);
	Vector3& operator *= (float mth);
	Vector3& operator /= (float mth);
	Vector3& operator = (const Vector3& mth);

	bool operator == (const Vector3& mth);
	bool operator != (const Vector3& mth);

	Vector3 operator+(const Vector3 &mth) const;
	Vector3 operator-(const Vector3 &mth) const;
	Vector3 operator*(float mth) const;

	float x;
	float y;
	float z;
};

const Vector3 VECTOR3_ZERO(0.0f, 0.0f, 0.0f);

const Vector3 VECTOR3_ONE(1.0f, 1.0f, 1.0f);

void CrossProduct(Vector3 one, Vector3 two, Vector3* product);

Vector3 ClampVector(Vector3 clampValue, float clampLimit);

Vector3 Lerp(Vector3 one, Vector3 two, float time);