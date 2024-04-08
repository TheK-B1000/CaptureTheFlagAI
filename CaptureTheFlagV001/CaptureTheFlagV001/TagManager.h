#ifndef TAGMANAGER_H
#define TAGMANAGER_H

#include <vector>

class Agent;
class Pathfinder;

class TagManager {
public:
    static void tagEnemy(Agent* agent, std::vector<Agent*>& otherAgents);
    static void resetFlag(Agent* agent);
    static void handleTaggedAgent(Agent* agent, Pathfinder* pathfinder);
    static void handleFlagCarrier(Agent* agent, Pathfinder* pathfinder);
    static void update(std::vector<Agent*>& agents, Pathfinder* pathfinder);
};

#endif