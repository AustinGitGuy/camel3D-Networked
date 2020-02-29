#include "c3Flock.h"

#include <GL/glew.h>

Flock::Flock()
{
}

Flock::~Flock()
{
}

void Flock::UpdateFlock()
{
	for(int i = 0; i < positionIndex; i++){
		accels[i] += Cohesion(i) * .05f + Seperation(i) * .1f + Alignment(i) * .1f;
		velocities[i] += accels[i];

		if(velocities[i].x > MAX_SPEED){
			velocities[i].x = MAX_SPEED;
		}

		if(velocities[i].y > MAX_SPEED){
			velocities[i].y = MAX_SPEED;
		}

		if(velocities[i].z > MAX_SPEED){
			velocities[i].z = MAX_SPEED;
		}

		if(velocities[i].x < -MAX_SPEED){
			velocities[i].x = -MAX_SPEED;
		}

		if(velocities[i].y < -MAX_SPEED){
			velocities[i].y = -MAX_SPEED;
		}

		if(velocities[i].z < -MAX_SPEED){
			velocities[i].z = -MAX_SPEED;
		}

		positions[i] += velocities[i];
	}
}

void Flock::DrawFlock(int width, int height){
	for(int i = 0; i < positionIndex; i++){
		if(localBoid[i]){
			glColor3f(1, 0, 0);
		}
		else {
			glColor3f(0, 0, 1);
		}
		glBegin(GL_QUADS);
		Vector3 pos;

		if(positions[i].x < 0){
			positions[i].x = width + positions[i].x;
		}
		else if(positions[i].x > width){
			positions[i].x = positions[i].x - width;
		}

		if(positions[i].y < 0){
			positions[i].y = height + positions[i].y;
		}
		else if(positions[i].y > height){
			positions[i].y = positions[i].y - height;
		}

		pos.x = (positions[i].x - width / 2) / (width / 2);

		pos.y = (positions[i].y - height / 2) / (height / 2);

		glVertex2f(pos.x - .025f, pos.y + .05f);
		glVertex2f(pos.x - .025f, pos.y - .05f);
		glVertex2f(pos.x + .025f, pos.y - .05f);
		glVertex2f(pos.x + .025f, pos.y + .05f);
		glEnd();
	}
}

void Flock::addBird(Vector3 position, bool isLocal)
{
	//Add starting data to Bird
	positions[positionIndex] = position;

	//Put the color here
	localBoid[positionIndex] = isLocal;

	positionIndex++;
}

Vector3 Flock::getBoidPosition(int index)
{
	return positions[index];
}

Vector3 Flock::getBoidVelocity(int index)
{
	return velocities[index];
}

Vector3 Flock::getBoidAcceleration(int index)
{
	return accels[index];
}

bool Flock::checkLocalBoid(int index)
{
	return localBoid[index];
}

Vector3 Flock::Seperation(int boidNum)
{
	Vector3 pos = VECTOR3_ZERO;
	int neighborNum = 0;
	for(int i = 0; i < positionIndex; i++){
		float distance;
		distance = sqrt(((positions[i].x - positions[boidNum].x) * (positions[i].x - positions[boidNum].x)) + ((positions[i].y - positions[boidNum].y) * (positions[i].y - positions[boidNum].y)));
		if(distance < FLOCK_DISTANCE){
			neighborNum++;
			pos.x += positions[i].x;
			pos.y += positions[i].y;
		}
	}

	if(neighborNum != 0){
		pos.x = pos.x / neighborNum;
		pos.y = pos.y / neighborNum;
	}
	else {
		return VECTOR3_ZERO;
	}

	Vector3 direction = pos - positions[boidNum];
	float distance = sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);

	direction /= distance;

	if(distance < 5){
		//Stop here
		return VECTOR3_ZERO;
	}

	//Update position
	return direction * -1;
}

Vector3 Flock::Alignment(int boidNum){
	Vector3 vel = VECTOR3_ZERO;

	int neighborNum = 0;
	for(int i = 0; i < positionIndex; i++){
		float distance;
		distance = sqrt(((positions[i].x - positions[boidNum].x) * (positions[i].x - positions[boidNum].x)) + ((positions[i].y - positions[boidNum].y) * (positions[i].y - positions[boidNum].y)));
		if(distance < FLOCK_DISTANCE){
			neighborNum++;
			vel.x += velocities[i].x;
			vel.y += velocities[i].y;
		}
	}

	if(vel != VECTOR3_ZERO){
		vel.x = vel.x / neighborNum;
		vel.y = vel.y / neighborNum;

		float velLength = sqrt(vel.x * vel.x + vel.y * vel.y + vel.z * vel.z);

		vel /= velLength;

		return vel;
	}
	else {
		return VECTOR3_ZERO;
	}
}

Vector3 Flock::Cohesion(int boidNum)
{
	Vector3 pos = VECTOR3_ZERO;
	int neighborNum = 0;
	for(int i = 0; i < positionIndex; i++){
		float distance;
		distance = sqrt(((positions[i].x - positions[boidNum].x) * (positions[i].x - positions[boidNum].x)) + ((positions[i].y - positions[boidNum].y) * (positions[i].y - positions[boidNum].y)));
		if(distance < FLOCK_DISTANCE){
			neighborNum++;
			pos.x += positions[i].x;
			pos.y += positions[i].y;
		}
	}

	if(neighborNum != 0){
		pos.x = pos.x / neighborNum;
		pos.y = pos.y / neighborNum;
	}
	else {
		return VECTOR3_ZERO;
	}

	Vector3 direction = pos - positions[boidNum];
	float distance = sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);

	direction /= distance;

	if(distance < 5){
		//Stop here
		return VECTOR3_ZERO;
	}

	//Update position
	return direction;
}