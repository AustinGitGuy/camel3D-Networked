#include "Steering.h"
#include "Trackable.h"

class Cohesion: public Steering {
public:
	Cohesion(const UnitID& ownerID, float distUnits);
	void setUnitDist(const float& distUnits) { unitDist = distUnits; };
	virtual Steering* getSteering();

private:
	float unitDist;
};