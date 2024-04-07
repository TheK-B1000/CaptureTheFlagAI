#include "TagManager.h"
#include "Agent.h"
#include <limits>

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