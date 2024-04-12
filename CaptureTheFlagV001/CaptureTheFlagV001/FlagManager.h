#ifndef FLAGMANAGER_H
#define FLAGMANAGER_H

#include "GameField.h"
#include <vector>

class Agent;

class FlagManager {
public:
    FlagManager(GameField* gameField);
    bool isTeamCarryingFlag(const std::vector<Agent*>& blueAgents, const std::vector<Agent*>& redAgents);
    bool grabFlag(Agent* agent);
    bool captureFlag(Agent* agent);
    void resetFlag(Agent* agent);

private:
    GameField* gameField;

};

#endif