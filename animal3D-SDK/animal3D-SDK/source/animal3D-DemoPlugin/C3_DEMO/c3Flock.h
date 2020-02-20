#pragma once

#include <iostream>
#include <vector>
#include "Vector3.h"

struct Boid
{
	float xPos,yPos;
	float angle;


};


class Flock
{
public:
	Flock();
	~Flock();


	//Will iterate through vector of bird in order to update their positions
	void UpdateFlock();

	//Adds bird to Flocklist
	void addBird(float startX, float startY, float startAngle);

	void SeperateBoids();
	void AlignBoids();
	void GroupBoids();

	float calcBoidDist(const Boid* firstBoid, const Boid* secondBoid);


private:

	std::vector<Boid> mFlock;




};