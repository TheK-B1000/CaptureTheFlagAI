#ifndef TAGMANAGER_H
#define TAGMANAGER_H

#include <vector>

class Agent;
class Pathfinder;
class GameManager;

class TagManager {
private:
    GameManager* gameManager;

public:
    TagManager(GameManager* gm) : gameManager(gm) { }

    static void tagEnemy(Agent* agent, std::vector<Agent*>& otherAgents);
    static void handleTaggedAgent(Agent* agent, Pathfinder* pathfinder, GameManager* gameManager);

};

#endif