// StudentWorld.h

#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
#include "Actor.h"
#include <string>
#include <vector>
using namespace std;

class StudentWorld : public GameWorld {
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld();
    
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    bool damageSomething(Actor* a, int damageAmt);                              // FOR PEAS: DAMAGE ROBOT, MARBLE OR PLAYER
    bool canAgentMoveTo(Agent* agent, int x, int y, int dx, int dy) const;      // CAN ROBOT/PLAYER MOVE TO SQUARE
    bool canMarbleMoveTo(int x, int y) const;                                   // CAN MARBLE MOVE TO SQUARE
    bool isPlayerColocatedWith(int x, int y) const;                             // IS PLAYER ON THIS SQUARE
    void addActor(Actor* a);                                                    // ADD ACTOR TO WORLD
    bool swallowSwallowable(Actor* a);                                          // FOR MARBLES: SWALLOW
    bool existsClearShotToPlayer(int x, int y, int dx, int dy) const;           // ROBOT HAS CLEAR SHOT TO PLAYER
    Actor* getColocatedStealable(int x, int y) const;                           // GET STEALABLE GOODIE ON THIS SQUARE
    bool doFactoryCensus(int x, int y, int distance, int& count) const;         // COUNT THIEFBOTS IN FACTORY RANGE
    void restorePlayerHealth() { player->setHitPoints(20); }                    // RESTORE PLAYER HEALTH GOODIE
    void increaseAmmo() { player->setPeas(player->getPeas() + 20); }            // AMMO GOODIE
    bool anyCrystals() const { return crystalCount > 0; }                       // ARE CRYSTALS LEFT IN THIS LEVEL
    void decCrystals() { crystalCount--; }                                      // PLAYER FOUND 1 CRYSTAL
    void setLevelFinished() { levelFinished = true; }                           // LEVEL IS FINISHED
private:
    std::vector<Actor*> actors;     // all actors
    Avatar* player;                 // player
    int bonusPoints;                // bonus points for current level
    int crystalCount;               // total crystals for this level
    bool levelFinished;             // level has been completed
    int currTick;                   // current tick for robot movement
    
    void setDisplayText();          // top display text
};

#endif // STUDENTWORLD_H_
