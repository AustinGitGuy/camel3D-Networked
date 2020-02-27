#include "Steering.h"
#include "Trackable.h"

class Seperation: public Steering {
public:
	Seperation(const UnitID& ownerID, float distUnits);
	void setUnitDist(const float& distUnits) { unitDist = distUnits; };
	virtual Steering* getSteering();

private:
	float unitDist;
};