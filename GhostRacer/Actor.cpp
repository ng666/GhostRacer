#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"
// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp


Actor::Actor(StudentWorld* studentWorld, bool isCollisionAvoidanceWorthy, double verticalSpeed, double horizontalSpeed, int health,
	int imageID, double startX, double startY, int dir, double size, unsigned int depth)
	:GraphObject(imageID, startX, startY, dir, size, depth), m_isAlive(true),
	m_studentWorld(studentWorld), m_isCollisionAvoidanceWorthy(isCollisionAvoidanceWorthy),
	m_verticalSpeed(verticalSpeed), m_horizontalSpeed(horizontalSpeed), m_health(health){}

Actor::~Actor() {}

bool Actor::ifOverlapWithGR()
{
	int delta_x = abs(studentWorld()->ghostRacer()->getX() - getX());
	int delta_y = abs(studentWorld()->ghostRacer()->getY() - getY());
	int radius_sum = studentWorld()->ghostRacer()->getRadius() + getRadius();
	if (delta_x < radius_sum * .25 && delta_y < radius_sum * .6)
	{
		return true;
	}
	return false;
}

bool Actor::moveAndPossiblyMoveOut()
{
	double vert_speed = verticalSpeed() - studentWorld()->ghostRacer()->verticalSpeed();
	double horiz_speed = horizontalSpeed();
	double new_y = getY() + vert_speed;
	double new_x = getX() + horiz_speed;
	moveTo(new_x, new_y);

	if ((getX() < 0 || getY() < 0) || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT)
	{
		setAlive(false);
		return true;
	}

	return false;
}

GhostRacer::GhostRacer(StudentWorld* studentWorld)
	: Actor(studentWorld, true, 0.0, 0.0, 100, 
		IID_GHOST_RACER, 128, 32, 90, 4.0, 0),
	m_nHolyWaterSprays(10) {}

void GhostRacer::doSomething()
{
	if (!isAlive())
		return;

	if (getX() <= (ROAD_CENTER - ROAD_WIDTH / 2))
	{
		if (getDirection() > 90)
		{
			takeDamageAndPossiblyDie(10);
		}
		setDirection(82);
		studentWorld()->playSound(SOUND_VEHICLE_CRASH);

		moveRacer();
	}

	if (getX() >= (ROAD_CENTER + ROAD_WIDTH / 2))
	{
		if (getDirection() < 90)
		{
			takeDamageAndPossiblyDie(10);
		}
		setDirection(98);
		studentWorld()->playSound(SOUND_VEHICLE_CRASH);

		moveRacer();
	}

	int ch;
	if (studentWorld()->getKey(ch))
	{
		switch (ch)
		{
		case KEY_PRESS_LEFT:
			if (getDirection() < 114)
			{
				setDirection(getDirection() + 8);
				moveRacer();
			}
			break;
		case KEY_PRESS_RIGHT:
			if (getDirection() > 66)
			{
				setDirection(getDirection() - 8);
				moveRacer();
			}
			break;
		case KEY_PRESS_UP:
			if (verticalSpeed() < 5.0)
			{
				changeVerticalSpeed(1.0);
				moveRacer();
			}
			break;
		case KEY_PRESS_DOWN:
			if (verticalSpeed() > -1.0)
			{
				changeVerticalSpeed(-1.0);
				moveRacer();
			}
			break;
		case KEY_PRESS_SPACE:
			if (nHolyWaterSprays() > 0)
			{
				double x = getX() + SPRITE_HEIGHT * cos(getDirection() * (3.1415926535) / 180);
				double y = getY() + SPRITE_HEIGHT * sin(getDirection() * (3.1415926535) / 180);
				studentWorld()->addActor(new Spray(studentWorld(),x, y, getDirection()));

				studentWorld()->playSound(SOUND_PLAYER_SPRAY);
				changeNHolyWaterSprays(-1);
				moveRacer();
			}
			break;
		default:
			break;
		}
	}

	moveRacer();
}

void GhostRacer::spin()
{
	int temp = randInt(0, 1);
	int new_dir = 0;
	if (temp == 0)
		new_dir = getDirection() + randInt(5, 20);
	else
		new_dir = getDirection() - randInt(5, 20);

	if (new_dir >= 60 && new_dir <= 120)
		setDirection(new_dir);
	else if (new_dir < 60)
		setDirection(60);
	else if (new_dir > 120)
		setDirection(120);

}

void GhostRacer::heal(int healthIncreased)
{
	if (health() + healthIncreased <= 100)
	{
		setHealth(health()+healthIncreased);
	}
	else
		setHealth(100);
}

void GhostRacer::moveRacer()
{
	double max_shift_per_tick = 4.0;
	int direction = getDirection();
	double delta_x = cos(direction*(3.1415926535)/180) * max_shift_per_tick;
	double cur_x = getX();
	double cur_y = getY();
	moveTo(cur_x + delta_x, cur_y);
}

bool  GhostRacer::takeDamageAndPossiblyDie(int healthReduced)
{
	damage(healthReduced);
	if (health() <= 0)
	{
		setAlive(false);
		studentWorld()->playSound(SOUND_PLAYER_DIE);
		return true;
	}
	return false;
}

BorderLine::BorderLine(StudentWorld* studentWorld, int imageID, double startX, double startY)
	:Actor(studentWorld, false, -4.0, 0.0,-99,  
		imageID, startX, startY, 0, 2.0, 1) {}

void BorderLine::doSomething()
{
	if (moveAndPossiblyMoveOut())
		return;
}

Pedestrian::Pedestrian(StudentWorld* sw, int imageID, double x, double y, double size)
	:Actor(sw,true,-4.0,0.0,2,imageID,x,y,0,size,0),m_planDistance(0){}

void Pedestrian::doSomething()
{
	if (!isAlive())
		return;

	if(ifOverlapWithGR())
	{
		doSomethingforSpecificPedestrianSubclass();
		return;
	}

	doSomethingforSpecificPedestrianSubclass2();

	if (moveAndPossiblyMoveOut())
		return;

	if(getSize()==2.0)
		changePlanDistance(-1);

	if (planDistance() > 0)
	{
		if (getSize() == 3.0)
			changePlanDistance(-1);
		return;
	}
	else
	{
		int temp_rand = randInt(0, 1);
		if (temp_rand == 0)
			changeHorizontalSpeed(randInt(-3, -1));
		else
			changeHorizontalSpeed(randInt(1, 3));

		changePlanDistance(randInt(4, 32));

		if (horizontalSpeed() < 0.0)
			setDirection(180);
		else
			setDirection(0);
	}
}

HumanPedestrian::HumanPedestrian(StudentWorld* studentWorld, double startX, double startY)
	:Pedestrian(studentWorld, IID_HUMAN_PED, startX, startY, 2.0) {}

bool HumanPedestrian::beSprayedIfAppropriate()
{
	setHorizontalSpeed(horizontalSpeed() * -1);
	if (getDirection() == 0)
		setDirection(180);
	else
		setDirection(0);
	studentWorld()->playSound(SOUND_PED_HURT);
	return true;

}

void HumanPedestrian::doSomethingforSpecificPedestrianSubclass()
{
	studentWorld()->ghostRacer()->takeDamageAndPossiblyDie(studentWorld()->ghostRacer()->health());
}

void HumanPedestrian::doSomethingforSpecificPedestrianSubclass2()
{
	;
}

ZombiePedestrian::ZombiePedestrian(StudentWorld* studentWorld, double startX, double startY)
	:Pedestrian(studentWorld, IID_ZOMBIE_PED, startX, startY, 3.0),m_numOfTicks(0){}

bool ZombiePedestrian::beSprayedIfAppropriate()
{
	damage(1);
	possibleDied();
	return true;
}

void ZombiePedestrian::doSomethingforSpecificPedestrianSubclass()
{
	studentWorld()->ghostRacer()->takeDamageAndPossiblyDie(5);
	damage(2); 
	possibleDied();
}

void ZombiePedestrian::doSomethingforSpecificPedestrianSubclass2()
{
	if ((abs(getX() - studentWorld()->ghostRacer()->getX()) <= 30)
		&& (getY() >= studentWorld()->ghostRacer()->getY())) 
	{
		setDirection(270);
		if (getX() < studentWorld()->ghostRacer()->getX())
			setHorizontalSpeed(1);
		else if (getX() > studentWorld()->ghostRacer()->getX())
			setHorizontalSpeed(-1);
		else
			setHorizontalSpeed(0);

		changeNumOfTicks(-1);
		if (numOfTicks() <= 0)
		{
			studentWorld()->playSound(SOUND_ZOMBIE_ATTACK);
			setNumOfTicks(20);
		}
	}
}

void ZombiePedestrian::possibleDied()
{
	if (health() <= 0)
	{
		setAlive(false);
		studentWorld()->playSound(SOUND_PED_DIE);
		if (!ifOverlapWithGR())
		{
			//1 in 5 chance adding a new healing goodie at its current position
			int temp = randInt(1, 5);
			if (temp == 5)
			{
				studentWorld()->addActor(new HealingGoodie(studentWorld(), getX(), getY()));
			}
		}
		studentWorld()->increaseScore(150);
	}
	else
		studentWorld()->playSound(SOUND_PED_HURT);
}

ZombieCab::ZombieCab(StudentWorld* studentWorld, double verticalSpeed, double startX, double startY)
	:Actor(studentWorld,true,verticalSpeed,0.0,3, IID_ZOMBIE_CAB, startX, startY,90,4.0,0),
	m_planLength(0),m_ifDamageGR(false){}

void ZombieCab::doSomething()
{
	if (!isAlive())
		return;

	if (ifOverlapWithGR())
	{
		if (ifDamageGR())
		{
			;
		}
		else 
		{
			studentWorld()->playSound(SOUND_VEHICLE_CRASH);
			studentWorld()->ghostRacer()->takeDamageAndPossiblyDie(20);

			if (getX() <= studentWorld()->ghostRacer()->getX())
			{
				setHorizontalSpeed(-5);
				setDirection(120 + randInt(0, 19));
			}
			else if (getX() > studentWorld()->ghostRacer()->getX())
			{
				setHorizontalSpeed(5);
				setDirection(60- randInt(0, 19));
			}
			setIfDamageGR(true);
		}
	}

	if (moveAndPossiblyMoveOut())
		return;

	if (verticalSpeed() > studentWorld()->ghostRacer()->verticalSpeed())
	{
		if (studentWorld()->ifCAWactorsInfront(this))
		{
			changeVerticalSpeed(-0.5);
			return;
		}
	}

	if (verticalSpeed() <= studentWorld()->ghostRacer()->verticalSpeed())
	{
		if (studentWorld()->ifCAWactorsBehind(this))
		{
			changeVerticalSpeed(0.5);
			return;
		}
	}

	changePlanLength(1);
	if (planLength() > 0)
		return;
	else
	{
		setPlanLength(randInt(4, 32));
		changeVerticalSpeed(randInt(-2, 2));
	}
}

bool ZombieCab::beSprayedIfAppropriate()
{
	damage(1);
	if (health() <= 0)
	{
		setAlive(false);
		studentWorld()->playSound(SOUND_VEHICLE_DIE);

		int temp = randInt(1, 5);
		if (temp == 5)
		{
			// 1 in 5 chance adding a oil slick at its current position
			studentWorld()->addActor(new OilSlick(studentWorld(), getX(), getY()));
		}

		studentWorld()->increaseScore(200);
		return true;
	}
	else
		studentWorld()->playSound(SOUND_VEHICLE_HURT);

	return true;
}

Spray::Spray(StudentWorld* sw, double x, double y, int dir)
	:Actor(sw,false,0.0,0.0,0, IID_HOLY_WATER_PROJECTILE,x,y,dir,1.0,1),
	m_maxTravelRange(160),m_travelDistance(0){}

void Spray::doSomething()
{
	if (!isAlive())
		return;

	if (studentWorld()->sprayFirstAppropriateActor(this))
	{
		setAlive(false);
		return;
	}
	else
	{
		moveForward(SPRITE_HEIGHT);
		changeTravelDistance(SPRITE_HEIGHT);
	}

	if ((getX() < 0 || getY() < 0) || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT)
		setAlive(false);

	if(m_travelDistance>=m_maxTravelRange)
		setAlive(false);
}

GoodieLikeObject::GoodieLikeObject(StudentWorld* sw, int imageID, double x, double y, double size,int dir)
	:Actor(sw,false,-4.0,0.0,0,imageID,x,y,dir,size,2){}


void GoodieLikeObject::doSomething()
{
	if (moveAndPossiblyMoveOut())
		return;
	doSomethingforSpecificGoodieSubclass();
}

OilSlick::OilSlick(StudentWorld* sw, double x, double y)
	:GoodieLikeObject(sw,IID_OIL_SLICK,x,y,randInt(2,5),0){}

void OilSlick::doSomethingforSpecificGoodieSubclass()
{
	if (ifOverlapWithGR())
	{
		studentWorld()->playSound(SOUND_OIL_SLICK);
		studentWorld()->ghostRacer()->spin();
	}
}

HealingGoodie::HealingGoodie(StudentWorld* sw, double x, double y)
	:GoodieLikeObject(sw, IID_HEAL_GOODIE,x,y,1.0,0){}

bool HealingGoodie::beSprayedIfAppropriate()
{
	setAlive(false);
	return true;
}

void HealingGoodie::doSomethingforSpecificGoodieSubclass()
{
	if (ifOverlapWithGR())
	{
		studentWorld()->ghostRacer()->heal(10);
		setAlive(false);
		studentWorld()->playSound(SOUND_GOT_GOODIE);
		studentWorld()->increaseScore(250);
	}
}

HolyWaterGoodie::HolyWaterGoodie(StudentWorld* sw, double x, double y)
	:GoodieLikeObject(sw, IID_HOLY_WATER_GOODIE,x,y,2.0,90){}

bool HolyWaterGoodie::beSprayedIfAppropriate()
{
	setAlive(false);
	return true;
}

void HolyWaterGoodie::doSomethingforSpecificGoodieSubclass()
{
	if (ifOverlapWithGR())
	{
		studentWorld()->ghostRacer()->changeNHolyWaterSprays(10);
		setAlive(false);
		studentWorld()->playSound(SOUND_GOT_GOODIE);
		studentWorld()->increaseScore(50);
	}
}

SoulGoodie::SoulGoodie(StudentWorld* sw, double x, double y)
	:GoodieLikeObject(sw, IID_SOUL_GOODIE,x,y,4.0,0){}

void SoulGoodie::doSomethingforSpecificGoodieSubclass()
{
	if (ifOverlapWithGR())
	{
		studentWorld()->recordSoulSaved();
		setAlive(false);
		studentWorld()->playSound(SOUND_GOT_SOUL);
		studentWorld()->increaseScore(100);
	}

	setDirection(getDirection() - 10);

}
