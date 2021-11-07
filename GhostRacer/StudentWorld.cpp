#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
    : GameWorld(assetPath), m_ghostRacer(nullptr), m_nSoulSaved(0),m_bonus(5000)
{
}

StudentWorld::~StudentWorld() { cleanUp(); }

int StudentWorld::init()
{
    m_ghostRacer = new GhostRacer(this);
    m_nSoulSaved = 0;
    m_bonus = 5000;

    //add yellow border line
    for (int n = 0; n < VIEW_HEIGHT / SPRITE_HEIGHT; n++)
    {
        int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH / 2;
        int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH / 2;
        m_actors.push_back(new BorderLine(this, IID_YELLOW_BORDER_LINE, LEFT_EDGE, n * SPRITE_HEIGHT));
        m_actors.push_back(new BorderLine(this, IID_YELLOW_BORDER_LINE, RIGHT_EDGE, n * SPRITE_HEIGHT));
    }

    //add white border line
    for (int n = 0; n < VIEW_HEIGHT / (4 * SPRITE_HEIGHT); n++)
    {
        int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH / 2;
        int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH / 2;
        m_actors.push_back(new BorderLine(this, IID_WHITE_BORDER_LINE, LEFT_EDGE + ROAD_WIDTH / 3, n * 4 * SPRITE_HEIGHT));
        m_actors.push_back(new BorderLine(this, IID_WHITE_BORDER_LINE, RIGHT_EDGE - ROAD_WIDTH / 3, n * 4 * SPRITE_HEIGHT));
    }

    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    //for ghost racer and actors
    if (m_ghostRacer->isAlive())
    {
        m_ghostRacer->doSomething();

        if (!m_ghostRacer->isAlive())
        {
            decLives();
            return GWSTATUS_PLAYER_DIED;
        }

        if (getLevel() * 2 + 5 == m_nSoulSaved)
        {
            increaseScore(m_bonus);
            return GWSTATUS_FINISHED_LEVEL;
        } 
    }
    for (size_t k = 0; k < m_actors.size(); k++)
    {
        if (m_actors[k]->isAlive())
        {
            m_actors[k]->doSomething();

            if (!m_ghostRacer->isAlive())
            {
                decLives();
                return GWSTATUS_PLAYER_DIED;
            }

            if (getLevel() * 2 + 5 == m_nSoulSaved)
            {
                increaseScore(m_bonus);
                return GWSTATUS_FINISHED_LEVEL;
            }
        }
    }

    removeDeadActors();

    //*****************add other new actors******************
    addNewBorderLines();
    addNewHumanPeds();
    addNewZombiePeds();
    addNewZombieCabs();
    addOilSlicks();
    addHolyWaterGoodies();
    addSoulGoodies();


    //***************update display text***************
    int numOfSoulleft = getLevel()*2+5-m_nSoulSaved;
    m_bonus--;
    string text = "Score: "+to_string(getScore())+"  Lvl: "+ to_string(getLevel()) +"  Souls2Save: " + to_string(numOfSoulleft)
                    +"  Lives: "+ to_string(getLives())+"  Health: "+ to_string(m_ghostRacer->health())
                    +"  Sprays: "+ to_string(m_ghostRacer->nHolyWaterSprays()) +"  Bonus: "+ to_string(m_bonus);
    setGameStatText(text);


    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    for (vector<Actor*>::iterator p = m_actors.begin(); p != m_actors.end();)
    {
            delete (*p);
            p = m_actors.erase(p);
    }

    delete m_ghostRacer;
}

bool StudentWorld::ifCAWactorsInfront(Actor* a) const
{
    for (size_t k = 0; k < m_actors.size(); k++)
    {
        if (m_actors[k] != a && m_actors[k]->ifCollisionAvoidanceWorthy()
            && (m_actors[k]->getX() >= a->getX() - ROAD_WIDTH / 6
                && m_actors[k]->getX() < a->getX() + ROAD_WIDTH / 6)
            && m_actors[k]->getY() > a->getY())
            if(m_actors[k]->getY()< a->getY()+96)
            return true;
    }

   /* if ((m_ghostRacer->getX() >= a->getX() - ROAD_WIDTH / 6
        && m_ghostRacer->getX() < a->getX() + ROAD_WIDTH / 6)
        && m_ghostRacer->getY() > a->getY())
        if (m_ghostRacer->getY() < a->getY() + 96)
            return true;*/

    return false;
}

bool StudentWorld::ifCAWactorsBehind(Actor* a) const
{
    for (size_t k = 0; k < m_actors.size(); k++)
    {
        if (m_actors[k] != a && m_actors[k]->ifCollisionAvoidanceWorthy()
            && (m_actors[k]->getX() >= a->getX() - ROAD_WIDTH / 6
                && m_actors[k]->getX() < a->getX() + ROAD_WIDTH / 6)
            && m_actors[k]->getY() < a->getY())
            if (m_actors[k]->getY() > a->getY() - 96)
                return true;
    }

   /* if ((m_ghostRacer->getX() >= a->getX() - ROAD_WIDTH / 6
        && m_ghostRacer->getX() < a->getX() + ROAD_WIDTH / 6)
        && m_ghostRacer->getY() < a->getY())
        if (m_ghostRacer->getY() > a->getY() - 96)
            return true;*/

    return false;
}

bool StudentWorld::sprayFirstAppropriateActor(Actor* spray)
{
    for (size_t k = 0; k < m_actors.size(); k++)
    {
        if (overlaps(spray, m_actors[k]))
        {
            if (m_actors[k]->beSprayedIfAppropriate())
                return true;
        }
    }
    return false;
}

bool StudentWorld::overlaps(const Actor* a1, const Actor* a2) const
{
    int delta_x = abs(a1->getX() - a2->getX());
    int delta_y = abs(a1->getY() - a2->getY());
    int radius_sum = a1->getRadius() +a2->getRadius();
    if (delta_x < radius_sum * .25 && delta_y < radius_sum * .6)
    {
        return true;
    }
    return false;
}

void StudentWorld::addActor(Actor* a)
{
    m_actors.push_back(a);
}

void StudentWorld::recordSoulSaved()
{
    m_nSoulSaved++;
}


void StudentWorld::removeDeadActors()
{
    for (vector<Actor*>::iterator p = m_actors.begin(); p != m_actors.end();)
    {
        if ((*p)->isAlive())
            p++;
        else
        {
            delete (*p);
            p = m_actors.erase(p);
        }
    }
}

void StudentWorld::addNewBorderLines()
{
    int last_white_border_y = 0;
    for (int k = m_actors.size() - 1; k > 0; k--)
    {
        if (m_actors[k]->health()==-99 && m_actors[k]->getX() == (ROAD_CENTER - ROAD_WIDTH / 6))
        {
            last_white_border_y = m_actors[k]->getY();
            break;
        }
    }

    int  new_border_y = VIEW_HEIGHT - SPRITE_HEIGHT;
    int  delta_y = new_border_y - last_white_border_y;
    if (delta_y >= SPRITE_HEIGHT)
    {
        m_actors.push_back(new BorderLine(this, IID_YELLOW_BORDER_LINE, ROAD_CENTER - ROAD_WIDTH / 2, new_border_y));
        m_actors.push_back(new BorderLine(this, IID_YELLOW_BORDER_LINE, ROAD_CENTER + ROAD_WIDTH / 2, new_border_y));
    }
    if (delta_y >= 4 * SPRITE_HEIGHT)
    {
        m_actors.push_back(new BorderLine(this, IID_WHITE_BORDER_LINE, ROAD_CENTER - ROAD_WIDTH / 6, new_border_y));
        m_actors.push_back(new BorderLine(this, IID_WHITE_BORDER_LINE, ROAD_CENTER + ROAD_WIDTH / 6, new_border_y));
    }
}

void StudentWorld::addNewHumanPeds()
{
    int ChanceHumanPed = 0;
    if (200 - getLevel() * 10 > 30)
        ChanceHumanPed = 200 - getLevel() * 10;
    else
        ChanceHumanPed = 30;
    int temp_rand = randInt(0, ChanceHumanPed - 1);
    if (temp_rand == 0)
        m_actors.push_back(new HumanPedestrian(this, randInt(0, VIEW_WIDTH), VIEW_HEIGHT));
}

void StudentWorld::addNewZombiePeds()
{
    int ChanceZombiePed = 0;
    if (100 - getLevel() * 10 > 20)
        ChanceZombiePed = 100 - getLevel() * 10;
    else
        ChanceZombiePed = 20;
    int temp_rand = randInt(0, ChanceZombiePed - 1);
    if (temp_rand == 0)
        m_actors.push_back(new ZombiePedestrian(this, randInt(0, VIEW_WIDTH), VIEW_HEIGHT));
}

void StudentWorld::addNewZombieCabs()
{
    int ChanceVehicle = 0;
    if (100 - getLevel() * 10 > 20)
        ChanceVehicle = 100 - getLevel() * 10;
    else
        ChanceVehicle = 20;
    int temp_rand = randInt(0, ChanceVehicle - 1);
    if (temp_rand == 0)
    {
        int leftLane = ROAD_CENTER -ROAD_WIDTH / 3;
        bool isLeftLanechecked = false;
        int middleLane = ROAD_CENTER;
        bool isMiddleLanechecked = false;
        int rightLane = ROAD_CENTER + ROAD_WIDTH / 3;
        bool isRightLanechecked = false;
        int cur_lane = randInt(0, 2);
        if (cur_lane == 0)
            cur_lane = leftLane;
        else if (cur_lane == 1)
            cur_lane = middleLane;
        else
            cur_lane = rightLane;

        double startY = 0;
        int startSpeed = 0;

        for (int i = 0; i < 3; i++)
        {
            if (cur_lane == leftLane)
                isLeftLanechecked = true;
            else if (cur_lane == middleLane)
                isMiddleLanechecked = true;
            else if(cur_lane == rightLane)
                isRightLanechecked = true;
            
            Actor* actorWithMinYtoBottom = nullptr;
            double minYtoBottom = VIEW_HEIGHT + 1;
            for (size_t k = 0; k < m_actors.size(); k++) //check actor items other than GR
            {
               
                if (m_actors[k]->ifCollisionAvoidanceWorthy()
                    && (m_actors[k]->getX() >= cur_lane - ROAD_WIDTH / 6
                        && m_actors[k]->getX() < cur_lane + ROAD_WIDTH / 6))
                    if (m_actors[k]->getY() < minYtoBottom)
                    {
                        minYtoBottom = m_actors[k]->getY();
                        actorWithMinYtoBottom = m_actors[k];
                    }
            }
            if(m_ghostRacer->getX() >= cur_lane - ROAD_WIDTH / 6
                && m_ghostRacer->getX() < cur_lane + ROAD_WIDTH / 6)//check ghost racer
                if(m_ghostRacer->getY() < minYtoBottom)
                {
                    minYtoBottom = m_ghostRacer->getY();
                    actorWithMinYtoBottom = m_ghostRacer;
                }

            if (actorWithMinYtoBottom == nullptr || (actorWithMinYtoBottom != nullptr
                && actorWithMinYtoBottom->getY() > (VIEW_HEIGHT / 3)))
            {
                startY = SPRITE_HEIGHT / 2;
                startSpeed = ghostRacer()->verticalSpeed() + randInt(2, 4);
                break;
            }

            Actor* actorWithMinYtoTop = nullptr;
            double minYtoTop = -1;
            for (size_t k = 0; k < m_actors.size(); k++)
            {

                if (m_actors[k]->ifCollisionAvoidanceWorthy()
                    && (m_actors[k]->getX() >= cur_lane - ROAD_WIDTH / 6
                        && m_actors[k]->getX() < cur_lane + ROAD_WIDTH / 6))
                    if (m_actors[k]->getY() > minYtoTop)
                    {
                        minYtoTop = m_actors[k]->getY();
                        actorWithMinYtoTop = m_actors[k];
                    }
            }
            if (m_ghostRacer->getX() >= cur_lane - ROAD_WIDTH / 6
                && m_ghostRacer->getX() < cur_lane + ROAD_WIDTH / 6)//check ghost racer
                if (m_ghostRacer->getY() > minYtoTop)
                {
                    minYtoTop = m_ghostRacer->getY();
                    actorWithMinYtoTop = m_ghostRacer;
                }

            if (actorWithMinYtoTop == nullptr || (actorWithMinYtoTop != nullptr
                && actorWithMinYtoTop->getY() < (VIEW_HEIGHT * 2 / 3)))
            {
                startY = VIEW_HEIGHT - SPRITE_HEIGHT / 2;
                startSpeed = ghostRacer()->verticalSpeed()- randInt(2, 4);
                break;
            }

            if (!isLeftLanechecked)
                cur_lane = leftLane;
            else if (!isRightLanechecked)
                cur_lane = rightLane;
            else if (!isMiddleLanechecked)
                cur_lane = middleLane;
            else
                cur_lane = -1;
        }

        if (cur_lane != -1)
            m_actors.push_back(new ZombieCab(this, startSpeed, cur_lane, startY));
    }
}

void StudentWorld::addOilSlicks()
{
    int ChanceOilSlick = 0;
    if (150 - getLevel() * 10 > 40)
        ChanceOilSlick = 150 - getLevel() * 10;
    else
        ChanceOilSlick = 40;
    int temp_rand = randInt(0, ChanceOilSlick - 1);
    if (temp_rand == 0)
        m_actors.push_back(new OilSlick(this, randInt(ROAD_CENTER-ROAD_WIDTH/2, ROAD_CENTER + ROAD_WIDTH / 2), VIEW_HEIGHT));
}

void StudentWorld::addHolyWaterGoodies()
{
    int ChanceOfHolyWater = 100 + 10 * getLevel();
    int temp_rand = randInt(0, ChanceOfHolyWater - 1);
    if (temp_rand == 0)
        m_actors.push_back(new HolyWaterGoodie(this, randInt(ROAD_CENTER - ROAD_WIDTH / 2, ROAD_CENTER + ROAD_WIDTH / 2), VIEW_HEIGHT));
}

void StudentWorld::addSoulGoodies()
{
    int ChanceOfLostSoul = 100;
    int temp_rand = randInt(0, ChanceOfLostSoul-1);
    if (temp_rand == 0)
        m_actors.push_back(new SoulGoodie(this, randInt(ROAD_CENTER - ROAD_WIDTH / 2, ROAD_CENTER + ROAD_WIDTH / 2), VIEW_HEIGHT));

}
