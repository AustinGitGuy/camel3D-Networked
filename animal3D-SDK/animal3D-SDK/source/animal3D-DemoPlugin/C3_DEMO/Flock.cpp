#include "c3Flock.h"

Flock::Flock()
{
}

Flock::~Flock()
{
}

void Flock::UpdateFlock()
{

}

void Flock::addBird(float startX, float startY, float startAngle)
{
	Boid bird = Boid();

	//Add starting data to Bird
	bird.xPos = startX;
	bird.yPos = startY;

	bird.angle = startAngle;

	mFlock.push_back(bird);

}

void Flock::SeperateBoids()
{

}

void Flock::AlignBoids()
{

}

void Flock::GroupBoids()
{

}

float Flock::calcBoidDist(const Boid* firstBoid, const Boid* secondBoid)
{
	float boidDist = 0.0;

	


	return boidDist;
}
