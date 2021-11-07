#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <vector>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class GhostRacer;
class Actor;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();

    GhostRacer* ghostRacer() const{ return m_ghostRacer; }

    bool ifCAWactorsInfront(Actor* a) const;
    bool ifCAWactorsBehind(Actor* a) const;
    bool sprayFirstAppropriateActor(Actor* spray);
    void addActor(Actor* a);
    void recordSoulSaved();

private:
    GhostRacer* m_ghostRacer;
    std::vector<Actor*> m_actors;
    int m_nSoulSaved;
    int m_bonus;

    void removeDeadActors();

    void addNewBorderLines();
    void addNewHumanPeds();
    void addNewZombiePeds();
    void addNewZombieCabs();
    void addOilSlicks();
    void addHolyWaterGoodies();
    void addSoulGoodies();

    bool overlaps(const Actor* a1, const Actor* a2) const;
};

#endif // STUDENTWORLD_H_

