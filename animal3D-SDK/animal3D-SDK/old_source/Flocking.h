#include "Steering.h"
#include "Trackable.h"

class Flocking: public Steering {
public:
	Flocking(const UnitID& ownerID, float distUnits);
	void setUnitDist(const float& distUnits) { unitDist = distUnits; };
	void setSepWeight(const float& sepWeight){seperationWeight = sepWeight;};
	void setAlignWeight(const float& alignWeight){alignmentWeight = alignWeight;};
	void setCohesionWeight(const float& cohereWeight){cohesionWeight = cohereWeight;};
	void setWanderWeight(const float& wandWeight){driftWeight = wandWeight;};

private:
	virtual Steering* getSteering();
	float unitDist;
	float seperationWeight;
	float alignmentWeight;
	float cohesionWeight;
	float driftWeight;
};