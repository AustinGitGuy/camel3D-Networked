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
	Bird bird = Bird();

	//Add starting data to Bird
	bird.xPos = startX;
	bird.yPos = startY;

	bird.angle = startAngle;

	mFlock.push_back(bird);

}
