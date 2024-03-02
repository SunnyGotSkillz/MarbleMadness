// Actor.cpp

#include "Actor.h"
#include "StudentWorld.h"
#include <iostream>
using namespace std;

void Avatar::doSomething() {
    if (!isAlive()) return; // player is dead, do nothing
    
    // user input
    int ch;
    if (getWorld()->getKey(ch)) {
        switch (ch) {
            case KEY_PRESS_LEFT:
                setDirection(left);
                if (getWorld()->canAgentMoveTo(this, getX(), getY(), -1, 0)) moveTo(getX()-1, getY());
                break;
            case KEY_PRESS_RIGHT:
                setDirection(right);
                if (getWorld()->canAgentMoveTo(this, getX(), getY(), 1, 0)) moveTo(getX()+1, getY());
                break;
            case KEY_PRESS_UP:
                setDirection(up);
                if (getWorld()->canAgentMoveTo(this, getX(), getY(), 0, 1)) moveTo(getX(), getY()+1);
                break;
            case KEY_PRESS_DOWN:
                setDirection(down);
 
                if (getWorld()->canAgentMoveTo(this, getX(), getY(), 0, -1)) moveTo(getX(), getY()-1);
                break;
            case KEY_PRESS_ESCAPE:
                setDead(); // player gives up
                break;
            case KEY_PRESS_SPACE:
                if (getPeas() != 0) {
                    // fire a pea
                    getWorld()->playSound(SOUND_PLAYER_FIRE);
                    setPeas(getPeas()-1);
                    Actor* pea;
                    if (getDirection() == up) {
                        pea = new Pea(getX(), getY()+1, up, getWorld());
                    } else if (getDirection() == down) {
                        pea = new Pea(getX(), getY()-1, down, getWorld());
                    } else if (getDirection() == left) {
                        pea = new Pea(getX()-1, getY(), left, getWorld());
                    } else { // right
                        pea = new Pea(getX()+1, getY(), right, getWorld());
                    }
                    
                    getWorld()->addActor(pea); // add pea to the world
                }
                break;
        }
    }
}

void Pea::doSomething() {
    if (!isAlive()) return;
    
    if (getWorld()->damageSomething(this, 2)) { // pea hit agent or wall/factory
        setDead();
    } else { // pea should continue
        if (getDirection() == up) {
            moveTo(getX(), getY()+1);
        } else if (getDirection() == down) {
            moveTo(getX(), getY()-1);
        } else if (getDirection() == left) {
            moveTo(getX()-1, getY());
        } else if (getDirection() == right) {
            moveTo(getX()+1, getY());
        }
    }
}

void Avatar::damage(int damageAmt) {
    setHitPoints(getHitPoints()-damageAmt);
    if (getHitPoints() <= 0) { // player is dead
        getWorld()->playSound(SOUND_PLAYER_DIE);
        setDead();
        setVisible(false);
    } else { // player survived
        getWorld()->playSound(SOUND_PLAYER_IMPACT);
    }
}

void Marble::damage(int damageAmt) {
    setHitPoints(getHitPoints()-damageAmt);
    if (getHitPoints() <= 0) { // marble is dead
        setDead();
        setVisible(false);
    }
}

void Pit::doSomething() {
    if (!isAlive()) return;
    
    if (getWorld()->swallowSwallowable(this)) {
        setDead();
    }
}

void PickupableItem::doSomething() {
    if (!isAlive()) return;
    
    if (getWorld()->isPlayerColocatedWith(getX(), getY())) { // player is on same square as item
        getWorld()->increaseScore(m_score); // increase score by given amount
        setDead();
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->decCrystals();
    }
}

void Goodie::doSomething() {
    if (isStolen() || !isAlive()) return; // don't check for players if the goodie has been stolen
    
    Actor* goodie = getWorld()->getColocatedStealable(getX(), getY());
    if (goodie != nullptr) {
        int x = goodie->getX();
        int y = goodie->getY();
        if (getWorld()->isPlayerColocatedWith(x, y) && !isStolen()) {
            doGoodie();     // execute specific goodie action
            setDead();
            getWorld()->playSound(SOUND_GOT_GOODIE);
            getWorld()->increaseScore(getScore());
        }
    }
}

void ExtraLifeGoodie::doGoodie() {
    getWorld()->incLives();
}

void RestoreHealthGoodie::doGoodie() {
    getWorld()->restorePlayerHealth();
}

void AmmoGoodie::doGoodie() {
    getWorld()->increaseAmmo();
}

void Exit::doSomething() {
    if (!getWorld()->anyCrystals()) { // player found all crystals
        setVisible(true);
        getWorld()->playSound(SOUND_REVEAL_EXIT);
    }
    
    if (getWorld()->isPlayerColocatedWith(getX(), getY()) && !getWorld()->anyCrystals()) { // player is on exit
        getWorld()->playSound(SOUND_FINISHED_LEVEL);
        getWorld()->increaseScore(2000);
        getWorld()->setLevelFinished();
    }
}

void Robot::damage(int damageAmt) {
    setHitPoints(getHitPoints()-damageAmt);
    
    if (getHitPoints() <= 0) { // robot died
        getWorld()->playSound(SOUND_ROBOT_DIE);
        setDead();
        setVisible(false);
        getWorld()->increaseScore(m_score);
    } else { // robot survived
        getWorld()->playSound(SOUND_ROBOT_IMPACT);
    }
}

// HANDLES PEA SHOOTING FUNCTIONALITY (RAGEBOTS AND MEAN THIEFBOTS)
void Robot::doSomething() {
    if (getDirection() == up) {
        if (getWorld()->existsClearShotToPlayer(getX(), getY()+1, 0, 1)) { // robot can shoot player
            Actor* pea = new Pea(getX(), getY()+1, up, getWorld());
            getWorld()->addActor(pea);
            getWorld()->playSound(SOUND_ENEMY_FIRE);
            m_justAttacked = true; // robot just attacked player so do nothing else in this tick
            return;
        }
    } else if (getDirection() == down) {
        if (getWorld()->existsClearShotToPlayer(getX(), getY()-1, 0, -1)) {
            Actor* pea = new Pea(getX(), getY()-1, down, getWorld());
            getWorld()->addActor(pea);
            getWorld()->playSound(SOUND_ENEMY_FIRE);
            m_justAttacked = true;
            return;
        }
    } else if (getDirection() == left) {
        if (getWorld()->existsClearShotToPlayer(getX()-1, getY(),-1, 0)) {
            Actor* pea = new Pea(getX()-1, getY(), left, getWorld());
            getWorld()->addActor(pea);
            getWorld()->playSound(SOUND_ENEMY_FIRE);
            m_justAttacked = true;
            return;
        }
    } else if (getDirection() == right) { // right
        if (getWorld()->existsClearShotToPlayer(getX()+1, getY(), 1, 0)) {
            Actor* pea = new Pea(getX()+1, getY(), right, getWorld());
            getWorld()->addActor(pea);
            getWorld()->playSound(SOUND_ENEMY_FIRE);
            m_justAttacked = true;
            return;
        }
    }
}

void RageBot::doSomething() {
    if (!isAlive()) return;
    
    Robot::doSomething(); // shoot pea if possible
    if (justAttacked()) { // if ragebot just attacked, then do nothing else in this tick
        setJustAttacked(false);
        return;
    }
    
    // robot didn't attack, so try to move
    if (getDirection() == left) {
        if (getWorld()->canAgentMoveTo(this, getX(), getY(), -1, 0)) moveTo(getX()-1, getY());
        else setDirection(right);
    } else if (getDirection() == right) {
        if (getWorld()->canAgentMoveTo(this, getX(), getY(), 1, 0)) moveTo(getX()+1, getY());
        else setDirection(left);
    } else if (getDirection() == down) {
        if (getWorld()->canAgentMoveTo(this, getX(), getY(), 0, -1)) moveTo(getX(), getY()-1);
        else setDirection(up);
    } else if (getDirection() == up) {
        if (getWorld()->canAgentMoveTo(this, getX(), getY(), 0, 1)) moveTo(getX(), getY()+1);
        else setDirection(down);
    }
}

void ThiefBot::damage(int damageAmt) {
    Robot::damage(damageAmt); // same damage functionality for thiefbots
    
    if (pickedUpGoodie && !isAlive()) { // drop stolen goodie if dead
        stolenGoodie->moveTo(getX(), getY()); // set goodie location to thiefbot death
        stolenGoodie->setVisible(true);
        stolenGoodie->setStolen(false);
    }
}

void ThiefBot::doSomething() {
    Actor* goodie = getWorld()->getColocatedStealable(getX(), getY());
    if (!pickedUpGoodie && goodie != nullptr) { // has not stolen goodie yet and goodie exists on square with thiefbot
        int chanceStealGoodie = rand() % 10 + 1;
        if (chanceStealGoodie == 1) { // 10% chance of stealing in this tick
            getWorld()->playSound(SOUND_ROBOT_MUNCH);
            pickedUpGoodie = true;
            stolenGoodie = goodie;
            goodie->setStolen(true);
            goodie->setVisible(false);
            return; // do nothing else in this tick
        }
    }
    
    if (currDistance != distanceBeforeTurning) { // thiefbot should keep going in this direction if possible
        if (getDirection() == left) {
            if (getWorld()->canAgentMoveTo(this, getX(), getY(), -1, 0)) {
                currDistance++;
                moveTo(getX()-1, getY());
            }
            else chooseNewDirection();
        } else if (getDirection() == right) {
            if (getWorld()->canAgentMoveTo(this, getX(), getY(), 1, 0)) {
                currDistance++;
                moveTo(getX()+1, getY());
            }
            else chooseNewDirection();
        } else if (getDirection() == down) {
            if (getWorld()->canAgentMoveTo(this, getX(), getY(), 0, -1)) {
                currDistance++;
                moveTo(getX(), getY()-1);
            }
            else chooseNewDirection();
        } else {
            if (getWorld()->canAgentMoveTo(this, getX(), getY(), 0, 1)) {
                currDistance++;
                moveTo(getX(), getY()+1);
            }
            else chooseNewDirection();
        }
    } else { // thiefbot has moved enough squares, so change direction
        chooseNewDirection();
    }
}

void RegularThiefBot::doSomething() {
    if (!isAlive()) return;
    ThiefBot::doSomething(); // same as ThiefBot
}

void MeanThiefBot::doSomething() {
    if (!isAlive()) return;
    
    Robot::doSomething(); // shoot peas just like Ragebots
    if (justAttacked()) {
        setJustAttacked(false);
        return;
    }
    ThiefBot::doSomething(); // also try to steal goodies and move in the same way as regular thiefbots
}

void ThiefBot::chooseNewDirection() {
    currDistance = 0;
    distanceBeforeTurning = rand() % 6 + 1;
    int dirs[] = {up,down,left,right};
    int d = dirs[rand() % 4]; // choose random direction
    
    int i = 0;
    while (i < 4) { // starting with the random direction, try every direction until an unobstructed one is found
        if (d == up) {
            if (getWorld()->canAgentMoveTo(this, getX(), getY(), 0, 1)) {
                setDirection(d);
                moveTo(getX(), getY()+1);
                currDistance++;
                break;
            } else d = (d+90) % 360;
        } else if (d == right) {
            if (getWorld()->canAgentMoveTo(this, getX(), getY(), 1, 0)) {
                setDirection(d);
                moveTo(getX()+1, getY());
                currDistance++;
                break;
            } else d = (d+90) % 360;
        } else if (d == left) {
            if (getWorld()->canAgentMoveTo(this, getX(), getY(), -1, 0)) {
                setDirection(d);
                moveTo(getX()-1, getY());
                currDistance++;
                break;
            } else d = (d+90) % 360;
            
        } else if (d == down) {
            if (getWorld()->canAgentMoveTo(this, getX(), getY(), 0, -1)) {
                setDirection(d);
                moveTo(getX(), getY()-1);
                currDistance++;
                break;
            } else d = (d+90) % 360;
        }
        
        i++;
    }
    
    if (i == 4) setDirection(d % 360); // set to original direction if every direction is obstructed
}

void ThiefBotFactory::doSomething() {
    int num = 0;
    if (getWorld()->doFactoryCensus(getX(), getY(), 3, num)) { // no thiefbot is on the factory
        if (num < 3) { // less than 3 thiefbots in range
            int randNum = rand() % 50 + 1;
            if (randNum == 1) { // 2% chance of spawning new thiefbot
                ThiefBot* thiefBot;
                if (m_type == ThiefBotFactory::REGULAR) {
                    getWorld()->playSound(SOUND_ROBOT_BORN);
                    thiefBot = new RegularThiefBot(getX(), getY(), getWorld());
                    getWorld()->addActor(thiefBot);
                }
                else if (m_type == ThiefBotFactory::MEAN) {
                    getWorld()->playSound(SOUND_ROBOT_BORN);
                    thiefBot = new MeanThiefBot(getX(), getY(), getWorld());
                    getWorld()->addActor(thiefBot);
                }
            }
        }
    }
}
