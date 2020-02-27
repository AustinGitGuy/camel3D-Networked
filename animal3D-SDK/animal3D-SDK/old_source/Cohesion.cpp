#include "Cohesion.h"
#include "Unit.h"
#include "UnitManager.h"
#include "Game.h"

Cohesion::Cohesion(const UnitID& ownerID, float distUnits){
	mType = Steering::COHESION;
	setOwnerID(ownerID);
	setUnitDist(distUnits);
}

Steering* Cohesion::getSteering(){
	Unit* pOwner = gpGame->getUnitManager()->getUnit(mOwnerID);
	PhysicsData data = pOwner->getPhysicsComponent()->getData();
	Vector2D pos = ZERO_VECTOR2D;
	int neighborNum = 0;
	for (auto it = gpGame->getUnitManager()->mUnitMap.begin(); it != gpGame->getUnitManager()->mUnitMap.end(); ++it){
		if(it->first != mOwnerID && it->second != NULL){
			float distance;
			distance = (it->second->getPositionComponent()->getPosition() - pOwner->getPositionComponent()->getPosition()).getLength();
			if(distance < unitDist){
				neighborNum++;
				pos.setX(pos.getX() + it->second->getPositionComponent()->getPosition().getX());
				pos.setY(pos.getY() + it->second->getPositionComponent()->getPosition().getY());
			}
		}
	}
	if(neighborNum != 0){
		pos.setX(pos.getX() / neighborNum);
		pos.setY(pos.getY() / neighborNum);
		this->setTargetLoc(pos);
	}
	else {
		return this;
	}

	//Now just arrive towards the center
	Vector2D direction = mTargetLoc - pOwner->getPositionComponent()->getPosition();
	float distance = direction.getLength();

	//If we have arrived stop
	if(distance < 10){
		data.acc = ZERO_VECTOR2D;
		data.vel = ZERO_VECTOR2D;
		data.rotVel = 0;
		this->mData = data;
		return this;
	}

	//Since speed is just the magnitude of velocity, create a variable here then multiply later on.
	float speed;
	if(distance > 150){
		speed = data.maxSpeed;
	}
	else {
		speed = data.maxSpeed * distance / 150;
	}

	Vector2D targetVel = direction;
	targetVel.normalize();
	targetVel *= speed;

	Vector2D targetAcc = targetVel - data.vel;

	if(targetAcc.getLength() > data.maxAccMagnitude){
		targetAcc.normalize();
		targetAcc += data.maxAccMagnitude;
	}

	data.acc = targetAcc;

	this->mData = data;
	return this;
}