#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
#include "Actor.h"
#include <string>
#include <vector>

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
    Actor* getColocatedStealable(int x, int y) const;
    void restorePlayerHealth() { player->setHitPoints(20); }
    void increaseAmmo() { player->setPeas(player->getPeas() + 20); }
    bool anyCrystals() const { return crystalCount > 0; }
    void decCrystals() { crystalCount--; } 
    void setLevelFinished() { levelFinished = true; }

private:
    std::vector<Actor*> actors;
    Avatar* player;
    int bonusPoints;
    int crystalCount;
    bool levelFinished;
    
    void setDisplayText();
};

#endif // STUDENTWORLD_H_
