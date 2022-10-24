#ifndef ACTOR_INCLUDED
#define ACTOR_INCLUDED

#include "GraphObject.h"
#include <math.h>

const int CITIZEN_SAVED_POINTS = 500;
const int DUMB_ZOMBIE_KILLED_POINTS = 1000;
const int SMART_ZOMBIE_KILLED_POINTS = 2000;
const int GOODIE_PICKED_UP_POINTS = 50;

const int CITIZEN_DEAD_POINTS = -1000;

class StudentWorld;
class Goodie;

class Actor : public GraphObject
{
public:
	Actor(StudentWorld* w, int imageID, double x, double y, int dir, int depth);
	virtual ~Actor() {};
	// Action to perform for each tick.
	virtual void doSomething() = 0;

	// Is this actor dead?
	bool isDead() const { return m_isDead; };

	// Mark this actor as dead.
	void setDead() { m_isDead = true; };

	// Get this actor's world
	StudentWorld* world() const { return pToWorld; };

	// If this is an activated object, perform its effect on a (e.g., for an
	// Exit have a use the exit).
	virtual void activateIfAppropriate(Actor* a) { return; };

	// If this object uses exits, use the exit.
	virtual void useExitIfAppropriate() { return; };

	// If this object can die by falling into a pit or burning, die.
	virtual void dieByFallOrBurnIfAppropriate() { return; };

	// If this object can be infected by vomit, get infected.
	virtual void beVomitedOnIfAppropriate() { return; };

	// If this object can pick up goodies, pick up g
	virtual void pickUpGoodieIfAppropriate(Goodie* g) { return; };

	// Does this object block agent movement?
	virtual bool blocksMovement() const { return false; };

	// Does this object block flames?
	virtual bool blocksFlame() const { return false; };

	// Can this object cause a zombie to vomit?
	virtual bool triggersZombieVomit() const { return false; };

	// Does this object trigger landmines only when they're active?
	virtual bool triggersOnlyActiveLandmines() const { return false; };

	// Is this object a threat to citizens?
	virtual bool threatensCitizens() const { return false; };

	// Does this object trigger citizens to follow it or flee it?
	virtual bool triggersCitizens() const { return false; };
	virtual bool isCitizen() { return false; }

	bool isAt(double x, double y) const;

	bool isOverlap(double x, double y) const    //check if 2 sprites overlap
	{
		return (pow((getX() - x), 2) + pow((getY() - y), 2) <= 100);
	};

private:
	bool m_isDead;
	StudentWorld* pToWorld;
};

//**************************************************************************************************************************************************
//**************************************************************************************************************************************************
//**************************************************************************************************************************************************
//**************************************************************************************************************************************************
//**************************************************************************************************************************************************

class Wall : public Actor
{
public:
	Wall(StudentWorld* w, double x, double y);
	virtual ~Wall() {};
	virtual void doSomething() { return; };
	virtual bool blocksMovement() const { return true; };
	virtual bool blocksFlame() const { return true; };
	//cannot be damaged by flame or infected:  beVomitedOnIfAppropriate returns false, dieByFallOrBurnIfAppropriate returns false
	//blocks movement of all agents
	//blocks flames
};

//*********************************************************************************************************************************************
//*********************************************************************************************************************************************

//have function ActivateIfAppropriate
class ActivatingObject : public Actor    
{
public:

	ActivatingObject(StudentWorld* w, int imageID, double x, double y, int depth, int dir);
	virtual ~ActivatingObject() {};

};

//*********************************************************************************************************************************************

class Exit : public ActivatingObject
{
public:
	Exit(StudentWorld* w, double x, double y);
	virtual ~Exit() {};
	virtual void doSomething();
	virtual void activateIfAppropriate(Actor* a);    //a is AGENT
	virtual bool blocksFlame() const { return true; };
};

//**************************************************************************************************

class Pit : public ActivatingObject
{
public:
	Pit(StudentWorld* w, double x, double y);
	virtual ~Pit() {};
	virtual void doSomething();
	virtual void activateIfAppropriate(Actor* a);
};

//**************************************************************************************************

class Flame : public ActivatingObject
{
public:
	Flame(StudentWorld* w, double x, double y, int dir);
	virtual ~Flame() {};
	virtual void doSomething();
	virtual void activateIfAppropriate(Actor* a);
private:
	int ticksFromCreation;
};

//**************************************************************************************************

class Vomit : public ActivatingObject
{
public:
	Vomit(StudentWorld* w, double x, double y);
	virtual ~Vomit() {};
	virtual void doSomething();
	virtual void activateIfAppropriate(Actor* a);
private:
	int ticksFromCreation;
};

//**************************************************************************************************

class Landmine : public ActivatingObject
{
public:
	Landmine(StudentWorld* w, double x, double y);
	virtual ~Landmine() {};
	virtual void doSomething();
	virtual void activateIfAppropriate(Actor* a);
	virtual void dieByFallOrBurnIfAppropriate();

	bool isActiveState() { return m_activeState; };
private:
	int m_safetyTicksLeft;
	bool m_activeState;
};

//*********************************************************************************************************************************************
//*********************************************************************************************************************************************

class Agent : public Actor
{
public:
	Agent(StudentWorld* w, int imageID, double x, double y, int dir);
	virtual ~Agent() {};
	virtual bool blocksMovement() const { return true; };
	virtual bool triggersOnlyActiveLandmines() const { return true; };
	bool isEvenTick() const { return m_isEvenTick; };
	void setIsEvenTick()
	{
		if (m_isEvenTick)
			m_isEvenTick = false;
		else
			m_isEvenTick = true;
	};
private:
	bool m_isEvenTick;
};

class Human : public Agent
{
public:
	Human(StudentWorld* w, int imageID, double x, double y);
	virtual ~Human() {};

	virtual bool triggersZombieVomit() const { return true; };

	// Make this human uninfected by vomit.
	void clearInfection() 
	{ 
		m_infectionDuration = 0;
		m_isInfected = false;
	}

	// How many ticks since this human was infected by vomit?
	int getInfectionDuration() const { return m_infectionDuration; };

	bool isInfected() const { return m_isInfected; };
	void setInfection() { m_isInfected = true; };
	void increaseInfectionDuration() { m_infectionDuration++; };	

private:
	bool m_isInfected;
	int m_infectionDuration;


};

//*********************************************************************************************************************************************


class Penelope : public Human
{
public:
	Penelope(StudentWorld* w, double x, double y);
	virtual ~Penelope() {};

	virtual void doSomething();
	virtual void useExitIfAppropriate();
	virtual void dieByFallOrBurnIfAppropriate();
	virtual void pickUpGoodieIfAppropriate(Goodie* g);

	// Increase the number of vaccines the object has.
	void increaseVaccines() { m_numVaccines++; };

	// Increase the number of flame charges the object has.
	void increaseFlameCharges() { m_numFlameCharges += 5; };

	// Increase the number of landmines the object has.
	void increaseLandmines() { m_numLandmines += 2; };

	// How many vaccines does the object have?
	int getNumVaccines() const { return m_numVaccines; };

	// How many flame charges does the object have?
	int getNumFlameCharges() const { return m_numFlameCharges; };

	// How many landmines does the object have?
	int getNumLandmines() const { return m_numLandmines; };

	virtual bool triggersCitizens() const { return true; };

	virtual void beVomitedOnIfAppropriate();
private:
	int m_numVaccines;
	int m_numFlameCharges;
	int m_numLandmines;
};

class Citizen : public Human
{
public:
	Citizen(StudentWorld* w, double x, double y);
	virtual ~Citizen() {};

	virtual void doSomething();
	virtual void useExitIfAppropriate();
	virtual void dieByFallOrBurnIfAppropriate();

	virtual void beVomitedOnIfAppropriate();
	virtual bool isCitizen() { return true; }
};

//*********************************************************************************************************************************************
//*********************************************************************************************************************************************

class Zombie : public Agent
{
public:
	Zombie(StudentWorld* w, double x, double y);
	virtual ~Zombie() {};

	virtual bool threatensCitizens() const { return true; };

	double getMovementPlanDist() const { return m_movementPlanDist; }
	void decMovementPlanDist() { m_movementPlanDist--; }
	void setMovementPlanDist(double n) { m_movementPlanDist = n; }
	void incMovementPlanDist(double n) { m_movementPlanDist += n; }
	void moveInDirectionIfNotBlocked();

	void vomitIfAppropriate();
private:
	bool m_isEvenTick;
	double m_movementPlanDist;
};

//*********************************************************************************************************************************************

class DumbZombie : public Zombie
{
public:
	DumbZombie(StudentWorld* w, double x, double y);
	virtual ~DumbZombie() {};

	virtual void doSomething();
	virtual void dieByFallOrBurnIfAppropriate();
};

class SmartZombie : public Zombie
{
public:
	SmartZombie(StudentWorld* w, double x, double y);
	virtual ~SmartZombie() {};

	virtual void doSomething();
	virtual void dieByFallOrBurnIfAppropriate();
};

//*********************************************************************************************************************************************
//*********************************************************************************************************************************************


class Goodie : public ActivatingObject
{
public:
	Goodie(StudentWorld* w, int imageID, double x, double y);    //added imageID
	virtual ~Goodie() {};

	virtual void activateIfAppropriate(Actor* a);
	virtual void dieByFallOrBurnIfAppropriate();
	virtual void doSomething();

	// Have p pick up this goodie.
	virtual void pickUp(Penelope* p) = 0;
};

//*********************************************************************************************************************************************

class VaccineGoodie : public Goodie
{
public:
	VaccineGoodie(StudentWorld* w, double x, double y);
	virtual ~VaccineGoodie() {};

	
	virtual void pickUp(Penelope* p);
};

class GasCanGoodie : public Goodie
{
public:
	GasCanGoodie(StudentWorld* w, double x, double y);
	virtual ~GasCanGoodie() {};

	virtual void pickUp(Penelope* p);
};

class LandmineGoodie : public Goodie
{
public:
	LandmineGoodie(StudentWorld* w, double x, double y);
	virtual ~LandmineGoodie() {};

	virtual void pickUp(Penelope* p);
};


#endif // ACTOR_INCLUDED