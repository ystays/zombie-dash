#include "Actor.h"
#include "StudentWorld.h"



Actor::Actor(StudentWorld* w, int imageID, double x, double y, int dir, int depth)
	:GraphObject(imageID, x, y, dir, depth, 1.0)
{
	pToWorld = w;
	m_isDead = false;
}

//isAt checks whether bounding box of Actor intersects x, y
bool Actor::isAt(double x, double y) const
{
	if ((getX() >= x) && ((getX() - x) < (SPRITE_WIDTH)))
	{
		if ((getY() >= y) && ((getY() - y) < (SPRITE_HEIGHT)))
		{
			return true;
		}
		else if ((y >= getY()) && ((y - getY()) < (SPRITE_HEIGHT)))
		{
			return true;
		}
	}
	else if ((x >= getX()) && ((x - getX()) < (SPRITE_WIDTH)))
	{
		if ((getY() >= y) && ((getY() - y) < (SPRITE_HEIGHT)))
		{
			return true;
		}
		else if ((y >= getY()) && ((y - getY()) < (SPRITE_HEIGHT)))
		{
			return true;
		}
	}
	if ((getY() >= y) && ((getY() - y) < (SPRITE_HEIGHT)))
	{
		if ((getX() >= x) && ((getX() - x) < (SPRITE_WIDTH)))
		{
			return true;
		}
		else if ((x >= getX()) && ((x - getX()) < (SPRITE_WIDTH)))
		{
			return true;
		}
	}
	else if ((y >= getY()) && ((y - getY()) < (SPRITE_HEIGHT)))
	{
		if ((getX() >= x) && ((getX() - x) < (SPRITE_WIDTH)))
		{
			return true;
		}
		else if ((x >= getX()) && ((x - getX()) < (SPRITE_WIDTH)))
		{
			return true;
		}
	}
	return false;
}




Wall::Wall(StudentWorld* w, double x, double y)
	:Actor(w, IID_WALL, x, y, right, 0)  //direction right, depth 0
{}

ActivatingObject::ActivatingObject(StudentWorld* w, int imageID, double x, double y, int depth, int dir)
	: Actor(w, imageID, x, y, depth, dir)
{}


//exit does not block vomit
Exit::Exit(StudentWorld* w, double x, double y)
	: ActivatingObject(w, IID_EXIT, x, y, right, 1)
{}

void Exit::activateIfAppropriate(Actor* a)
{
	a->useExitIfAppropriate();  //calls function useExitIfAppropriate on a

}

void Exit::doSomething()
{
	world()->activateOnAppropriateActors(this);
}



Pit::Pit(StudentWorld* w, double x, double y)
	: ActivatingObject(w, IID_PIT, x, y, right, 0)
{}

void Pit::activateIfAppropriate(Actor* a)
{
	a->dieByFallOrBurnIfAppropriate();
}

void Pit::doSomething()
{
	//cause any person or zombie that overlaps with it to be destroyed
	world()->activateOnAppropriateActors(this);
	//when zombie/person is destroyed, behave just as it were damaged by a flame
}



Flame::Flame(StudentWorld* w, double x, double y, int dir)
	: ActivatingObject(w, IID_FLAME, x, y, dir, 0)
{
	ticksFromCreation = 0;
}

void Flame::activateIfAppropriate(Actor* a)
{
	a->dieByFallOrBurnIfAppropriate();
}

void Flame::doSomething()
{
	if (isDead())
		return;
	if (ticksFromCreation == 2)
	{
		setDead();
		return;
	}
	else
	{
		world()->activateOnAppropriateActors(this);
		ticksFromCreation++;
	}
}



Vomit::Vomit(StudentWorld* w, double x, double y)
	: ActivatingObject(w, IID_VOMIT, x, y, right, 0)
{
	ticksFromCreation = 0;
}

void Vomit::activateIfAppropriate(Actor* a)
{
	a->beVomitedOnIfAppropriate();
}

void Vomit::doSomething()
{
	if (isDead())
		return;
	if (ticksFromCreation == 2)
	{
		setDead();
		return;
	}
	else
	{
		world()->activateOnAppropriateActors(this);
		ticksFromCreation++;
	}
}



Landmine::Landmine(StudentWorld* w, double x, double y)
	: ActivatingObject(w, IID_LANDMINE, x, y, right, 1)
{
	m_safetyTicksLeft = 30;
	m_activeState = false;
	//starts in an 'alive' state
}

void Landmine::activateIfAppropriate(Actor* a)
{
	if (a->triggersOnlyActiveLandmines())
	{
		dieByFallOrBurnIfAppropriate();
	}
}

void Landmine::doSomething()
{
	if (isDead())
		return;
	if (!isActiveState())    //not yet active
	{
		m_safetyTicksLeft--;
		//std::cerr << "LANDMINE" << std::endl;
		if (m_safetyTicksLeft == 0)
		{
			m_activeState = true;
		}
		return;
	}
	else
	{
		world()->activateOnAppropriateActors(this);
	}
}

void Landmine::dieByFallOrBurnIfAppropriate()
{
	setDead();
	world()->playSound(SOUND_LANDMINE_EXPLODE);
	world()->addActor(new Flame(world(), getX(), getY(), up));
	world()->addActor(new Flame(world(), getX() + SPRITE_WIDTH, getY(), up));
	world()->addActor(new Flame(world(), getX() - SPRITE_WIDTH, getY(), up));
	world()->addActor(new Flame(world(), getX(), getY() + SPRITE_HEIGHT, up));
	world()->addActor(new Flame(world(), getX(), getY() - SPRITE_HEIGHT, up));
	world()->addActor(new Flame(world(), getX() + SPRITE_WIDTH, getY() + SPRITE_HEIGHT, up));
	world()->addActor(new Flame(world(), getX() - SPRITE_WIDTH, getY() - SPRITE_HEIGHT, up));
	world()->addActor(new Flame(world(), getX() + SPRITE_WIDTH, getY() - SPRITE_HEIGHT, up));
	world()->addActor(new Flame(world(), getX() - SPRITE_WIDTH, getY() + SPRITE_HEIGHT, up));
	world()->addActor(new Pit(world(), getX(), getY()));

}

//***************************************************************************GOODIES

Goodie::Goodie(StudentWorld* w, int imageID, double x, double y)
	: ActivatingObject(w, imageID, x, y, right, 1)
{}

void Goodie::dieByFallOrBurnIfAppropriate()
{
	setDead();
}

void Goodie::activateIfAppropriate(Actor* a)
{
	if (isDead())
		return;
	a->pickUpGoodieIfAppropriate(this);    //a is pointer to Penelope
}

void Goodie::doSomething()
{
	world()->activateOnAppropriateActors(this);
}


VaccineGoodie::VaccineGoodie(StudentWorld* w, double x, double y)
	:Goodie(w, IID_VACCINE_GOODIE, x, y)
{}

void VaccineGoodie::pickUp(Penelope* p)
{
	p->increaseVaccines();
}


GasCanGoodie::GasCanGoodie(StudentWorld* w, double x, double y)
	: Goodie(w, IID_GAS_CAN_GOODIE, x, y)
{}

void GasCanGoodie::pickUp(Penelope* p)
{
	p->increaseFlameCharges();
}


LandmineGoodie::LandmineGoodie(StudentWorld* w, double x, double y)
	: Goodie(w, IID_LANDMINE_GOODIE, x, y)
{}

void LandmineGoodie::pickUp(Penelope* p)
{
	p->increaseLandmines();
}



Agent::Agent(StudentWorld* w, int imageID, double x, double y, int dir)
	: Actor(w, imageID, x, y, dir, 0)
{
	m_isEvenTick = false;
}



Human::Human(StudentWorld* w, int imageID, double x, double y)
	: Agent(w, imageID, x, y, right)
{
	clearInfection();
	m_infectionDuration = 0;
}


Penelope::Penelope(StudentWorld* w, double x, double y)
	: Human(w, IID_PLAYER, x, y)
{
	m_numFlameCharges = 0;
	m_numLandmines = 0;
	m_numVaccines = 0;
}

void Penelope::beVomitedOnIfAppropriate()
{
	setInfection();
}

void Penelope::doSomething()
{
	if (isDead())
		return;
	if (isInfected())
	{
		increaseInfectionDuration();
		if (getInfectionDuration() == 500)
		{
			setDead();
			world()->playSound(SOUND_PLAYER_DIE);
			return;
		}

	}


	int ch;
	if (world()->getKey(ch))
	{
		switch (ch)
		{
			//3. The doSomething() method must check to see if the player pressed a key.
			//				If the player pressed a key : 
		case KEY_PRESS_SPACE:
		{
			//		a.If the player pressed the space key and Penelope has at least one flamethrower charge, 
			//		then Penelope will attempt to fire three flames into the three slots directly in front of her : 
			if (getNumFlameCharges() >= 1)
			{
				//			i.Penelope’s flamethrower charge count must decrease by 1. 
				m_numFlameCharges--;
				//			ii.Penelope must play the SOUND_PLAYER_FIRE sound effect
				world()->playSound(SOUND_PLAYER_FIRE);
				//			iii.Penelope will add up to three new flame objects to the game.			
				//				If Penelope is at(px, py) this is where the new flame objects will go : 
				double posiX, posiY;
				for (int i = 1; i < 4; i++)
				{
					if (getDirection() == up)
					{
						// • If Penelope is facing up : posi = (px, py + i * SPRITE_HEIGHT) 
						posiX = getX();
						posiY = getY() + i * SPRITE_HEIGHT;
						if (world()->isFlameBlockedAt(posiX, posiY))
							break;
						else
						{
							Flame* f = new Flame(world(), posiX, posiY, getDirection());
							world()->addActor(f);
						}
						
					}
					else if (getDirection() == down)
					{
						// • If she is facing down : posi = (px, py − i *SPRITE_HEIGHT) 
						posiX = getX();
						posiY = getY() - i * SPRITE_HEIGHT;
						if (world()->isFlameBlockedAt(posiX, posiY))
							break;
						else
						{
							Flame* f = new Flame(world(), posiX, posiY, getDirection());
							world()->addActor(f);
						}
					}
					else if (getDirection() == left)
					{
						// • If she is facing left : posi = (px − i *SPRITE_WIDTH, py) 
						posiX = getX() - i * SPRITE_WIDTH;
						posiY = getY();
						if (world()->isFlameBlockedAt(posiX, posiY))
							break;
						else
						{
							Flame* f = new Flame(world(), posiX, posiY, getDirection());
							world()->addActor(f);
						}
					}
					else
					{
						// • If she is facing right : posi = (px + i * SPRITE_WIDTH, py) 
						posiX = getX() + i * SPRITE_WIDTH;
						posiY = getY();
						if (world()->isFlameBlockedAt(posiX, posiY))
							break;
						else
						{
							Flame* f = new Flame(world(), posiX, posiY, getDirection());
							world()->addActor(f);
						}
					}
				}
			}
			break;
		}
		case KEY_PRESS_TAB:
			//		b.If the user pressed the tab key and if Penelope has any landmines in her inventory, 
			if ((getNumLandmines() >= 1))
			{
				//				Penelope will introduce a new landmine object at her current(x, y) location into the game and 
				world()->addActor(new Landmine(world(), getX(), getY()));
				//				her landmine count will decrease by 1.
				m_numLandmines--;
				break;
			}
			break;
		case KEY_PRESS_ENTER:
		{
			//		c.If the user pressed the enter key and if Penelope has any vaccines in her inventory, 
			if (getNumVaccines() >= 1)
			{//				Penelope will set her infected status to false and reduce her vaccine count by 1.  
				clearInfection();
				m_numVaccines--;
			//						(She wasted that vaccine if she was not infected.) 
			}
			break;
		}
		case KEY_PRESS_LEFT:
		{
			setDirection(left);
			double dest_x = getX() - 4;
			double dest_y = getY();
			if (!world()->isAgentMovementBlockedAt(dest_x, dest_y, this))  //movement does not cause overlap
			{
				moveTo(dest_x, dest_y);
			}
			break;
		}
		case KEY_PRESS_RIGHT:
		{
			setDirection(right);
			double dest_x = getX() + 4;
			double dest_y = getY();
			if (!world()->isAgentMovementBlockedAt(dest_x, dest_y, this))
			{
				moveTo(dest_x, dest_y);
			}
			break;
		}
		case KEY_PRESS_UP:
		{
			setDirection(up);
			double dest_x = getX();
			double dest_y = getY() + 4;
			if (!world()->isAgentMovementBlockedAt(dest_x, dest_y, this))
			{
				moveTo(dest_x, dest_y);
			}
			break;
		}
		case KEY_PRESS_DOWN:
		{
			setDirection(down);
			double dest_x = getX();
			double dest_y = getY() - 4;
			if (!world()->isAgentMovementBlockedAt(dest_x, dest_y, this))
			{
				moveTo(dest_x, dest_y);
			}
			break;

		}
		}
	}
	return;

}

//level is finished if all citizens have been saved/killed
void Penelope::useExitIfAppropriate()
{
	world()->recordLevelFinishedIfAllCitizensGone();
}

void Penelope::dieByFallOrBurnIfAppropriate()
{
	if (!isDead())
	{
		setDead();
	}
}

void Penelope::pickUpGoodieIfAppropriate(Goodie* g)
{
	world()->increaseScore(GOODIE_PICKED_UP_POINTS);
	world()->playSound(SOUND_GOT_GOODIE);
	g->pickUp(this);
	g->setDead();
}



Citizen::Citizen(StudentWorld* w, double x, double y)
	: Human(w, IID_CITIZEN, x, y)
{}


void Citizen::doSomething()
{
	if (isDead())
		return;

	if (isInfected())
	{
		increaseInfectionDuration();
		if (getInfectionDuration() == 500)
		{
			setDead();
			world()->playSound(SOUND_ZOMBIE_BORN);
			world()->increaseScore(CITIZEN_DEAD_POINTS);
			//world()->recordCitizenGone();
			int k = randInt(1, 10);
			if (k <= 3)
				world()->addActor(new SmartZombie(world(), getX(), getY()));
			else
				world()->addActor(new DumbZombie(world(), getX(), getY()));
			return;
		}
	}
	if (isEvenTick())
	{
		setIsEvenTick();    //change m_isEvenTick to ~m_isEvenTick
		return;
	}

	double dist_p;
	bool isThreat;
	double otherX, otherY;

	//The citizen must determine its distance to Penelope : dist_p 
	//	5. The citizen must determine its distance to the nearest zombie : dist_z 
	if (world()->locateNearestCitizenTrigger(getX(), getY(), otherX, otherY, dist_p, isThreat))
	{
		//	6. If dist_p < dist_z or no zombies exist in the level(so dist_z is irrelevant), 
		//	and the citizen's Euclidean distance from Penelope is less than or equal to 80 pixels 
		//	(so the citizen wants to follow Penelope): 
		if (!isThreat && dist_p <= 80)    //Penelope
		{
			//		a. If the citizen is on the same row or column as Penelope: 
			if (getX() == otherX || getY() == otherY)
			{

				Direction dirToMove;
				double xToMove, yToMove, xToMove2, yToMove2;
				// i. If the citizen can move 2 pixels in the direction toward Penelope without being blocked 
				//	(by another citizen, Penelope, a zombie, or a wall), the citizen will 
				if (getX() == otherX)
				{
					if (getY() > otherY)
					{//Citizen above Penelope
						dirToMove = down;
						xToMove = getX();
						xToMove2 = getX();
						yToMove = getY() - 1;
						yToMove2 = getY() - 2;
					}
					else
					{//Citizen below Penelope
						dirToMove = up;
						xToMove = getX();
						xToMove2 = getX();
						yToMove = getY() + 1;
						yToMove2 = getY() + 2;
					}
				}
				else    //getY() == otherY
				{
					if (getX() > otherX)
					{//Penelope to left of citizen
						dirToMove = left;
						xToMove = getX() - 1;
						xToMove2 = getX() - 2;
						yToMove = getY();
						yToMove2 = getY();
					}
					else
					{
						dirToMove = right;
						xToMove = getX() + 1;
						xToMove2 = getX() + 2;
						yToMove = getY();
						yToMove2 = getY();
					}
				}
				
				if (!(world()->isAgentMovementBlockedAt(xToMove, yToMove, this)) && !(world()->isAgentMovementBlockedAt(xToMove2, yToMove2, this)))
				{
					// 1. Set its direction to be facing toward Penelope. 

					setDirection(dirToMove);    //face Penelope
					// 2. Move 2 pixels in that direction using the GraphObject class's moveTo() method. 
					moveTo(xToMove, yToMove);    //move 2 pixels in that direction
					moveTo(xToMove2, yToMove2);
					// 3. Immediately return and do nothing more during the current tick.
					return;
				}
				// ii.Otherwise, the citizen would be blocked.
				
			}
			// b.If the citizen is not on the same row or column as Penelope, determine the one horizontal and the one vertical direction 
			// that would get the citizen closer to Penelope.
			else 
			{
				Direction dirToMove;
				double xToMove, yToMove;
				double xToMove2, yToMove2;
				double chooseDirection = randInt(1, 2);

				//			i.Choose one of those two directions at random. If the the citizen can move 2 pixels in that 
				//		direction without being blocked(by another citizen, Penelope, a zombie, or a wall), the citizen will 
				if (chooseDirection == 1)
				{
					if (getX() > otherX)
					{ //citizen to right of Penelope
						dirToMove = left;
						xToMove = getX() - 1;
						xToMove2 = getX() - 2;
						yToMove = getY();
						yToMove2 = getY();

					}
					else if (getX() < otherX)
					{
						dirToMove = right;
						xToMove = getX() + 1;
						xToMove2 = getX() + 2;
						yToMove = getY();
						yToMove2 = getY();
					}

					if (!world()->isAgentMovementBlockedAt(xToMove, yToMove, this) && !world()->isAgentMovementBlockedAt(xToMove2, yToMove2, this))
					{
						setDirection(dirToMove);
						moveTo(xToMove, yToMove);
						moveTo(xToMove2, yToMove2);
						return;
					}
				}
				else    //chooseDirection == 2
				{
					if (getY() > otherY)
					{//citizen above Penelope
						dirToMove = down;
						xToMove = getX();
						xToMove2 = getX();
						yToMove = getY() - 1;
						yToMove2 = getY() - 2;
					}
					else if (getY() < otherY)
					{
						dirToMove = up;
						xToMove = getX();
						xToMove2 = getX();
						yToMove = getY() + 1;
						yToMove2 = getY() + 2;
					}
					if (!world()->isAgentMovementBlockedAt(xToMove, yToMove, this) && !world()->isAgentMovementBlockedAt(xToMove2, yToMove2, this))
					{
						setDirection(dirToMove);
						moveTo(xToMove, yToMove);
						moveTo(xToMove2, yToMove2);
						return;
					}
				}
			}
		}
		//7. If there is a zombie whose Euclidean distance from the citizen is less than or equal to 80 pixels, the citizen wants to run away, so:
		else if (isThreat && dist_p <= 80)
		{
		//a.For each of the four directions up, down, left and right, the citizen must :
			//i.Determine if the citizen is blocked from moving 2 pixels in that direction.
			//	If the citizen is blocked from moving there, it ignores this direction.
			Direction dirToMove;
			bool shouldMove = false;
			double xToMove, yToMove, xToMove2, yToMove2;
			double otherX2, otherY2;
			double temp_dist;
			double dist = (sqrt(pow(getX() - otherX, 2) + pow(getY() - otherY, 2)));
			for (int i = 0; i < 4; i++)
			{

				if (i == 0 && !world()->isAgentMovementBlockedAt(getX(), getY() + 1, this) && !world()->isAgentMovementBlockedAt(getX(), getY() + 2, this))
				{ // direction is up
					world()->locateNearestCitizenThreat(getX(), getY() + 2, otherX2, otherY2, temp_dist);
					if (temp_dist > dist)
					{
						dirToMove = up;
						shouldMove = true;
						xToMove = getX();
						xToMove2 = getX();
						yToMove = getY() + 1;
						yToMove2 = getY() + 2;
					}
				}
				// ii.Otherwise, the citizen determines the distance to the nearest zombie of any type if it were to move to this new location.
				
				if (i == 1 && !world()->isAgentMovementBlockedAt(getX(), getY() - 1, this) && !world()->isAgentMovementBlockedAt(getX(), getY() - 2, this))
				{ // direction is down
					world()->locateNearestCitizenThreat(getX(), getY() - 2, otherX2, otherY2, temp_dist);
					if (temp_dist > dist)
					{
						dirToMove = down;
						shouldMove = true;
						xToMove = getX();
						xToMove2 = getX();
						yToMove = getY() - 1;
						yToMove2 = getY() - 2;
					}
				}
				if (i == 2 && !world()->isAgentMovementBlockedAt(getX() - 1, getY(), this) && !world()->isAgentMovementBlockedAt(getX() - 2, getY(), this))
				{ // direction is left
					world()->locateNearestCitizenThreat(getX() - 2, getY(), otherX2, otherY2, temp_dist);
					if (temp_dist > dist)
					{
						dirToMove = left;
						shouldMove = true;
						xToMove = getX() - 1;
						xToMove2 = getX() - 2;
						yToMove = getY();
						yToMove2 = getY();
					}
				}
				if (i == 3 && !world()->isAgentMovementBlockedAt(getX() + 1, getY(), this) && !world()->isAgentMovementBlockedAt(getX() + 2, getY(), this))
				{ // direction is right
					world()->locateNearestCitizenThreat(getX() + 2, getY(), otherX2, otherY2, temp_dist);
					if (temp_dist > dist)
					{
						dirToMove = right;
						shouldMove = true;
						xToMove = getX() + 1;
						xToMove2 = getX() + 2;
						yToMove = getY();
						yToMove2 = getY();
					}
				}
				//b.If none of the movement options would allow the citizen to get further away from the nearest zombie
				//	(e.g., right now it’s 20 pixels away from the nearest zombie, but if it were to move in any of the four directions it would get even closer 
				//	to some zombie), then doSomething() immediately returns – there is no better place for the citizen to move to than where it is now.
			}
			if (!shouldMove)
				return;
			else
			{
				//c.Otherwise: 
				//	i.Set the citizen’s direction to the direction that will take it furthest away from the nearest zombie.
				setDirection(dirToMove);
				//	ii.Move 2 pixels in that direction using the GraphObject class's moveTo() method. 
				moveTo(xToMove, yToMove);
				moveTo(xToMove2, yToMove2);
				//	iii. Immediately return and do nothing more during the current tick.
				return;
			}
		}
		//8. At this point, there are no zombies whose Euclidean distance from the citizen is less than or equal to 80 pixels.
		//	The citizen does nothing this tick.
		else
		{
			return;
		}
	}
}

void Citizen::useExitIfAppropriate()
{
	//	//inform StudentWorld object that user is to receive 500 points
	world()->increaseScore(CITIZEN_SAVED_POINTS);
	//	//Set the citizen object’s state to dead (does not deduct points from player)
	setDead();
	world()->playSound(SOUND_CITIZEN_SAVED);
}

void Citizen::dieByFallOrBurnIfAppropriate()
{
	setDead();
	//world()->recordCitizenGone();
	world()->increaseScore(CITIZEN_DEAD_POINTS);
	world()->playSound(SOUND_CITIZEN_DIE);
}

void Citizen::beVomitedOnIfAppropriate()
{
	setInfection();
	//if citizen play sound
	world()->playSound(SOUND_CITIZEN_INFECTED);
}



Zombie::Zombie(StudentWorld* w, double x, double y)
	: Agent(w, IID_ZOMBIE, x, y, right)
{
	m_isEvenTick = false;
	setMovementPlanDist(0);

}

void Zombie::vomitIfAppropriate()
{
	//The dumb zombie must check to see if a person(either Penelope or one of the citizens on the level) 
	//	is in front of it in the direction it is facing : 
	double otherX, otherY, dist;
	double vomitCoordX, vomitCoordY;
	if (world()->locateNearestVomitTrigger(getX(), getY(), otherX, otherY, dist))
	{
		if ((getX() == otherX && (getY() < otherY) && getDirection() == up) || ((getX() == otherX) && (getY() > otherY) && getDirection() == down) ||
			(getY() == otherY && (getX() < otherX) && getDirection() == right) || (getY() == otherY && (getX() > otherX) && getDirection() == left))
			//	a.It will compute vomit coordinates in the direction it is facing, 
			//	SPRITE_WIDTH pixels away if it is facing left or right, or SPRITE_HEIGHT pixels away if it is facing up or down.
			//		So if the dumb zombie is at position(x, y) facing left, it would compute the vomit coordinates(x−SPRITE_WIDTH, y), 
			//		i.e., (x−16, y).
		{

			switch (getDirection())
			{
			case up:
			{
				vomitCoordX = getX();
				vomitCoordY = getY() + SPRITE_HEIGHT;
				break;
			}
			case down:
			{
				vomitCoordX = getX();
				vomitCoordY = getY() - SPRITE_HEIGHT;
				break;
			}
			case left:
			{
				vomitCoordX = getX() - SPRITE_WIDTH;
				vomitCoordY = getY();
				break;
			}
			case right:
			{
				vomitCoordX = getX() + SPRITE_WIDTH;
				vomitCoordY = getY();
				break;
			}
			}

			//	b.If there is a person whose Euclidean distance from the vomit coordinates is less than or equal to 10 pixels, 
			//		then there is a 1 in 3 chance that the dumb zombie will vomit.
			if (world()->locateNearestVomitTrigger(vomitCoordX, vomitCoordY, otherX, otherY, dist))
			{
				if (dist <= 10)
				{
					//		If the zombie choses to vomit, it will : 
					if (randInt(1, 3) == 1)
					{
						//			i.Introduce a vomit object into the game at the vomit coordinates.
						world()->addActor(new Vomit(world(), vomitCoordX, vomitCoordY));
						//			ii.Play the sound SOUND_ZOMBIE_VOMIT.
						world()->playSound(SOUND_ZOMBIE_VOMIT);
						//			iii.Immediately return and do nothing more this tick. 
						return;
					}
				}
			}
		}
	}
}

void Zombie::moveInDirectionIfNotBlocked()
{
	//	5. The zombie will then determine a destination coordinate(dest_x, dest_y) that is 1 pixel in front of it in the direction it is facing. 
	double dest_x, dest_y;
	switch (getDirection())
	{
	case up:
	{
		dest_x = getX();
		dest_y = getY() + 1;
		break;
	}
	case down:
	{
		dest_x = getX();
		dest_y = getY() - 1;
		break;
	}
	case left:
	{
		dest_x = getX() - 1;
		dest_y = getY();
		break;
	}
	case right:
	{
		dest_x = getX() + 1;
		dest_y = getY();
		break;
	}
	}
	//	6. If the movement to(dest_x, dest_y) would not cause the zombie’s bounding box to intersect with the bounding box of any wall, 
	//	person or other zombie objects, then : 
	if (!(world()->isAgentMovementBlockedAt(dest_x, dest_y, this)))
	{
		//			a.Update the zombie’s location to(dest_x, dest_y) using the GraphObject class’s moveTo() method.
		moveTo(dest_x, dest_y);
		//			b.Decrease the movement plan distance by 1. 
		decMovementPlanDist();
	}
	else
	{
		//	7. Otherwise, the zombie was blocked from moving by another wall, person or zombie, so set the movement plan distance to 0 
		//	(which will cause the zombie to pick a new direction to move during the next tick)
		setMovementPlanDist(0);
	}
}

DumbZombie::DumbZombie(StudentWorld* w, double x, double y)
	: Zombie(w, x, y)
{}

void DumbZombie::dieByFallOrBurnIfAppropriate()
{
	setDead();
	world()->playSound(SOUND_ZOMBIE_DIE);
	world()->increaseScore(DUMB_ZOMBIE_KILLED_POINTS);
	int carryVaccine = randInt(1, 10);
	if (carryVaccine == 1)
	{
		int randDir = randInt(1, 4);
		double coordX, coordY;
		switch (randDir)
		{
		case 1:
		{
			//up
			coordX = getX();
			coordY = getY() + SPRITE_HEIGHT;
			break;
		}
		case 2:
		{
			//down
			coordX = getX();
			coordY = getY() + SPRITE_WIDTH;
			break;
		}
		case 3:
		{
			//left
			coordX = getX() - SPRITE_WIDTH;
			coordY = getY();
			break;
		}
		case 4:
		{
			//right
			coordX = getX() + SPRITE_WIDTH;
			coordY = getY();
			break;
		}
		}
		if (!world()->isAgentMovementBlockedAt(coordX, coordY, this))
		{
			world()->addActor(new VaccineGoodie(world(), coordX, coordY));
		}
	}
}

void DumbZombie::doSomething()
{
	if (isDead())
		return;
	if (isEvenTick())
	{
		setIsEvenTick();    //change m_isEvenTick to ~m_isEvenTick
		return;
	}
	vomitIfAppropriate();

		//	4. The dumb zombie will check to see if it needs a new movement plan because its current movement plan distance has reached zero.

		//	If so, the dumb zombie will : 
	if (getMovementPlanDist() == 0)
	{
		//			a.Pick a new random movement plan distance in the range 3 through 10 inclusive.
		double movementPlan = randInt(3, 10);
		incMovementPlanDist(movementPlan);
		//			b.Set its direction to a random direction(up, down, left, or right). 
		int randDir = randInt(1, 4);
		switch (randDir)
		{
		case 1:
			setDirection(up);
			break;
		case 2:
			setDirection(down);
			break;
		case 3:
			setDirection(left);
			break;
		case 4:
			setDirection(right);
			break;
		}
	}
	
	moveInDirectionIfNotBlocked();
	

}



SmartZombie::SmartZombie(StudentWorld* w, double x, double y)
	: Zombie(w, x, y)
{}

void SmartZombie::doSomething()
{
	if (isDead())
		return;
	if (isEvenTick())
	{
		setIsEvenTick();    //change m_isEvenTick to ~m_isEvenTick
		return;
	}

	vomitIfAppropriate();

	//	4. The dumb zombie will check to see if it needs a new movement plan because its current movement plan distance has reached zero.

	//	If so, the dumb zombie will : 
	if (getMovementPlanDist() == 0)
	{
		// a.Pick a new random movement plan distance in the range 3 through 10 inclusive.
		double movementPlan = randInt(3, 10);
		incMovementPlanDist(movementPlan);
		// b.Select the person(Penelope or a citizen) closest to the smart zombie, i.e., the one whose Euclidean distance from the zombie is the smallest.
			//If more than one person is the same smallest distance away, select one of them.
		double otherX, otherY, dist;
		world()->locateNearestVomitTrigger(getX(), getY(), otherX, otherY, dist);
		
		if (dist > 80)
		{
			int randDir = randInt(1, 4);
			switch (randDir)
			{
			case 1:
				setDirection(up);
				break;
			case 2:
				setDirection(down);
				break;
			case 3:
				setDirection(left);
				break;
			case 4:
				setDirection(right);
				break;
			}
		}
		else
		{
			if (getX() == otherX)
			{
				if (getY() > otherY)
				{//zombie is above human
					setDirection(down);
				}
				else
				{//zombie is below human
					setDirection(up);
				}
			}
			else if (getY() == otherY)
			{
				if (getX() > otherX)
				{
					setDirection(left);
				}
				else
				{//zombie is to left of human
					setDirection(right);
				}
			}
			else //choose random direction that get zombie closer
			{
				int randDir2 = randInt(1, 2);
				if (randDir2 == 1)
				{
					if (getX() > otherX)
					{
						setDirection(left);
					}
					else
					{
						setDirection(right);
					}
				}
				else    //randDir = 2
				{
					if (getY() > otherY)
					{
						setDirection(down);
					}
					else
					{
						setDirection(up);
					}
				}
			}

		}

	}
	moveInDirectionIfNotBlocked();
}

void SmartZombie::dieByFallOrBurnIfAppropriate()
{
	setDead();
	world()->increaseScore(SMART_ZOMBIE_KILLED_POINTS);
	world()->playSound(SOUND_ZOMBIE_DIE);
}