#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameConstants.h"

class Actor : public GraphObject {
public:
    Actor(int imageID, double startX, double startY, int dir, double size)
        : GraphObject(imageID, startX, startY, dir, size)
        {}
    virtual void doSomething() = 0;
    void setHitPoints(int pts) { hitPoints = pts; }
    int getHitPoints () { return hitPoints; }
private:
    int hitPoints;
};

class Wall : public Actor {
public:
    Wall(double startX, double startY) : Actor(IID_WALL, startX, startY, none, 1.0)  {}
    virtual void doSomething() { return; }
private:
};

class Avatar : public Actor {
public:
    Avatar(double startX, double startY) : Actor(IID_PLAYER, startX, startY, right, 1.0) {
        setHitPoints(20);
        peas = 20;
    }
    virtual void doSomething();
private:
    int peas;
};

#endif // ACTOR_H_
