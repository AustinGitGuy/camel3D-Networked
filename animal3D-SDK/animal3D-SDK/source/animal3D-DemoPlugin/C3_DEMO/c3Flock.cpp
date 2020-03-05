#include "c3Flock.h"

#include <GL/glew.h>
#include <ctime>
#include <time.h>


Flock::Flock()
{
	lastFrame = (float)clock() / CLOCKS_PER_SEC;
}

Flock::~Flock()
{
}

void Flock::UpdateFlock(int width, int height)
{
	float currentFrame, deltaTime;

	currentFrame = (float)clock() / CLOCKS_PER_SEC;

	deltaTime = currentFrame - lastFrame;

	lastFrame = currentFrame;
	
	//Updating Velocities
	for(int i = 0; i < positionIndex; i++){
		
		accels[i] += Cohesion(i) * .1f + Seperation(i) * .05f + Alignment(i) * .1f;
		velocities[i] = velocities[i] + accels[i] * deltaTime;
			
		//Velocity Checks
		velocities[i] = ClampVector(velocities[i], MAX_SPEED);
		accels[i] = ClampVector(accels[i], MAX_ACCEL);
			
		//Calc final position usign physics equation
		positions[i] = positions[i] + velocities[i] * deltaTime +  accels[i] * (deltaTime * deltaTime) * 0.5;
			
		//Boids Wrap around screen
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

		pos.x = (positions[i].x - width / 2) / (width / 2);

		pos.y = (positions[i].y - height / 2) / (height / 2);

		glVertex2f(pos.x - .025f, pos.y + .05f);
		glVertex2f(pos.x - .025f, pos.y - .05f);
		glVertex2f(pos.x + .025f, pos.y - .05f);
		glVertex2f(pos.x + .025f, pos.y + .05f);
		glEnd();
	}
}

void Flock::addBird(Vector3 position, bool isLocal, int index)
{
	if(index == -1){
		//Add starting data to Bird
		positions[positionIndex] = position;

		localBoid[positionIndex] = isLocal;
	}
	else {
		positions[index] = position;
		localBoid[index] = isLocal;

		if(positionIndex < index){
			positionIndex = index;
		}
	}

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