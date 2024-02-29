#include "Actor.h"
#include "StudentWorld.h"

void Avatar::doSomething() {
    if (!isAlive()) return; // avatar is dead, do nothing
    
    // user input
    int ch;
    if (getWorld()->getKey(ch)) {
        switch (ch) {
            case KEY_PRESS_LEFT:
                setDirection(180);
                if (getWorld()->canAgentMoveTo(this, getX(), getY(), -1, 0)) moveTo(getX()-1, getY());
                break;
            case KEY_PRESS_RIGHT:
                setDirection(0);
                if (getWorld()->canAgentMoveTo(this, getX(), getY(), 1, 0)) moveTo(getX()+1, getY());
                break;
            case KEY_PRESS_UP:
                setDirection(90);
                if (getWorld()->canAgentMoveTo(this, getX(), getY(), 0, 1)) moveTo(getX(), getY()+1);
                break;
            case KEY_PRESS_DOWN:
                setDirection(270);
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
    
    getWorld()->increaseScore(m_score);
    setDead();
    getWorld()->playSound(SOUND_GOT_GOODIE);
}

