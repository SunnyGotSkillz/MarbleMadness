// Actor.h

#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameConstants.h"
#include <random>
#include <iostream>
using namespace std;

class StudentWorld;

class Actor : public GraphObject {
public:
    Actor(int imageID, double startX, double startY, int dir, StudentWorld* actorWorld) : GraphObject(imageID, startX, startY, dir) {
        world = actorWorld;
        setVisible(true);
        alive = true;
        stolen = false;
        hitPoints = 0;
    }
    virtual void doSomething() = 0;
    void setHitPoints(int pts) { hitPoints = pts; }
    int getHitPoints() const { return hitPoints; }
    StudentWorld* getWorld() const { return world; }
    bool isAlive() const { return alive; }
    void setDead() { alive = false; }
    virtual void damage(int damageAmt) { hitPoints -= damageAmt; }
    
    virtual bool stopsPea() const { return false; }                 // ACTOR STOPS PEA (WALLS, FACTORIES)
    virtual bool isDamageable() const { return false; }             // ACTOR CAN TAKE DAMAGE (PLAYER, MARBLE, ROBOTS)
    virtual bool allowsAgentColocation() const { return false; }    // AGENT CAN BE ON THE SAME SQUARE AS ACTOR (CRYSTALS, GOODIES, EXIT)
    virtual bool allowsMarbleColocation() const { return false; }   // MARBLE BE ON SAME SQUARE AS ACTOR (FOR PITS)
    virtual bool canBePushed() const { return false; }              // ACTOR CAN BE PUSHED (MARBLE)
    virtual bool isSwallowable() const { return false; }            // ACTOR CAN BE SWALLOWED (MARBLE)
    virtual bool isStealable() const { return false; }              // ACTOR CAN BE STOLEN (GOODIES)
    virtual bool countsInFactoryCensus() const { return false; }    // ACTOR COUNTS IN FACTORY CENSUS (THIEF BOTS)
    virtual void setStolen(bool stole) { stolen = stole; }          // SET ACTOR TO STOLEN (FOR GOODIES)
    virtual bool isStolen() const { return stolen; }                      // ACTOR IS STOLEN (FOR GOODIES)
private:
    int hitPoints;
    StudentWorld* world;
    bool alive;
    bool stolen;
};

class Agent : public Actor {
public:
    Agent(int imageID, double startX, double startY, int dir, StudentWorld* actorWorld) : Actor(imageID, startX, startY, dir, actorWorld) {}
    
    virtual bool stopsPea() const { return false; }
    virtual bool isDamageable() const { return true; }
    virtual bool canPushMarbles() const { return false; }       // AGENT CAN PUSH MARBLES (ONLY PLAYER)
};

class Avatar : public Agent {
public:
    Avatar(double startX, double startY, StudentWorld* actorWorld) : Agent(IID_PLAYER, startX, startY, right, actorWorld) {
        setHitPoints(20);
        m_peas = 20;
    }
    
    virtual void doSomething();
    void setPeas(int peas) { m_peas = peas; }       // PEAS = AMMO
    int getPeas() const { return m_peas; }
    double getHealthPct() const { return ((double) getHitPoints() / 20) * 100; }
    virtual void damage(int damageAmt);             // HANDLE PLAYER TAKING DAMAGE
    virtual bool canPushMarbles() const { return true; }
private:
    int m_peas; // AMMO
};

class Wall : public Actor {
public:
    Wall(double startX, double startY, StudentWorld* actorWorld) : Actor(IID_WALL, startX, startY, none, actorWorld)  {
        setHitPoints(0);
    }
    virtual void doSomething() { return; }              // walls don't do anything
    virtual bool stopsPea() const { return true; }
};

class Marble : public Actor {
public:
    Marble(double startX, double startY, StudentWorld* actorWorld) : Actor(IID_MARBLE, startX, startY, none, actorWorld) {
        setHitPoints(10);
    }
    virtual void doSomething() { return; }              // marbles don't do anything
    virtual void damage(int damageAmt);
    virtual bool isDamageable() const { return true; }
    virtual bool canBePushed() const { return true; }
    virtual bool isSwallowable() const { return true; }
};

class Pea : public Actor {
public:
    Pea(double startX, double startY, int dir, StudentWorld* actorWorld) : Actor(IID_PEA, startX, startY, dir, actorWorld) {}
    virtual void doSomething();
    virtual bool allowsAgentColocation() const { return true; }
};

class Pit : public Actor {
public:
    Pit(double startX, double startY, StudentWorld* actorWorld) : Actor(IID_PIT, startX, startY, none, actorWorld) {}
    virtual void doSomething();
    virtual bool allowsMarbleColocation() const { return true; }
};

class Exit : public Actor {
public:
    Exit(int startX, int startY, StudentWorld* world) : Actor(IID_EXIT, startX, startY, none, world) {
        setVisible(false); // invisible by default
    }
    virtual void doSomething();
    virtual bool allowsAgentColocation() const { return true; }
};

class PickupableItem : public Actor {
public:
    PickupableItem(StudentWorld* world, int startX, int startY, int imageID, int score) : Actor(imageID, startX, startY, none, world) { m_score = score; }
    virtual void doSomething();
    virtual bool allowsAgentColocation() const { return true; }
    int getScore() const { return m_score; }
private:
    int m_score; // score when player picks up item
};

class Crystal : public PickupableItem {
public:
    Crystal(int startX, int startY, StudentWorld* world) : PickupableItem(world, startX, startY, IID_CRYSTAL, 50) {}
};

class Goodie : public PickupableItem {
public:
    Goodie(StudentWorld* world, int startX, int startY, int imageID, int score) : PickupableItem(world, startX, startY, imageID, score) {}
    virtual void doSomething();
    virtual bool isStealable() const { return true; }
private:
    virtual void doGoodie() = 0; // execute specific goodie action
};

class ExtraLifeGoodie : public Goodie {
public:
    ExtraLifeGoodie(int startX, int startY, StudentWorld* world) : Goodie(world, startX, startY, IID_EXTRA_LIFE, 1000) {}
private:
    virtual void doGoodie();
};

class RestoreHealthGoodie : public Goodie {
public:
    RestoreHealthGoodie(int startX, int startY, StudentWorld* world) : Goodie(world, startX, startY, IID_RESTORE_HEALTH, 500) {}
private:
    virtual void doGoodie();
};

class AmmoGoodie : public Goodie {
public:
    AmmoGoodie(int startX, int startY, StudentWorld* world) : Goodie(world, startX, startY, IID_AMMO, 100) {}
private:
    virtual void doGoodie();
};

class Robot : public Agent {
public:
    Robot(StudentWorld* world, int startX, int startY, int imageID, int hitPoints, int score, int startDir) : Agent(imageID, startX, startY, startDir, world) {
        m_score = score;
        setHitPoints(hitPoints);
        m_justAttacked = false;
    }
    virtual void doSomething();
    virtual bool isDamageable() const { return true; }
    virtual void damage(int damageAmt);
    virtual bool canPushMarbles() const { return false; }
    virtual bool needsClearShot() const { return true; }    // NEEDS CLEAR SHOT AT PLAYER
    virtual bool isShootingRobot() const { return true; }   // ROBOT CAN SHOOT (difference between ragebots and regular thiefbots)
    bool justAttacked() const { return m_justAttacked; }
    void setJustAttacked(bool var) { m_justAttacked = var; }
private:
    int m_score;
    bool m_justAttacked;        // did robot just attack player
};

class RageBot : public Robot {
public:
    RageBot(int startX, int startY, int startDir, StudentWorld* world) : Robot(world, startX, startY, IID_RAGEBOT, 10, 100, startDir) {}
    virtual void doSomething();
};

class ThiefBot : public Robot {
public:
    ThiefBot(int startX, int startY, int imageID, int hitPoints, int score, StudentWorld* world) : Robot(world, startX, startY, imageID, hitPoints, score, right) {
        distanceBeforeTurning = rand() % 6 + 1;
        currDistance = 0;
        dir = right;
        pickedUpGoodie = false;
        stolenGoodie = nullptr;
    }
    virtual void doSomething();
    virtual bool countsInFactoryCensus() const { return true; }
    virtual void damage(int damageAmt);
    bool hasGoodie() const { return pickedUpGoodie; }
private:
    int distanceBeforeTurning;
    int currDistance;           // current distance traveled
    int dir;
    bool pickedUpGoodie;        // thiefbot has a goodie
    Actor* stolenGoodie;        // stolen goodie
    
    void chooseNewDirection();  // choose new direction to go in
};

class RegularThiefBot : public ThiefBot {
public:
    RegularThiefBot(int startX, int startY, StudentWorld* world) : ThiefBot(startX, startY, IID_THIEFBOT, 5, 10, world) {}
    virtual void doSomething();
    virtual bool isShootingRobot() const { return false; }
};

class MeanThiefBot : public ThiefBot {
public:
    MeanThiefBot(int startX, int startY, StudentWorld* world) : ThiefBot(startX, startY, IID_MEAN_THIEFBOT, 8, 20, world) {}
    virtual void doSomething();
};

class ThiefBotFactory : public Actor {
public:
    enum ProductType { REGULAR, MEAN };

    ThiefBotFactory(int startX, int startY, StudentWorld* world, ProductType type) : Actor(IID_ROBOT_FACTORY, startX, startY, none, world) {
        m_type = type;
    }
    virtual void doSomething();
    virtual bool stopsPea() const { return true; }
private:
    ProductType m_type;
};

#endif // ACTOR_H_
