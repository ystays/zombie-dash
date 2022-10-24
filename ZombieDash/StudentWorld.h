#ifndef STUDENTWORLD_INCLUDED
#define STUDENTWORLD_INCLUDED

#include "GameWorld.h"
#include <string>
#include <list>

#include <iostream>

class Actor;
class Penelope;

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir);
	virtual ~StudentWorld() { cleanUp(); };

	virtual int init();
	virtual int move();
	virtual void cleanUp();

	// Add an actor to the world.
	void addActor(Actor* a);

	// Indicate that the player has finished the level if all citizens
	// are gone.
	void recordLevelFinishedIfAllCitizensGone();

	// For each actor overlapping a, activate a if appropriate.
	void activateOnAppropriateActors(Actor* a);

	// Is an agent blocked from moving to the indicated location?
	bool isAgentMovementBlockedAt(double x, double y, Actor* a) const;

	// Is creation of a flame blocked at the indicated location?
	bool isFlameBlockedAt(double x, double y) const;

	int numCitizensLeft() const { return m_numCitizensLeft; };

	void allowCitizenExit(double x, double y);

	void changeGameStatus(int a) { m_gameStatus = a; };

	void vomitOnTarget(double x, double y);

	// Return true if there is a living human, otherwise false.  If true,
	 // otherX, otherY, and distance will be set to the location and distance
	 // of the human nearest to (x,y).
	bool locateNearestVomitTrigger(double x, double y, double& otherX, double& otherY, double& distance);

	// Return true if there is a living zombie or Penelope, otherwise false.
	// If true, otherX, otherY, and distance will be set to the location and
	// distance of the one nearest to (x,y), and isThreat will be set to true
	// if it's a zombie, false if a Penelope.
	bool locateNearestCitizenTrigger(double x, double y, double& otherX, double& otherY, double& distance, bool& isThreat) const;

	// Return true if there is a living zombie, false otherwise.  If true,
	// otherX, otherY and distance will be set to the location and distance
	// of the one nearest to (x,y).
	bool locateNearestCitizenThreat(double x, double y, double& otherX, double& otherY, double& distance) const;


private:
	Penelope* player;
	std::list<Actor*> li;
	int m_gameStatus;
	int m_numCitizensLeft;
};

#endif // STUDENTWORLD_INCLUDED