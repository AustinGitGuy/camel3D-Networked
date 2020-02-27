#include "Flocking.h"
#include "Unit.h"
#include "UnitManager.h"
#include "Game.h"
#include "Alignment.h"
#include "Seperation.h"
#include "Cohesion.h"
#include "WanderSteering.h"

Flocking::Flocking(const UnitID& ownerID, float distUnits){
	mType = Steering::FLOCKING;
	setOwnerID(ownerID);
	setUnitDist(distUnits);
}

Steering* Flocking::getSteering(){
	seperationWeight = gpGame->seperateWeight;
	alignmentWeight = gpGame->alignWeight;
	cohesionWeight = gpGame->cohereWeight;
	driftWeight = gpGame->driftWeight;

	Unit* pOwner = gpGame->getUnitManager()->getUnit(mOwnerID);
	PhysicsData data = pOwner->getPhysicsComponent()->getData();

	Alignment align = Alignment(mOwnerID, unitDist);
	Seperation seperate = Seperation(mOwnerID, unitDist);
	Cohesion cohere = Cohesion(mOwnerID, unitDist);

	Steering* sepSteer = seperate.getSteering();
	Steering* alignSteer = align.getSteering();
	Steering* cohereSteer = cohere.getSteering();

	Vector2D newAcc = ZERO_VECTOR2D;
	float newRotAcc = 0;

	newAcc += sepSteer->getData().acc * seperationWeight;
	newRotAcc += sepSteer->getData().rotAcc * seperationWeight;

	newAcc += alignSteer->getData().acc * alignmentWeight;
	newRotAcc += alignSteer->getData().rotAcc * alignmentWeight;

	newAcc += cohereSteer->getData().acc * cohesionWeight;
	newRotAcc += cohereSteer->getData().rotAcc * cohesionWeight;

	newAcc += driftWeight;

	data.acc = newAcc;
	data.rotAcc = newRotAcc;

	this->mData = data;
	return this;
}