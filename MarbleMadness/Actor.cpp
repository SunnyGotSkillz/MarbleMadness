#include "Actor.h"
#include "StudentWorld.h"

void Avatar::doSomething() {
    if (getHitPoints() == 0) return; // avatar is dead, do nothing
    
    // user input
    int ch;
    if (getWorld()->getKey(ch)) {
        switch (ch) {
            case KEY_PRESS_LEFT:
                setDirection(180);
                if (!(getWorld()->isWall(getX()-1, getY()))) moveTo(getX()-1, getY());
                break;
            case KEY_PRESS_RIGHT:
                setDirection(0);
                if (!(getWorld()->isWall(getX()+1, getY()))) moveTo(getX()+1, getY());
                break;
            case KEY_PRESS_UP:
                setDirection(90);
                if (!(getWorld()->isWall(getX(), getY()+1))) moveTo(getX(), getY()+1);
                break;
            case KEY_PRESS_DOWN:
                setDirection(270);
                if (!(getWorld()->isWall(getX(), getY()-1))) moveTo(getX(), getY()-1);
                break;
        }
    }
}
