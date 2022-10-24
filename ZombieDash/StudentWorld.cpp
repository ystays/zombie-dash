#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>

#include <iostream>
#include <sstream>
#include <iomanip>

#include <math.h>
#include "Level.h"
#include "Actor.h"
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

StudentWorld::StudentWorld(string assetPath)
	: GameWorld(assetPath)
{
	m_gameStatus = GWSTATUS_CONTINUE_GAME;
	m_numCitizensLeft = 0;
}

int StudentWorld::init()
{
	m_gameStatus = GWSTATUS_CONTINUE_GAME;
	Level lev(assetPath());
	int level = getLevel();

	//use stringstream to get level file name
	ostringstream levelname;
	levelname << "level";
	levelname.fill('0');
	levelname << setw(2) << level;
	levelname << ".txt";

	string levelFile = levelname.str();
	cerr << levelFile << endl;
	Level::LoadResult result = lev.loadLevel(levelFile);

	if (result == Level::load_fail_file_not_found || getLevel() == 100)
	{
		cerr << "Cannot find " << levelFile << " data file" << endl;
		return GWSTATUS_PLAYER_WON;
	}
	else if (result == Level::load_fail_bad_format)
	{
		cerr << "Your level was improperly formatted" << endl;
		return GWSTATUS_LEVEL_ERROR;
	}
	else if (result == Level::load_success)
	{
		cerr << "Successfully loaded level" << endl;
		for (int i = 0; i < LEVEL_WIDTH; i++)
		{
			for (int j = 0; j < LEVEL_HEIGHT; j++)
			{
				Level::MazeEntry ge = lev.getContentsOf(i, j);  // level_x=i, level_y=j         
				int x = i * SPRITE_WIDTH;
				int y = j * SPRITE_HEIGHT;

				switch (ge)  // so x=i * 16 and y=j * 16   
				{
				case Level::empty:
				{
					cout << "Location " << x << ", " << y << " is empty" << endl;
					break;
				}
				case Level::smart_zombie:
				{	
					//allocate new smart_zombie in vi
					addActor(new SmartZombie(this, x, y));
					cout << "Location " << x << ", " << y << " starts with a smart zombie" << endl;
					break;
				}
				case Level::dumb_zombie:
				{
					addActor(new DumbZombie(this, x, y));
					cout << "Location " << x << ", " << y << " starts with a dumb zombie" << endl;
					break;
				}
				case Level::player:
				{
					player = new Penelope(this, x, y);
					cout << "Location " << x << ", " << y << " is where Penelope starts" << endl;
					break;
				}
				case Level::citizen:
				{
					addActor(new Citizen(this, x, y));
					m_numCitizensLeft++;
					break;
				}
				case Level::vaccine_goodie:
				{
					addActor(new VaccineGoodie(this, x, y));
					break;
				}
				case Level::gas_can_goodie:
				{
					addActor(new GasCanGoodie(this, x, y));
					break;
				}
				case Level::landmine_goodie:
				{
					addActor(new LandmineGoodie(this, x, y));
					break;
				}
				case Level::exit:
				{
					addActor(new Exit(this, x, y));
					cout << "Location " << x << ", " << y << " is where an exit is" << endl;
					break;
				}
				case Level::wall:
				{
					addActor(new Wall(this, x, y));
					cout << "Location " << x << ", " << y << " holds a Wall" << endl;
					break;
				}
				case Level::pit:
				{
					addActor(new Pit(this, x, y));
					cout << "Location " << x << ", " << y << " has a pit in the ground" << endl;
					break;
				}
				}
			}
		}
	}

	return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
	ostringstream text;
	text << "Score: ";
	if (getScore() >= 0)
	{
		text.fill('0');
		text << setw(6) << getScore() << "  ";
	}
	else
	{
		text << "-";
		text.fill('0');
		text << setw(5) << -1 * getScore() << "  ";
	}
	text << "Level: ";
	text << getLevel() << "  ";
	text << "Lives: ";
	text << getLives() << "  ";
	text << "Vaccines: ";
	text << player->getNumVaccines() << "  ";
	text << "Flames: ";
	text << player->getNumFlameCharges() << "  ";
	text << "Mines: ";
	text << player->getNumLandmines() << "  ";
	text << "Infected: ";
	text << player->getInfectionDuration() << "  ";


	string gameStatText = text.str();
	setGameStatText(gameStatText);

	if (player->isDead())
	{
		changeGameStatus(GWSTATUS_PLAYER_DIED);
		decLives();
	}
	else
	{
		player->doSomething();
	}
	int k = li.size();
	for (list<Actor*>::iterator p = li.begin(); p != li.end() && k != 0; k--)
	{
		if ((*p)->isDead())
		{
			delete *p;
			p = li.erase(p);
		}
		else
		{
			(*p)->doSomething();
			p++;
		}
	}
	return m_gameStatus;
}

//*******************************************

void StudentWorld::cleanUp()
{

	//delete Actors
	for (list<Actor*>::iterator p = li.begin(); p != li.end(); )
	{
		delete *p;
		p = li.erase(p);
	}
	//delete Penelope object
	if (player != nullptr)
	{
		delete player;
	}
	player = nullptr;
}

void StudentWorld::addActor(Actor* a)
{
	li.push_back(a);
}


bool StudentWorld::isAgentMovementBlockedAt(double x, double y, Actor* a) const
{
	if (a != player && player->isAt(x, y))
		return true;
	for (list<Actor*>::const_iterator p = li.begin(); p != li.end(); p++)
	{
		if ((*p) != a && ((*p)->isAt(x, y)) && ((*p)->blocksMovement()))
		{
			return true;
		}
	}
	return false;
}

bool StudentWorld::isFlameBlockedAt(double x, double y) const
{
	//Penelope object does not block flames
	for (list<Actor*>::const_iterator p = li.begin(); p != li.end(); p++)
	{
		if ((*p)->isAt(x, y) && (*p)->blocksFlame())
			return true;
	}
	return false;
}

void StudentWorld::recordLevelFinishedIfAllCitizensGone()
{
	for (list<Actor*>::const_iterator p = li.begin(); p != li.end(); p++)
	{
		if ((*p)->isCitizen())
			return;
	}
	m_gameStatus = GWSTATUS_FINISHED_LEVEL;
}

void StudentWorld::allowCitizenExit(double x, double y)
{
	for (list<Actor*>::iterator p = li.begin(); p != li.end(); p++)
	{
		if ((*p)->isOverlap(x, y))
			(*p)->useExitIfAppropriate();
	}
}


void StudentWorld::vomitOnTarget(double x, double y)
{
	for (list<Actor*>::const_iterator p = li.begin(); p != li.end(); p++)
	{
		if ((*p)->getX() == x && (*p)->getY() == y)
			(*p)->beVomitedOnIfAppropriate();
	}
}


//Activates a if agent overlaps a
void StudentWorld::activateOnAppropriateActors(Actor* a)
{
	if (a->isOverlap(player->getX(), player->getY()))
	{
		a->activateIfAppropriate(player);
	}

	for (list<Actor*>::iterator p = li.begin(); p != li.end(); p++)
	{
		if ((*p) != a && a->isOverlap((*p)->getX(), (*p)->getY()))
		{
			a->activateIfAppropriate(*p);    //if landmine, will add 9 new Actors
		}
	}
}

// Return true if there is a living human, otherwise false.  If true,
// otherX, otherY, and distance will be set to the location and distance
// of the human nearest to (x,y).
bool StudentWorld::locateNearestVomitTrigger(double x, double y, double& otherX, double& otherY, double& distance)
{
	int numHumans = 0;
	double dist_p = (sqrt(pow(VIEW_WIDTH, 2) + pow(VIEW_HEIGHT, 2)));
	if (!(player->isDead()))
	{
		otherX = player->getX();
		otherY = player->getY();
		dist_p = sqrt(pow(otherX - x, 2) + pow(otherY - y, 2));
		numHumans++;
	}
	for (list<Actor*>::const_iterator p = li.begin(); p != li.end(); p++)
	{
		if ((*p)->triggersZombieVomit())
		{
			double tempDist_p = sqrt(pow((*p)->getX() - x, 2) + pow((*p)->getY() - y, 2));
			if (tempDist_p < dist_p)
			{
				dist_p = tempDist_p;
				otherX = (*p)->getX();
				otherY = (*p)->getY();
			}
			numHumans++;
		}
	}
	if (numHumans == 0)
		return false;
	else
	{
		distance = dist_p;
		return true;
	}
}

// Return true if there is a living zombie or Penelope, otherwise false.
// If true, otherX, otherY, and distance will be set to the location and
// distance of the one nearest to (x,y), and isThreat will be set to true
// if it's a zombie, false if a Penelope.
bool StudentWorld::locateNearestCitizenTrigger(double x, double y, double& otherX, double& otherY, double& distance, bool& isThreat) const
{
	int numAgents = 0;
	double dist_p = (sqrt(pow(VIEW_WIDTH, 2) + pow(VIEW_HEIGHT, 2)));

	if (!player->isDead())
	{
		otherX = player->getX();
		otherY = player->getY();
		dist_p = sqrt(pow(otherX - x, 2) + pow(otherY - y, 2));
		isThreat = false;
		numAgents++;
	}
	for (list<Actor*>::const_iterator p = li.begin(); p != li.end(); p++)
	{
		//if zombie is in world
		if ((*p)->threatensCitizens())
		{
			double tempDist_p = sqrt(pow((*p)->getX() - x, 2) + pow((*p)->getY() - y, 2));
			if (tempDist_p < dist_p)
			{
				dist_p = tempDist_p;
				otherX = (*p)->getX();
				otherY = (*p)->getY();
				isThreat = true;
			}
			numAgents++;
		}
	}
	if (numAgents == 0)
		return false;
	else
	{
		distance = dist_p;
		return true;
	}
}

// Return true if there is a living zombie, false otherwise.  If true,
// otherX, otherY and distance will be set to the location and distance
// of the one nearest to (x,y).
bool StudentWorld::locateNearestCitizenThreat(double x, double y, double& otherX, double& otherY, double& distance) const
{
	int numZombies = 0;
	double dist_p = (sqrt(pow(VIEW_WIDTH, 2) + pow(VIEW_HEIGHT, 2)));

	for (list<Actor*>::const_iterator p = li.begin(); p != li.end(); p++)
	{
		if ((*p)->threatensCitizens() && !(*p)->isDead())
		{
			double tempDist_p = sqrt(pow((*p)->getX() - x, 2) + pow((*p)->getY() - y, 2));
			if (tempDist_p < dist_p)
			{
				dist_p = tempDist_p;
				otherX = (*p)->getX();
				otherY = (*p)->getY();
				numZombies++;
			}
		}

	}
	if (numZombies == 0)
		return false;
	else
	{
		distance = dist_p;
		return true;
	}

}