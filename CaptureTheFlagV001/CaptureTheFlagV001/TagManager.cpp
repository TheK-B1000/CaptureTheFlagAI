#include "TagManager.h"
#include "Agent.h"
#include "Pathfinder.h"
#include <limits>
#include <algorithm>

void TagManager::tagEnemy(Agent* agent, std::vector<Agent*>& otherAgents) {
    if (agent->isTagged() || agent->getCooldownTimer() > 0) {
        return;
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
