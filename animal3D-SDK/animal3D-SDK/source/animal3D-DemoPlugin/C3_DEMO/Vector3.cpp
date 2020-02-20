#include "Vector3.h"
#include <cmath>

Vector3::Vector3(float nX, float nY, float nZ):x(nX),y(nY),z(nZ){}

Vector3& Vector3::operator += (const Vector3& mth){
	x += mth.x;
	y += mth.y;
	z += mth.z;
	return *this;
}

Vector3& Vector3::operator -= (const Vector3& mth){
	x -= mth.x;
	y -= mth.y;
	z -= mth.z;
	return *this;
}

Vector3& Vector3::operator = (const Vector3& mth){
	x = mth.x;
	y = mth.y;
	z = mth.z;
	return *this;
}

Vector3& Vector3::operator *= (float mth){
	x *= mth;
	y *= mth;
	z *= mth;
	return *this;
}

Vector3& Vector3::operator /= (float mth){
	x /= mth;
	y /= mth;
	z /= mth;
	return *this;
}

Vector3 Vector3::operator+(const Vector3 &other) const {
	Vector3 result = *this;
	result += other;
	return result;
}

Vector3 Vector3::operator-(const Vector3 &other) const {
	Vector3 result = *this;
	result -= other;
	return result;
}

Vector3 Vector3::operator*(float mth) const {
	Vector3 result = *this;     
	result.x *= mth;
	result.y *= mth;
	result.z *= mth;

	return result;
}

bool Vector3::operator==(const Vector3& mth){
	if((x == mth.y) && (y == mth.y) && (z == mth.z)){
		return true;
	}
	else return false;
}

bool Vector3::operator!=(const Vector3& mth){
	if((x == mth.x) && (y == mth.y) && (z == mth.z)){
		return false;
	}
	else return true;
}

void CrossProduct(Vector3 one, Vector3 two, Vector3* product){
	product->x = one.y * two.z - one.z * two.y; 
	product->y = one.x * two.z - one.z * two.x; 
	product->z = one.x * two.y - one.y * two.x; 
}

Vector3 Lerp(Vector3 one, Vector3 two, float time){
	Vector3 result(0, 0, 0);
	result.x = (1 - time) * one.x + time * two.x;
	result.y = (1 - time) * one.y + time * two.y;
	result.z = (1 - time) * one.z + time * two.z;
	return result;
}
