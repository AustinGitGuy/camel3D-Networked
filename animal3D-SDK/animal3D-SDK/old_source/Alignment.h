#include "Steering.h"
#include "Trackable.h"

class Alignment: public Steering {
public:
	Alignment(const UnitID& ownerID, float distUnits);
	void setUnitDist(const float& distUnits) { unitDist = distUnits; };
	virtual Steering* getSteering();

private:
	float unitDist;
};