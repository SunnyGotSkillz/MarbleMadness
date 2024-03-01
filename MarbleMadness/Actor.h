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
    Actor(int imageID, double startX, double startY, int dir, StudentWorld* actorWorld)
    : GraphObject(imageID, startX, startY, dir) {
        world = actorWorld;
        setVisible(true);
        alive = true;
        stolen = false;
        hitPoints = 0;
    }
    virtual void doSomething() = 0;
    void setHitPoints(int pts) { hitPoints = pts; }
    int getHitPoints() const { return hitPoints; }
    StudentWorld* getWorld() { return world; }
    bool isAlive() const { return alive; }
    void setDead() { alive = false; }
    
    virtual void damage(int damageAmt) { hitPoints -= damageAmt; }
    virtual bool stopsPea() const { return false; }
    virtual bool isDamageable() const { return false; }
    virtual bool allowsAgentColocation() const { return false; }
    virtual bool allowsMarbleColocation() const { return false; }
    virtual bool canBePushed() const { return false; }
    virtual bool isSwallowable() const { return false; }
    virtual bool isStealable() const { return false; }
    virtual bool countsInFactoryCensus() const { return false; }
    virtual void setStolen(bool stole) { stolen = stole; }
    virtual bool isStolen() { return stolen; }
private:
    int hitPoints;
    StudentWorld* world;
    bool alive;
    bool stolen;
};

class Agent : public Actor {
public:
    Agent(int imageID, double startX, double startY, int dir, StudentWorld* actorWorld)
    : Actor(imageID, startX, startY, dir, actorWorld) { }
    
    virtual bool stopsPea() const { return false; }
    virtual bool isDamageable() const { return true; }
    virtual bool canPushMarbles() const { return false; };
};

class Avatar : public Agent {
public:
    Avatar(double startX, double startY, StudentWorld* actorWorld) : Agent(IID_PLAYER, startX, startY, right, actorWorld) {
        setHitPoints(20);
        m_peas = 20;
    }
    
    virtual void doSomething();
    void setPeas(int peas) { m_peas = peas; }
    int getPeas() const { return m_peas; }
    double getHealthPct() const { return ((double) getHitPoints() / 20) * 100; }
    virtual void damage(int damageAmt);
    virtual bool canPushMarbles() const { return true; }
private:
    int m_peas;
};

class Wall : public Actor {
public:
    Wall(double startX, double startY, StudentWorld* actorWorld) : Actor(IID_WALL, startX, startY, none, actorWorld)  {
        setHitPoints(0);
    }
    virtual void doSomething() { return; }
    virtual bool stopsPea() const { return true; }
private:
};

class Marble : public Actor {
public:
    Marble(double startX, double startY, StudentWorld* actorWorld) : Actor(IID_MARBLE, startX, startY, none, actorWorld) {
        setHitPoints(10);
    }
    virtual void doSomething() { return; }
    virtual void damage(int damageAmt);
    virtual bool isDamageable() const { return true; }
    virtual bool canBePushed() const { return true; }
    virtual bool isSwallowable() const { return true; }
private:
};

class Pea : public Actor {
public:
    Pea(double startX, double startY, int dir, StudentWorld* actorWorld) : Actor(IID_PEA, startX, startY, dir, actorWorld) {
        setHitPoints(10);
    }
    virtual void doSomething();
    virtual bool allowsAgentColocation() const { return true; };
private:
    
};

class Pit : public Actor {
public:
    Pit(double startX, double startY, StudentWorld* actorWorld) : Actor(IID_PIT, startX, startY, none, actorWorld) {}
    virtual void doSomething();
    virtual bool allowsMarbleColocation() const { return true; };
    
private:
};

class Exit : public Actor {
public:
    Exit(int startX, int startY, StudentWorld* world) : Actor(IID_EXIT, startX, startY, none, world) {
        setVisible(false);
    }
    virtual void doSomething();
    virtual bool allowsAgentColocation() const { return true; };
};

class PickupableItem : public Actor {
public:
    PickupableItem(StudentWorld* world, int startX, int startY, int imageID, int score) : Actor(imageID, startX, startY, none, world) { m_score = score; }
    virtual void doSomething();
    virtual bool allowsAgentColocation() const { return true; };
private:
    int m_score;
};

class Crystal : public PickupableItem
{
public:
    Crystal(int startX, int startY, StudentWorld* world) : PickupableItem(world, startX, startY, IID_CRYSTAL, 50) {}
};

class Goodie : public PickupableItem {
public:
    Goodie(StudentWorld* world, int startX, int startY, int imageID, int score) : PickupableItem(world, startX, startY, imageID, score) {
        stolen = false;
    }
    virtual void doSomething();
    virtual bool isStealable() const { return true; }

    // Set whether this goodie is currently stolen.
    void setStolen(bool status) { stolen = status; }
    void setType(int goodie) { type = goodie; }
private:
    bool stolen;
    int type; // 0: Extra life, 1: Restore health, 2: Ammo
};

class ExtraLifeGoodie : public Goodie {
public:
    ExtraLifeGoodie(int startX, int startY, StudentWorld* world) : Goodie(world, startX, startY, IID_EXTRA_LIFE, 1000) {
        setType(0);
    }
};

class RestoreHealthGoodie : public Goodie {
public:
    RestoreHealthGoodie(int startX, int startY, StudentWorld* world) : Goodie(world, startX, startY, IID_RESTORE_HEALTH, 500) {
        setType(1);
    }
};

class AmmoGoodie : public Goodie {
public:
    AmmoGoodie(int startX, int startY, StudentWorld* world) : Goodie(world, startX, startY, IID_AMMO, 100) {
        setType(2);
    }
};

class Robot : public Agent {
public:
    Robot(StudentWorld* world, int startX, int startY, int imageID, int hitPoints, int score, int startDir) : Agent(imageID, startX, startY, startDir, world) {
        m_score = score;
        setHitPoints(hitPoints);
    }
    virtual void doSomething() const { return; }
    virtual bool isDamageable() const { return true; };
    virtual void damage(int damageAmt);
    virtual bool canPushMarbles() const { return false; };
    virtual bool needsClearShot() const { return true; }
    virtual bool isShootingRobot() const { return true; };
private:
    int m_score;
    
    void firePea();
};

class RageBot : public Robot {
public:
    RageBot(int startX, int startY, int startDir, StudentWorld* world) : Robot(world, startX, startY, IID_RAGEBOT, 10, 100, startDir) {}
    virtual void doSomething();
};



#endif // ACTOR_H_
