#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    player = nullptr;
    actors.clear();
}

StudentWorld::~StudentWorld() {
    cleanUp();
}

int StudentWorld::init()
{
    // create level data file string
    string curLevel = "level";
    if (getLevel() < 10) curLevel += "0" + to_string(getLevel()) + ".txt";
    else curLevel += to_string(getLevel()) + ".txt";
    curLevel = "level00.txt"; // TEST
    
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
                crystalCount++;
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
                //actors.push_back(new ThiefBotFactory(x, y, this, ThiefBotFactory::REGULAR));
            } else if (item == Level::mean_thiefbot_factory) {
                // MEAN THIEFBOT FACTORY at (x,y)
                //actors.push_back(new ThiefBotFactory(x, y, this, ThiefBotFactory::MEAN));
            }
        }
    }
    actors.push_back(new RegularThiefBot(2,8,this));
        
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move() {
    int ticks = (28 - getLevel()) / 4;
    if (ticks < 3) ticks = 3;
    currTick++;
    
    // Give player a chance to do something
    if (player->isAlive()) player->doSomething();
    if (!(player->isAlive())) {
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    
    // Give each actor a chance to do something
    std::vector<Actor*>::iterator it;
    it = actors.begin();
    while (it != actors.end()) {
        if (!(*it)->isStolen() && (*it)->isAlive()) { // check if actor is alive because they may have died earlier in the tick
            if ((*it)->isDamageable() && !(*it)->isSwallowable()) {
                if (currTick == ticks) {
                    (*it)->doSomething();
                }
            } else (*it)->doSomething();
            
            if (!(player->isAlive())) { // player died after actor did something
                decLives();
                return GWSTATUS_PLAYER_DIED;
            }
            
            if (levelFinished) {
                increaseScore(bonusPoints);
                return GWSTATUS_FINISHED_LEVEL;
            }
        }
        
        it++;
    }
    
    if (currTick == ticks) currTick = 0;
    
    // Delete any dead actors
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
    
    if (bonusPoints >= 1) bonusPoints--;
    
    setDisplayText();
    
    if (!(player->isAlive())) { // player died after actor did something
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
    
    if (levelFinished) {
        increaseScore(bonusPoints);
        return GWSTATUS_FINISHED_LEVEL;
    }
    
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
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
        if (!((*it)->allowsMarbleColocation()) && ((*it)->getX()) == x && ((*it)->getY()) == y) {
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
        if ((*it)->allowsAgentColocation() && (player->getX()) == x && (player->getY()) == y) {
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
        if ((*it)->allowsAgentColocation() && (*it)->isStealable() && ((*it)->getX()) == x && ((*it)->getY()) == y) {
            return *it;
        }
        it++;
    }

    return nullptr;
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
    actors.push_back(a);
}

bool StudentWorld::damageSomething(Actor* a, int damageAmt) { // only pea
    std::vector<Actor*>::iterator it;
    it = actors.begin();
    bool factory = false;
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
            factory = true;
        }
        
        it++;
    }
    
    if (factory) return true;
    return false;
}

bool StudentWorld::existsClearShotToPlayer(int x, int y, int dx, int dy) const {
    if (dy == 1 || dy == -1) {
        if (player->getX() != x) {
            return false;
        } else {
            if (dy == 1) {
                if (player->getY() < y) return false;
                for (int i = y; i < player->getY(); i++) {
                    std::vector<Actor*>::const_iterator it;
                    it = actors.begin();
                    while (it != actors.end()) {
                        if ((*it)->getY() == i && (*it)->getX() == x && ((*it)->stopsPea() || (*it)->isDamageable())) {
                            return false;
                        }
                                
                        it++;
                    }
                }
            } else if (dy == -1) {
                if (player->getY() > y) return false;
                for (int i = y; i > player->getY(); i--) {
                    std::vector<Actor*>::const_iterator it;
                    it = actors.begin();
                    while (it != actors.end()) {
                        if ((*it)->getY() == i && (*it)->getX() == x && ((*it)->stopsPea() || (*it)->isDamageable())) {
                            return false;
                        }
                                
                        it++;
                    }
                }
            }
        }
    } else if (dx == 1 || dx == -1) {
        if (player->getY() != y) {
            return false;
        } else {
            if (dx == 1) {
                if (player->getX() < x) return false;
                for (int i = x; i < player->getX(); i++) {
                    std::vector<Actor*>::const_iterator it;
                    it = actors.begin();
                    while (it != actors.end()) {
                        if ((*it)->getX() == i && (*it)->getY() == y && ((*it)->stopsPea() || (*it)->isDamageable())) {
                            return false;
                        }
                                
                        it++;
                    }
                }
            } else if (dx == -1) {
                if (player->getX() > x) return false;
                for (int i = x; i > player->getX(); i--) {
                    std::vector<Actor*>::const_iterator it;
                    it = actors.begin();
                    while (it != actors.end()) {
                        if ((*it)->getX() == i && (*it)->getY() == y && ((*it)->stopsPea() || (*it)->isDamageable())) {
                            return false;
                        }
                                
                        it++;
                    }
                }
            }
        }
    }
    
    return true;
}

bool StudentWorld::doFactoryCensus(int x, int y, int distance, int& count) const {
    std::vector<Actor*>::const_iterator it;
    count = 0;
    
    int x1 = max(0, x-distance);
    int x2 = min(VIEW_WIDTH, x+distance);
    int y1 = max(0, y-distance);
    int y2 = min(VIEW_HEIGHT, y+distance);
    for (int a = x1; a <= x2; a++) {
        for (int b = y1; b <= y2; b++) {
            it = actors.begin();
            while (it != actors.end()) {
                if ((*it)->getX() == a && (*it)->getY() == b && (*it)->countsInFactoryCensus()) {
                    if (a == x && b == y) return false;
                    else count++;
                }
                
                it++;
            }
        }
    }
    
    return true;
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
