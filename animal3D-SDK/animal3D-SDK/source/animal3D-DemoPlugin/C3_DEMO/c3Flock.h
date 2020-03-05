#pragma once

#include <iostream>
#include <vector>
#include "Vector3.h"


const int FLOCK_SIZE = 45;
const float MAX_SPEED = 5;
const float MAX_ACCEL = 3;
const int FLOCK_DISTANCE = 125;

class Flock
{
public:
	Flock();
	~Flock();


	//Will iterate through vector of boids in order to update their positions
	void UpdateFlock(int width, int height);

	void DrawFlock(int width, int height);

	//Adds bird to Flocklist
	void addBird(Vector3 position, bool isLocal = true, int index = -1);

	//Access functions for boids
	Vector3 getBoidPosition(int index);
	Vector3 getBoidVelocity(int index);
	Vector3 getBoidAcceleration(int index);
	bool checkLocalBoid(int index);

	void setBoidPosition(int index, Vector3 position){ positions[index] = position; }
	void setBoidVelocity(int index, Vector3 velocity) { velocities[index] = velocity; }
	void setBoidAcceleration(int index, Vector3 acceleration) { accels[index] = acceleration; }
	
	int GetPositionIndex(){ return positionIndex; }

	Vector3 Seperation(int boidNum);
	Vector3 Cohesion(int boidNum);
	Vector3 Alignment(int boidNum);

private:
	Vector3 positions[FLOCK_SIZE];
	Vector3 velocities[FLOCK_SIZE];
	Vector3 accels[FLOCK_SIZE];
	bool localBoid[FLOCK_SIZE];

	int positionIndex = 0;

	float lastFrame;

};