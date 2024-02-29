#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
#include "Actor.h"
#include <string>
#include <vector>

// left:   dx is -1, dy is  0
// right:  dx is  1, dy is  0
// up:     dx is  0, dy is  1
// down:   dx is  0, dy is -1

class StudentWorld : public GameWorld {
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    bool damageSomething(Actor* a, int damageAmt);
    bool canAgentMoveTo(Agent* agent, int x, int y, int dx, int dy) const;
    bool canMarbleMoveTo(int x, int y) const;
    bool isPlayerColocatedWith(int x, int y) const;
    void addActor(Actor* a);
    bool swallowSwallowable(Actor* a);
    

private:
    std::vector<Actor*> actors;
    Actor* player;
    int bonusPoints;
};

#endif // STUDENTWORLD_H_
