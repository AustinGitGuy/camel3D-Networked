#pragma once

#include <iostream>
#include <vector>
#include "Vector3.h"

const int FLOCK_SIZE = 20;
const float MAX_SPEED = 3;

class Flock
{
public:
	Flock();
	~Flock();


	//Will iterate through vector of boids in order to update their positions
	void UpdateFlock();

	void DrawFlock(int width, int height);

	//Adds bird to Flocklist
	void addBird(Vector3 position);

	Vector3 Seperation(int boidNum);
	Vector3 Cohesion(int boidNum);
	Vector3 Alignment(int boidNum);

private:
	Vector3 colors[FLOCK_SIZE];
	Vector3 positions[FLOCK_SIZE];
	Vector3 velocities[FLOCK_SIZE];
	Vector3 accels[FLOCK_SIZE];

	int positionIndex = 0;
};