#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameConstants.h"

class StudentWorld;

class Actor : public GraphObject {
public:
    Actor(int imageID, double startX, double startY, int dir, StudentWorld* actorWorld)
    : GraphObject(imageID, startX, startY, dir) {
        world = actorWorld;
        setVisible(true);
        alive = true;
    }
    virtual void doSomething() = 0;
    void setHitPoints(int pts) { hitPoints = pts; }
    int getHitPoints() const { return hitPoints; }
    StudentWorld* getWorld() { return world; }
    bool isAlive() const { return alive; }
    void setDead() { alive = false; }
    
    virtual void damage(int damageAmt) { hitPoints -= hitPoints; }
    virtual bool stopsPea() const { return false; }
    virtual bool isDamageable() const { return false; }
    virtual bool allowsAgentColocation() const { return false; }
    virtual bool allowsMarbleColocation() const { return false; }
    virtual bool canBePushed() const { return false; }
    virtual bool isSwallowable() const { return false; }
private:
    int hitPoints;
    StudentWorld* world;
    bool alive;
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
    virtual void damage(int damageAmt);
    virtual bool canPushMarbles() const { return true; };
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
    Pit(double startX, double startY, StudentWorld* actorWorld) : Actor(IID_PIT, startX, startY, none, actorWorld) {
    }
    virtual void doSomething();
    virtual bool allowsMarbleColocation() const { return true; };
    
private:
};

class PickupableItem : public Actor {
public:
    PickupableItem(StudentWorld* world, int startX, int startY, int imageID, int score) : Actor(imageID, startX, startY, none, world) { m_score = score; }
    virtual void doSomething();
    virtual bool allowsAgentColocation() const;
private:
    int m_score;
};

class Crystal : public PickupableItem
{
public:
    Crystal(int startX, int startY, StudentWorld* world) : PickupableItem(world, startX, startY, IID_CRYSTAL, 50) {}
};

#endif // ACTOR_H_
