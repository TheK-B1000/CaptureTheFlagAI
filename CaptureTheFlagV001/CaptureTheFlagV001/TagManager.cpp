#include "TagManager.h"
#include "Agent.h"
#include "Pathfinder.h"
#include <limits>
#include <algorithm>

void TagManager::tagEnemy(Agent* agent, std::vector<Agent*>& otherAgents) {
    if (agent->isTagged()) {
        return;
    }

    Agent* nearestEnemy = nullptr;
    float minDistance = std::numeric_limits<float>::max();

    for (Agent* otherAgent : otherAgents) {
        if (otherAgent->getSide() != agent->getSide() && !otherAgent->isTagged()) {
            float distance = agent->distanceTo(otherAgent);
        }
    }

    if (nearestEnemy != nullptr) {
        nearestEnemy->setIsTagged(true);
    }
}

void TagManager::resetFlag(Agent* agent) {
    if (agent->isCarryingFlag()) {
        agent->setCarryingFlag(false);
    }
}

void TagManager::handleTaggedAgent(Agent* agent, Pathfinder* pathfinder) {
    if (agent->isTagged()) {
        if (agent->checkInHomeZone()) {
            agent->setIsTagged(false);
        }
        else {
            // Move towards home zone using pathfinding
            std::pair<int, int> homeZonePosition = agent->getHomeZonePosition();
            std::vector<std::pair<int, int>> path = pathfinder->findPath(agent->getX(), agent->getY(), homeZonePosition.first, homeZonePosition.second);
            if (!path.empty()) {
                std::pair<int, int> nextPosition = path[0];
                agent->moveTo(nextPosition);
            }
        }
    }
}

void TagManager::handleFlagCarrier(Agent* agent, Pathfinder* pathfinder) {
    if (agent->isCarryingFlag()) {
        if (agent->checkInHomeZone()) {
            agent->captureFlag();
        }
        else {
            // Move towards home zone using pathfinding
            std::pair<int, int> homeZonePosition = agent->getHomeZonePosition();
            std::vector<std::pair<int, int>> path = pathfinder->findPath(agent->getX(), agent->getY(), homeZonePosition.first, homeZonePosition.second);
            if (!path.empty()) {
                std::pair<int, int> nextPosition = path[0];
                agent->moveTo(nextPosition);
            }
        }
    }
}

void TagManager::update(std::vector<Agent*>& agents, Pathfinder* pathfinder) {
    for (Agent* agent : agents) {
        if (agent->isTagged() && agent->isCarryingFlag()) {
            resetFlag(agent);
        }
        handleTaggedAgent(agent, pathfinder);
        handleFlagCarrier(agent, pathfinder);
    }
}