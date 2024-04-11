#include "FlagManager.h"
#include "Agent.h"
#include <iostream>

bool FlagManager::isTeamCarryingFlag(const std::vector<Agent*>& agents) {
    for (const Agent* agent : agents) {
        if (agent->isCarryingFlag()) {
            return true;
        }
    }
    return false;
}

bool FlagManager::grabFlag(Agent* agent, const std::vector<Agent*>& blueAgents, const std::vector<Agent*>& redAgents) {
    if (!agent->isCarryingFlag() && !agent->isTagged() && agent->distanceToEnemyFlag() <= 10) {
        if (!isTeamCarryingFlag(agent->getSide() == "blue" ? blueAgents : redAgents)) {
            agent->setCarryingFlag(true);
            return true;
        }
    }
    return false;
}

bool FlagManager::captureFlag(Agent* agent) {
    if (agent->isCarryingFlag() && agent->checkInHomeZone() && !agent->isTagged()) {
        agent->setCarryingFlag(false);
        if (agent->getSide() == "blue") {
            std::cout << "Blue flag captured!" << std::endl;
        }
        else {
            std::cout << "Red flag captured!" << std::endl;
        }
        resetFlag(agent);
        return true;
    }
    return false;
}

void FlagManager::resetFlag(Agent* agent) {
    agent->setCarryingFlag(false);
    if (agent->getSide() == "blue") {
        std::cout << "Red flag reset!" << std::endl;
    }
    else {
        std::cout << "Blue flag reset!" << std::endl;
    }
}