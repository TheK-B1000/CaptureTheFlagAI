#ifndef TAGMANAGER_H
#define TAGMANAGER_H

#include <vector>
#include "Agent.h"

class Pathfinder;
class GameField;

class TagManager {
public:
    TagManager(GameField* gameField);

    void checkTagging(std::vector<Agent*>& blueAgents, std::vector<Agent*>& redAgents);
    void update(std::vector<Agent*>& agents, Pathfinder* pathfinder);

private:
    GameField* gameField;
    void tagEnemy(Agent* agent, std::vector<Agent*>& otherAgents);
    void resetFlag(Agent* agent);
    void handleTaggedAgent(Agent* agent, Pathfinder* pathfinder);
    void handleFlagCarrier(Agent* agent, Pathfinder* pathfinder);
};

#endif