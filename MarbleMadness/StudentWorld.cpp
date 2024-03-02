// StudentWorld.cpp

#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
using namespace std;

GameWorld* createStudentWorld(string assetPath) {
	return new StudentWorld(assetPath);
}

StudentWorld::StudentWorld(string assetPath) : GameWorld(assetPath) {
    player = nullptr;
    actors.clear();
}

StudentWorld::~StudentWorld() {
    cleanUp();  // free up space allocated by actors and player
}

int StudentWorld::init() {
    // create level data file string
    string curLevel = "level";
    if (getLevel() < 10) curLevel += "0" + to_string(getLevel()) + ".txt";
    else curLevel += to_string(getLevel()) + ".txt";
    
    Level lev(assetPath());
    Level::LoadResult result = lev.loadLevel(curLevel);
    
    // check for level errors or player victory
    if (result == Level::load_fail_file_not_found || getLevel() == 100) return GWSTATUS_PLAYER_WON;
    if (result == Level:: load_fail_bad_format) return GWSTATUS_LEVEL_ERROR;
    
    // level sucessfully loaded
    bonusPoints = 1000;
    crystalCount = 0;
    levelFinished = false;
    currTick = 0;
    Level::MazeEntry item;
    for (int x = 0; x < VIEW_WIDTH; x++) {
        for (int y = 0; y < VIEW_HEIGHT; y++) {
            item = lev.getContentsOf(x, y);
            if (item == Level::player) {
                // PLAYER at (x,y)
                player = new Avatar(x, y, this);
            } else if (item == Level::wall) {
                // WALL at (x,y)
                actors.push_back(new Wall(x, y, this));
            } else if (item == Level::marble) {
                // MARBLE at (x,y)
                actors.push_back(new Marble(x, y, this));
            } else if (item == Level::pit) {
                // PIT at (x,y)
                actors.push_back(new Pit(x, y, this));
            } else if (item == Level::crystal) {
                // CRYSTAL at (x,y)
                actors.push_back(new Crystal(x, y, this));
                crystalCount++; // increase total crystal count
            } else if (item == Level::exit) {
                // EXIT at (x,y)
                actors.push_back(new Exit(x, y, this));
            } else if (item == Level::extra_life) {
                // EXTRA LIFE GOODIE at (x,y)
                actors.push_back(new ExtraLifeGoodie(x, y, this));
            } else if (item == Level::restore_health) {
                // RESTORE HEALTH GOODIE at (x,y)
                actors.push_back(new RestoreHealthGoodie(x, y, this));
            } else if (item == Level::ammo) {
                // AMMO GOODIE at (x,y)
                actors.push_back(new AmmoGoodie(x, y, this));
            } else if (item == Level::vert_ragebot) {
                // VERT RAGEBOT at (x,y)
                actors.push_back(new RageBot(x, y, Actor::down, this));
            } else if (item == Level::horiz_ragebot) {
                // HORIZ RAGEBOT at (x,y)
                actors.push_back(new RageBot(x, y, Actor::right, this));
            } else if (item == Level::thiefbot_factory) {
                // NORMAL THIEFBOT FACTORY at (x,y)
                actors.push_back(new ThiefBotFactory(x, y, this, ThiefBotFactory::REGULAR));
            } else if (item == Level::mean_thiefbot_factory) {
                // MEAN THIEFBOT FACTORY at (x,y)
                actors.push_back(new ThiefBotFactory(x, y, this, ThiefBotFactory::MEAN));
            }
        }
    }
        
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move() {
    int ticks = (28 - getLevel()) / 4; // calculate which tick robots can act on
    if (ticks < 3) ticks = 3;
    currTick++;
    
    // Give player a chance to do something
    if (player->isAlive()) player->doSomething();
    if (!(player->isAlive())) {
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    
    // Give each actor a chance to do something
    for (int i = 0; i < actors.size(); ++i) {
        if ((actors[i])->isAlive()) { // check if actor is alive because they may have died earlier in the tick
            if ((actors[i])->isDamageable() && !(actors[i])->isSwallowable()) { // actor is robot
                if (currTick == ticks) {
                    (actors[i])->doSomething(); // robots only do something if we hit our target ticks
                }
            } else (actors[i])->doSomething(); // not a robot, do something
            
            if (!(player->isAlive())) { // player died after actor did something
                decLives();
                return GWSTATUS_PLAYER_DIED;
            }
            
            if (levelFinished) { // level has been completed
                increaseScore(bonusPoints);
                playSound(SOUND_FINISHED_LEVEL);
                return GWSTATUS_FINISHED_LEVEL;
            }
        }
    }
    
    if (currTick == ticks) currTick = 0;
    
    // Delete any dead actors
    std::vector<Actor*>::iterator it;
    it = actors.begin();
    it = actors.end();
    it--;
    while (it != actors.begin()) {
        vector<Actor*>::iterator it2;
        it2 = it;
        it2--;
        if (!((*it)->isAlive())) {
            (*it)->setVisible(false);
            delete *it;
            actors.erase(it);
        }
        it = it2;
    }
    if (!((*it)->isAlive())) {
        (*it)->setVisible(false);
        delete *it;
        actors.erase(it);
    }
    
    if (bonusPoints >= 1) bonusPoints--; // remove 1 bonus point per tick
    
    setDisplayText();
    
    if (!(player->isAlive())) { // player died after actor did something
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    
    if (levelFinished) { // level is finished
        increaseScore(bonusPoints);
        playSound(SOUND_FINISHED_LEVEL);
        return GWSTATUS_FINISHED_LEVEL;
    }
    
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp() {
    if (actors.end() == actors.begin()) return; // empty vector
    
    std::vector<Actor*>::iterator it;
    it = actors.end();
    it--;
    while (it != actors.begin()) {
        std::vector<Actor*>::iterator it2;
        it2 = it;
        it2--;
        delete *it;
        actors.erase(it);
        it = it2;
    }
    delete *it;
    actors.erase(it);
    
    delete player;
}

bool StudentWorld::canAgentMoveTo(Agent *agent, int x, int y, int dx, int dy) const {
    std::vector<Actor*>::const_iterator it;
    it = actors.begin();
    while (it != actors.end()) {
        if (!((*it)->allowsAgentColocation()) && ((*it)->getX()) == x+dx && ((*it)->getY()) == y+dy) {
            if (agent->canPushMarbles() && (*it)->canBePushed() && canMarbleMoveTo(((*it)->getX())+dx, ((*it)->getY())+dy)) { // marble
                (*it)->moveTo(((*it)->getX())+dx, ((*it)->getY())+dy);
                return true;
            }
            return false;
        }
        
        it++;
    }

    return true;
}

bool StudentWorld::canMarbleMoveTo(int x, int y) const {
    std::vector<Actor*>::const_iterator it;
    it = actors.begin();
    while (it != actors.end()) {
        if (!((*it)->allowsMarbleColocation()) && ((*it)->getX()) == x && ((*it)->getY()) == y) { // marble cannot move to this spot
            return false;
        }
        it++;
    }

    return true;
}

bool StudentWorld::isPlayerColocatedWith(int x, int y) const {
    std::vector<Actor*>::const_iterator it;
    it = actors.begin();
    while (it != actors.end()) {
        if ((*it)->allowsAgentColocation() && (player->getX()) == x && (player->getY()) == y) { // player is on same spot as actor
            return true;
        }
        it++;
    }

    return false;
}

Actor* StudentWorld::getColocatedStealable(int x, int y) const {
    std::vector<Actor*>::const_iterator it;
    it = actors.begin();
    while (it != actors.end()) {
        if (!(*it)->isStolen() && (*it)->allowsAgentColocation() && (*it)->isStealable() && ((*it)->getX()) == x && ((*it)->getY()) == y) {
            return *it; // return stealable goodie
        }
        it++;
    }

    return nullptr; // no stealable goodie at (x,y)
}

bool StudentWorld::swallowSwallowable(Actor* a) { // pit only
    std::vector<Actor*>::iterator it;
    it = actors.begin();
    while (it != actors.end()) {
        if ((*it)->isSwallowable() && ((*it)->getX()) == a->getX() && ((*it)->getY()) == a->getY()) {
            (*it)->setDead();
            return true;
        }
        it++;
    }

    return false;
}

void StudentWorld::addActor(Actor *a) {
    actors.push_back(a); // add actor to vector
}

bool StudentWorld::damageSomething(Actor* a, int damageAmt) { // pea only
    std::vector<Actor*>::iterator it;
    it = actors.begin();
    bool factoryOrWall = false;
    while (it != actors.end()) {
        if (player->getX() == a->getX() && player->getY() == a->getY()) {
            player->damage(damageAmt);
            return true;
        }
        if (((*it)->getX()) == a->getX() && ((*it)->getY()) == a->getY() && (*it)->isDamageable()) {
            // marble, robot
            (*it)->damage(damageAmt);
            return true;
        } else if (((*it)->getX()) == a->getX() && ((*it)->getY()) == a->getY() && (*it)->stopsPea()) {
            // wall or robot factory
            factoryOrWall = true; // don't return immediately, check if a robot exists on that square
        }
        
        it++;
    }
    
    if (factoryOrWall) return true; // robot doesn't exist, but factory/wall does so return true still
    return false;
}

bool StudentWorld::existsClearShotToPlayer(int x, int y, int dx, int dy) const {
    if (dy == 1 || dy == -1) {
        if (player->getX() != x) { // player and pea are not on the same line
            return false;
        } else {
            if (dy == 1) { // pea facing upward
                if (player->getY() < y) return false;
                for (int i = y; i < player->getY(); i++) { // check every y-coordinate in that x for obstructions
                    std::vector<Actor*>::const_iterator it;
                    it = actors.begin();
                    while (it != actors.end()) {
                        if ((*it)->getY() == i && (*it)->getX() == x && ((*it)->stopsPea() || (*it)->isDamageable())) {
                            return false; // found obstruction -> no clear shot
                        }
                                
                        it++;
                    }
                }
            } else if (dy == -1) { // pea facing downward
                if (player->getY() > y) return false;
                for (int i = y; i > player->getY(); i--) { // check every y-coordinate in that x for obstructions
                    std::vector<Actor*>::const_iterator it;
                    it = actors.begin();
                    while (it != actors.end()) {
                        if ((*it)->getY() == i && (*it)->getX() == x && ((*it)->stopsPea() || (*it)->isDamageable())) {
                            return false; // found obstruction -> no clear shot
                        }
                                
                        it++;
                    }
                }
            }
        }
    } else if (dx == 1 || dx == -1) {
        if (player->getY() != y) { // player and pea are not on the same line
            return false;
        } else {
            if (dx == 1) { // pea facing right
                if (player->getX() < x) return false;
                for (int i = x; i < player->getX(); i++) { // check every x-coordinate in that y for obstructions
                    std::vector<Actor*>::const_iterator it;
                    it = actors.begin();
                    while (it != actors.end()) {
                        if ((*it)->getX() == i && (*it)->getY() == y && ((*it)->stopsPea() || (*it)->isDamageable())) {
                            return false; // found obstruction -> no clear shot
                        }
                                
                        it++;
                    }
                }
            } else if (dx == -1) { // pea facing left
                if (player->getX() > x) return false;
                for (int i = x; i > player->getX(); i--) { // check every x-coordinate in that y for obstructions
                    std::vector<Actor*>::const_iterator it;
                    it = actors.begin();
                    while (it != actors.end()) {
                        if ((*it)->getX() == i && (*it)->getY() == y && ((*it)->stopsPea() || (*it)->isDamageable())) {
                            return false; // found obstruction -> no clear shot
                        }
                                
                        it++;
                    }
                }
            }
        }
    }
    
    return true; // no obstructions found and pea is in-line with player, so shoot
}

bool StudentWorld::doFactoryCensus(int x, int y, int distance, int& count) const {
    count = 0;
    
    // calculate endpoints of factory rectangle without going beyond game borders
    int x1 = max(0, x-distance);
    int x2 = min(VIEW_WIDTH, x+distance);
    int y1 = max(0, y-distance);
    int y2 = min(VIEW_HEIGHT, y+distance);
    
    // iterate through every square in this rectangle
    for (int a = x1; a <= x2; a++) {
        for (int b = y1; b <= y2; b++) {
            std::vector<Actor*>::const_iterator it;
            it = actors.begin();
            while (it != actors.end()) {
                if ((*it)->getX() == a && (*it)->getY() == b && (*it)->countsInFactoryCensus()) {
                    if (a == x && b == y) return false; // robot is on same square as factory, so don't allow more spawns
                    else count++;
                }
                
                it++;
            }
        }
    }
    
    return true; // no robot found on factory square
}

void StudentWorld::setDisplayText() {
    int score = getScore();
    int level = getLevel();
    int livesLeft = getLives();
    int health = player->getHealthPct();
    int ammo = player->getPeas();
    
    ostringstream oss;
    oss << "Score: ";
    oss.fill('0');
    oss << setw(7) << score;
    oss <<  " Level: ";
    oss << setw(2) << level;
    oss.fill(' ');
    oss << " Lives: " << livesLeft << " Health: ";
    oss.setf(ios::fixed);
    oss.precision(0);
    oss << health << "%" << " Ammo: " << ammo << setw(4) << " Bonus: " << bonusPoints;
    string s = oss.str();
    
    setGameStatText(s);
}
