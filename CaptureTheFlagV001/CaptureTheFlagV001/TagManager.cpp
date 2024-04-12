#include "TagManager.h"
#include "Agent.h"
#include "Pathfinder.h"
#include <limits>
#include <algorithm>

TagManager::TagManager(GameField* gameField) : gameField(gameField) {}

void TagManager::checkTagging(std::vector<Agent*>& blueAgents, std::vector<Agent*>& redAgents) {
    for (Agent* agent : blueAgents) {
        if (agent->isOnEnemySide() && !agent->isTagged()) {
            for (Agent* enemyAgent : redAgents) {
                if (enemyAgent->checkInHomeZone() && !enemyAgent->isTagged() && agent->distanceTo(enemyAgent) <= agent->getTaggingDistance()) {
                    agent->setIsTagged(true);
                    break;
                }
            }
        }
    }

    for (Agent* agent : redAgents) {
        if (agent->isOnEnemySide() && !agent->isTagged()) {
            for (Agent* enemyAgent : blueAgents) {
                if (enemyAgent->checkInHomeZone() && !enemyAgent->isTagged() && agent->distanceTo(enemyAgent) <= agent->getTaggingDistance()) {
                    agent->setIsTagged(true);
                    break;
                }
            }
        }
    }
}

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
                agent->setX(nextPosition.first);
                agent->setY(nextPosition.second);
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
                agent->setX(nextPosition.first);
                agent->setY(nextPosition.second);
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