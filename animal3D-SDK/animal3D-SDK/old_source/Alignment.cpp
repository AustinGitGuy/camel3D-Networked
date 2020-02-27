#include "Alignment.h"
#include "Unit.h"
#include "UnitManager.h"
#include "Game.h"

Alignment::Alignment(const UnitID& ownerID, float distUnits){
	mType = Steering::ALIGNMENT;
	setOwnerID(ownerID);
	setUnitDist(distUnits);
}

Steering* Alignment::getSteering(){
	Unit* pOwner = gpGame->getUnitManager()->getUnit(mOwnerID);
	PhysicsData data = pOwner->getPhysicsComponent()->getData();
	Vector2D velo = ZERO_VECTOR2D;
	int neighborNum = 0;
	for (auto it = gpGame->getUnitManager()->mUnitMap.begin(); it != gpGame->getUnitManager()->mUnitMap.end(); ++it){
		if(it->first != mOwnerID && it->second != NULL){
			float distance;
			distance = (it->second->getPositionComponent()->getPosition() - pOwner->getPositionComponent()->getPosition()).getLength();
			if(distance < unitDist){
				neighborNum++;
				velo += it->second->getPhysicsComponent()->getVelocity();
			}
		}
	}
	if(neighborNum != 0){
		velo /= neighborNum;
		velo.normalize();
		data.acc = velo;
	}
	else {
		data.acc = ZERO_VECTOR2D;
		data.vel = ZERO_VECTOR2D;
		data.rotVel = 0;
	}
	this->mData = data;
	return this;
}