#include "Actor.h"
#include "StudentWorld.h"
#include <iostream>
using namespace std;

void Avatar::doSomething() {
    if (!isAlive()) return; // avatar is dead, do nothing
    
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
                setDead();
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
                    
                    getWorld()->addActor(pea);
                }
                break;
        }
    }
}

void Pea::doSomething() {
    if (!isAlive()) return;
    
    if (getWorld()->damageSomething(this, 2)) {
        // pea hit agent or wall/factory
        setDead();
    } else {
        // pea can continue
        if (getDirection() == up) {
            moveTo(getX(), getY()+1);
        } else if (getDirection() == down) {
            moveTo(getX(), getY()-1);
        } else if (getDirection() == left) {
            moveTo(getX()-1, getY());
        } else { // right
            moveTo(getX()+1, getY());
        }
    }
}

void Avatar::damage(int damageAmt) {
    setHitPoints(getHitPoints()-damageAmt);
    if (getHitPoints() <= 0) {
        getWorld()->playSound(SOUND_PLAYER_DIE);
        setDead();
        setVisible(false);
    } else {
        getWorld()->playSound(SOUND_PLAYER_IMPACT);
    }
}

void Marble::damage(int damageAmt) {
    setHitPoints(getHitPoints()-damageAmt);
    if (getHitPoints() <= 0) {
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
    
    if (getWorld()->isPlayerColocatedWith(getX(), getY())) {
        getWorld()->increaseScore(m_score);
        setDead();
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->decCrystals();
    }
}

void Goodie::doSomething() {
    if (isStolen() || !isAlive()) return;
    
    Actor* goodie = getWorld()->getColocatedStealable(getX(), getY());
    if (goodie != nullptr) {
        int x = goodie->getX();
        int y = goodie->getY();
        if (getWorld()->isPlayerColocatedWith(x, y) && !isStolen()) {
            doGoodie();            
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
    if (!getWorld()->anyCrystals()) {
        setVisible(true);
        getWorld()->playSound(SOUND_REVEAL_EXIT);
    }
    
    if (getWorld()->isPlayerColocatedWith(getX(), getY()) && !getWorld()->anyCrystals()) {
        getWorld()->playSound(SOUND_FINISHED_LEVEL);
        getWorld()->increaseScore(2000);
        getWorld()->setLevelFinished();
    }
}

void Robot::damage(int damageAmt) {
    setHitPoints(getHitPoints()-damageAmt);
    
    if (getHitPoints() <= 0) {
        getWorld()->playSound(SOUND_ROBOT_DIE);
        
        setDead();
        setVisible(false);
        getWorld()->increaseScore(m_score);
    } else {
        getWorld()->playSound(SOUND_ROBOT_IMPACT);
    }
}

void Robot::doSomething() {
    if (getDirection() == up) {
        if (getWorld()->existsClearShotToPlayer(getX(), getY()+1, 0, 1)) {
            Actor* pea = new Pea(getX(), getY()+1, up, getWorld());
            getWorld()->addActor(pea);
            getWorld()->playSound(SOUND_ENEMY_FIRE);
            m_justAttacked = true;
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
    
    Robot::doSomething();
    if (justAttacked()) {
        setJustAttacked(false);
        return;
    }
    
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
    Robot::damage(damageAmt);
    
    if (pickedUpGoodie && !isAlive()) {
        stolenGoodie->moveTo(getX(), getY());
        stolenGoodie->setVisible(true);
        stolenGoodie->setStolen(false);
    }
}

void ThiefBot::doSomething() {
    Actor* goodie = getWorld()->getColocatedStealable(getX(), getY());
    if (!pickedUpGoodie && goodie != nullptr) {
        int chanceStealGoodie = rand() % 10 + 1;
        if (chanceStealGoodie == 1) {
            getWorld()->playSound(SOUND_ROBOT_MUNCH);
            pickedUpGoodie = true;
            stolenGoodie = goodie;
            goodie->setStolen(true);
            goodie->setVisible(false);
            return;
        }
    }
    
    if (currDistance != distanceBeforeTurning) {
        if (getDirection() == left) {
            if (getWorld()->canAgentMoveTo(this, getX(), getY(), -1, 0)) moveTo(getX()-1, getY());
            else chooseNewDirection();
        } else if (getDirection() == right) {
            if (getWorld()->canAgentMoveTo(this, getX(), getY(), 1, 0)) moveTo(getX()+1, getY());
            else chooseNewDirection();
        } else if (getDirection() == down) {
            if (getWorld()->canAgentMoveTo(this, getX(), getY(), 0, -1)) moveTo(getX(), getY()-1);
            else chooseNewDirection();
        } else {
            if (getWorld()->canAgentMoveTo(this, getX(), getY(), 0, 1)) moveTo(getX(), getY()+1);
            else chooseNewDirection();
        }
    } else {
        chooseNewDirection();
    }
}

void RegularThiefBot::doSomething() {
    if (!isAlive()) return;
    ThiefBot::doSomething();
}

void MeanThiefBot::doSomething() {
    if (!isAlive()) return;
    
    ThiefBot::doSomething();
    if (hasGoodie()) return;
    Robot::doSomething();
}

void ThiefBot::chooseNewDirection() {
    distanceBeforeTurning = rand() % 6 + 1;
    int dirs[] = {up,down,left,right};
    int d = dirs[rand() % 6 + 1];
    
    int i = 0;
    while (i < 4) {
        if (d == up) {
            if (getWorld()->canAgentMoveTo(this, getX(), getY(), 0, 1)) {
                setDirection(d);
                moveTo(getX(), getY()+1);
                break;
            } else d = (d+90) % 360;
        } else if (d == right) {
            if (getWorld()->canAgentMoveTo(this, getX(), getY(), 1, 0)) {
                setDirection(d);
                moveTo(getX()+1, getY());
                break;
            } else d = (d+90) % 360;
        } else if (d == left) {
            if (getWorld()->canAgentMoveTo(this, getX(), getY(), -1, 0)) {
                setDirection(d);
                moveTo(getX()-1, getY());
                break;
            } else d = (d+90) % 360;
            
        } else if (d == down) {
            if (getWorld()->canAgentMoveTo(this, getX(), getY(), 0, -1)) {
                setDirection(d);
                moveTo(getX(), getY()-1);
                break;
            } else d = (d+90) % 360;
        }
        
        i++;
    }
    
    if (i == 4) setDirection(d % 360);
}

void ThiefBotFactory::doSomething() {
    int num = 0;
    if (getWorld()->doFactoryCensus(getX(), getY(), 3, num)) {
        if (num < 3) {
            int randNum = rand() % 50 + 1;
            if (randNum == 1) {
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
