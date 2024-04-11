#ifndef FLAGMANAGER_H
#define FLAGMANAGER_H

#include <vector>

class Agent;

class FlagManager {
public:
    static bool isTeamCarryingFlag(const std::vector<Agent*>& agents);
    static bool grabFlag(Agent* agent, const std::vector<Agent*>& blueAgents, const std::vector<Agent*>& redAgents);
    static bool captureFlag(Agent* agent);
    static void resetFlag(Agent* agent);
};

#endif