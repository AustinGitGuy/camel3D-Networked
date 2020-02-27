#include "Seperation.h"
#include "Unit.h"
#include "UnitManager.h"
#include "Game.h"

Seperation::Seperation(const UnitID& ownerID, float distUnits){
	mType = Steering::SEPERATION;
	setOwnerID(ownerID);
	setUnitDist(distUnits);
}

Steering* Seperation::getSteering(){
	Unit* pOwner = gpGame->getUnitManager()->getUnit(mOwnerID);
	PhysicsData data = pOwner->getPhysicsComponent()->getData();
	Vector2D pos = ZERO_VECTOR2D;
	int neighborNum = 0;
	for (auto it = gpGame->getUnitManager()->mUnitMap.begin(); it != gpGame->getUnitManager()->mUnitMap.end(); ++it){
		if(it->first != mOwnerID && it->second != NULL){
			float distance;
			Vector2D direction;
			direction = it->second->getPositionComponent()->getPosition() - pOwner->getPositionComponent()->getPosition();
			distance = direction.getLength();
			if(distance < unitDist){
				direction.normalize();
				float modifier = std::fmin(100 / distance, pOwner->getMaxAcc());
				pos += direction * modifier;
				neighborNum++;
			}
		}
	}
	if(neighborNum != 0){
		data.acc = pos;
	}
	else {
		data.acc = ZERO_VECTOR2D;
		data.vel = ZERO_VECTOR2D;
		data.rotVel = 0;
	}
	this->mData = data;
	return this;
}