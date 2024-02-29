#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
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
    
    Level lev(assetPath());
    Level::LoadResult result = lev.loadLevel(curLevel);
    
    // check for level errors or player victory
    if (result == Level::load_fail_file_not_found || getLevel() == 100) return GWSTATUS_PLAYER_WON;
    if (result == Level:: load_fail_bad_format) return GWSTATUS_LEVEL_ERROR;
    
    // level sucessfully loaded
    bonusPoints = 1000;
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
            }
        }
    }
        
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you type q
    setGameStatText("Game will end when you type q");
    
    // Give player a chance to do something
    if (player->isAlive()) player->doSomething();
    if (!(player->isAlive())) {
        return GWSTATUS_PLAYER_DIED;
    }
    
    // Give each actor a chance to do something
    std::vector<Actor*>::iterator it;
    it = actors.begin();
    while (it != actors.end()) {
        if ((*it)->isAlive()) { // check if actor is alive because they may have died earlier in the tick
            (*it)->doSomething();
            
            if (!(player->isAlive())) { // player died after actor did something
                return GWSTATUS_PLAYER_DIED;
            }
            
            // TODO: IF PLAYER STEPS ON EXIT AND COLLECTED ALL CRYSTALS, THEN INCREASE PLAYER'S SCORE AND RETURN GWSTATUS_FINISHED_LEVEL
        }
        
        it++;
    }
    
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
    
    // TODO: ACTIVATE THE EXIT ONCE THE PLAYER HAS COLLECTED ALL THE CRYSTALS
    
    // TODO: UPDATE THE STATUS TEXT AT THE TOP OF THE SCREEN
    
    // TODO: CHECK IF PLAYER DIED OR COMPLETED LEVEL AGAIN AND RETURN VALUE
    
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
        if (((*it)->getX()) == x && ((*it)->getY()) == y) {
            return true;
        }
        it++;
    }

    return false;
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
        if (((*it)->getX()) == a->getX() && ((*it)->getY()) == a->getY() && (*it)->isDamageable()) {
            // marble, robot, player
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
