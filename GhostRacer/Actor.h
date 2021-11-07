#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameConstants.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class StudentWorld;

class Actor : public GraphObject
{
public:
	Actor(StudentWorld* studentWorld, bool isCollisionAvoidanceWorthy, double verticalSpeed, double horizontalSpeed, int health,
		int imageID, double startX, double startY, int dir, double size, unsigned int depth);

	virtual ~Actor();

	virtual void doSomething() = 0;

	virtual bool beSprayedIfAppropriate() { return false; }

	double verticalSpeed() const{ return m_verticalSpeed; }

	int health() const{ return m_health; }

	bool ifCollisionAvoidanceWorthy() const { return m_isCollisionAvoidanceWorthy; }

	bool isAlive() const{ return m_isAlive; }

protected:
	StudentWorld* studentWorld() const { return m_studentWorld; }

	void changeVerticalSpeed(double speedChange) { m_verticalSpeed += speedChange; }

	double horizontalSpeed() const { return m_horizontalSpeed; }
	void changeHorizontalSpeed(double speedChange) { m_horizontalSpeed += speedChange; }
	void setHorizontalSpeed(double speed) { m_horizontalSpeed = speed; }

	void damage(int healthReduced) { m_health -= healthReduced; }
	void setHealth(int health) { m_health = health; }

	void setAlive(bool isAlive) { m_isAlive = isAlive; }

	bool ifOverlapWithGR();
	bool moveAndPossiblyMoveOut();

private:
	StudentWorld* m_studentWorld;
	bool m_isCollisionAvoidanceWorthy;
	double m_verticalSpeed;
	double m_horizontalSpeed;
	int m_health;
	bool m_isAlive;
};

class GhostRacer :public Actor
{
public:
	GhostRacer(StudentWorld* studentWorld);

	virtual void doSomething();

	bool takeDamageAndPossiblyDie(int healthReduced);

	int nHolyWaterSprays() const { return m_nHolyWaterSprays; }

	void changeNHolyWaterSprays(int amount) { m_nHolyWaterSprays += amount; }

	void spin();

	void heal(int healthIncreased);

private:

	int m_nHolyWaterSprays;
	void moveRacer();
};

class BorderLine : public Actor
{
public:
	BorderLine(StudentWorld* studentWorld, int imageID, double startX, double startY);

	virtual void doSomething();
};

class Pedestrian :public Actor
{
public:
	Pedestrian(StudentWorld* sw, int imageID, double x, double y, double size);
	virtual void doSomething();

private:
	int m_planDistance;
	int planDistance() const { return m_planDistance; }
	void changePlanDistance(int i) { m_planDistance += i; }

	virtual void doSomethingforSpecificPedestrianSubclass() = 0;
	virtual void doSomethingforSpecificPedestrianSubclass2() = 0;
};

class HumanPedestrian : public Pedestrian
{
public:
	HumanPedestrian(StudentWorld* studentWorld, double startX, double startY);
	virtual bool beSprayedIfAppropriate();

private:
	virtual void doSomethingforSpecificPedestrianSubclass();
	virtual void doSomethingforSpecificPedestrianSubclass2();
};

class ZombiePedestrian :public Pedestrian
{
public:
	ZombiePedestrian(StudentWorld* studentWorld, double startX, double startY);
	virtual bool beSprayedIfAppropriate();

private:
	int m_numOfTicks;
	virtual void doSomethingforSpecificPedestrianSubclass();
	virtual void doSomethingforSpecificPedestrianSubclass2();
	int numOfTicks() const { return m_numOfTicks; }
	void changeNumOfTicks(int ticksReduced) { m_numOfTicks += ticksReduced; }
	void setNumOfTicks(int ticks) { m_numOfTicks = ticks; }
	void possibleDied();
};

class ZombieCab :public Actor
{
public:
	ZombieCab(StudentWorld* studentWorld, double verticalSpeed,double startX, double startY);

	virtual void doSomething();

	virtual bool beSprayedIfAppropriate();

private:
	int m_planLength;
	bool m_ifDamageGR;
	int planLength() const { return m_planLength; }
	void changePlanLength(int plReduced) { m_planLength -= plReduced; }
	void setPlanLength(int pl) { m_planLength = pl; }

	bool ifDamageGR() const { return m_ifDamageGR; }
	void setIfDamageGR(bool ifDamageGR) { m_ifDamageGR = ifDamageGR; }

};

class Spray : public Actor
{
public:
	Spray(StudentWorld* sw, double x, double y, int dir);
	virtual void doSomething();
private:
	int m_maxTravelRange;
	int m_travelDistance;
	void changeTravelDistance(int td) { m_travelDistance += td; }
};

class GoodieLikeObject: public Actor
{
public: 
	GoodieLikeObject(StudentWorld* sw, int imageID, double x, double y, double size, int dir);
	virtual void doSomething();

private:
	virtual void doSomethingforSpecificGoodieSubclass() = 0;

};

class OilSlick :public GoodieLikeObject
{
public:
	OilSlick(StudentWorld* sw, double x, double y);

private:
	virtual void doSomethingforSpecificGoodieSubclass();
};

class HealingGoodie :public GoodieLikeObject
{
public:
	HealingGoodie(StudentWorld* sw, double x, double y);
	virtual bool beSprayedIfAppropriate();

private:
	virtual void doSomethingforSpecificGoodieSubclass();
};

class HolyWaterGoodie :public GoodieLikeObject
{
public:
	HolyWaterGoodie(StudentWorld* sw, double x, double y);
	virtual bool beSprayedIfAppropriate();

private:
	virtual void doSomethingforSpecificGoodieSubclass();
};

class SoulGoodie :public GoodieLikeObject
{
public:
	SoulGoodie(StudentWorld* sw, double x, double y);

private:
	virtual void doSomethingforSpecificGoodieSubclass();
};



#endif // ACTOR_H_
