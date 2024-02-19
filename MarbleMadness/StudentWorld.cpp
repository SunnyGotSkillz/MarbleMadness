#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
}

StudentWorld::~StudentWorld() {
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
}

int StudentWorld::init()
{
    // initialize the maze's vector
    if (!actors.empty()) actors.clear();
    
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
    Level::MazeEntry item;
    for (int x = 0; x < VIEW_WIDTH; x++) {
        for (int y = 0; y < VIEW_HEIGHT; y++) {
            item = lev.getContentsOf(x, y);
            if (item == Level::player) {
                // PLAYER at (x,y)
                actors.push_back(new Avatar(x, y));
            } else if (item == Level::wall) {
                // WALL at (x,y)
                actors.push_back(new Wall(x,y));
            }
        }
    }
        
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you type q
    setGameStatText("Game will end when you type q");
    
    // Give each actor a chance to do something
    std::vector<Actor*>::iterator it;
    it = actors.begin();
    while (it != actors.end()) {
        if ((*it)->getHitPoints() > 0) { // actor is alive
            (*it)->doSomething();
        }
        
        // check if player died or completed level
        
        
        it++;
    }
   
    // return PLAYER DIED OR PLAYER COMPLETED LEVEL OR CONTINUE GAME
    
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
}
