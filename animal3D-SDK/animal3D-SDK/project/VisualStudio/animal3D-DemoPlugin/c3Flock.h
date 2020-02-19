#pragma once

#include <iostream>
#include <vector>

struct Bird
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



private:

	std::vector<Bird> mFlock;




};