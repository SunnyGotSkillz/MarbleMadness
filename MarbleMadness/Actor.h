#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameConstants.h"

class StudentWorld;

class Actor : public GraphObject {
public:
    Actor(int imageID, double startX, double startY, int dir, double size, StudentWorld* actorWorld)
        : GraphObject(imageID, startX, startY, dir, size)
    { world = actorWorld; }
    virtual void doSomething() = 0;
    void setHitPoints(int pts) { hitPoints = pts; }
    int getHitPoints() { return hitPoints; }
    StudentWorld* getWorld() { return world; }
private:
    int hitPoints;
    StudentWorld* world;
};

class Wall : public Actor {
public:
    Wall(double startX, double startY, StudentWorld* actorWorld) : Actor(IID_WALL, startX, startY, none, 1.0, actorWorld)  {
        setVisible(true);
        setHitPoints(0);
    }
    virtual void doSomething() { return; }
private:
};

class Avatar : public Actor {
public:
    Avatar(double startX, double startY, StudentWorld* actorWorld) : Actor(IID_PLAYER, startX, startY, right, 1.0, actorWorld) {
        setVisible(true);
        setHitPoints(20);
        peas = 20;
    }
    virtual void doSomething();
private:
    int peas;
};

#endif // ACTOR_H_
