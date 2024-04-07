#ifndef TAGMANAGER_H
#define TAGMANAGER_H

#include <vector>

class Agent;

class TagManager {
public:
    static void tagEnemy(Agent* agent, std::vector<Agent*>& otherAgents);
};

#endif 