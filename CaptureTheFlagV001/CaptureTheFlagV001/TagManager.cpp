#include "TagManager.h"
#include "Agent.h"
#include "Pathfinder.h"
#include <limits>
#include <algorithm>

void TagManager::tagEnemy(Agent* agent, std::vector<Agent*>& otherAgents) {
    if (agent->isTagged() || agent->getCooldownTimer() > 0) {
        return;
    }

    Agent* nearestEnemy = nullptr;
    float minDistance = std::numeric_limits<float>::max();

    for (Agent* otherAgent : otherAgents) {
        if (otherAgent->getSide() != agent->getSide() && !otherAgent->isTagged() && otherAgent->isOnEnemySide()) {
            float distance = agent->distanceTo(otherAgent);
            if (distance < minDistance && distance <= agent->getTaggingDistance()) {
                nearestEnemy = otherAgent;
                minDistance = distance;
            }
        }
    }

    if (nearestEnemy != nullptr) {
        nearestEnemy->setIsTagged(true);
        agent->setCooldownTimer(agent->getCooldownDuration());
    }
}

void TagManager::resetFlag(Agent* agent) {
    if (agent->isCarryingFlag()) {
        agent->setCarryingFlag(false);
        // Reset the flag to its original position
    }
}

void TagManager::handleTaggedAgent(Agent* agent, Pathfinder* pathfinder, GameManager* gameManager) {
    if (agent->isTagged()) {
        // Use GameManager to get the team zone position
        std::pair<int, int> teamZonePosition = gameManager->getTeamZonePosition(agent->getSide());

        // Check if agent is in its team zone
        if (agent->checkInTeamZone()) {
            agent->setIsTagged(false);
        }
        else {
            // Move towards team zone using pathfinding
            std::vector<std::pair<int, int>> path = pathfinder->findPath(agent->getX(), agent->getY(), teamZonePosition.first, teamZonePosition.second);
            if (!path.empty()) {
                std::pair<int, int> nextPosition = path[0];
                agent->setX(nextPosition.first);
                agent->setY(nextPosition.second);
            }
        }
    }
}

void TagManager::handleFlagCarrier(Agent* agent, Pathfinder* pathfinder, GameManager* gameManager) {
    if (agent->isCarryingFlag()) {
        // Use GameManager to get the team zone position
        std::pair<int, int> teamZonePosition = gameManager->getTeamZonePosition(agent->getSide());

        // Check if agent is in its team zone
        if (agent->checkInTeamZone()) {
            agent->captureFlag();
        }
        else {
            // Move towards team zone using pathfinding
            std::vector<std::pair<int, int>> path = pathfinder->findPath(agent->getX(), agent->getY(), teamZonePosition.first, teamZonePosition.second);
            if (!path.empty()) {
                std::pair<int, int> nextPosition = path[0];
                agent->setX(nextPosition.first);
                agent->setY(nextPosition.second);
            }
        }
    }
}



void TagManager::update(std::vector<Agent*>& agents, Pathfinder* pathfinder, GameManager* gameManager) {
    for (Agent* agent : agents) {
        if (agent->isTagged() && agent->isCarryingFlag()) {
            resetFlag(agent);
        }

        handleTaggedAgent(agent, pathfinder, gameManager);
        handleFlagCarrier(agent, pathfinder, gameManager);

        if (agent->getCooldownTimer() > 0) {
            agent->decrementCooldownTimer();
        }
    }

    for (Agent* agent : agents) {
        if (!agent->isTagged() && agent->getCooldownTimer() == 0) {
            std::vector<Agent*> otherAgents;
            std::copy_if(agents.begin(), agents.end(), std::back_inserter(otherAgents), [agent](Agent* otherAgent) {
                return otherAgent != agent;
                });
            tagEnemy(agent, otherAgents);
        }
    }
}