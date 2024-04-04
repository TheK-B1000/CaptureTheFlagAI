
#include "FlagManager.h"
#include <cmath>

FlagManager::FlagManager(GameField& gameField) : gameField(gameField) {
    // Initialize flagStatus and flagPositions based on gameField information
    flagStatus[0] = false; // Blue team
    flagStatus[1] = false; // Red team

    flagPositions[0] = QPointF(70, 290); // Blue flag position
    flagPositions[1] = QPointF(710, 290); // Red flag position
}

void FlagManager::attemptFlagGrab(Agent& agent) {
    if (agent.isTagged()) return; // Cannot grab if tagged

    // Determine if the agent is within the enemy flag zone
    int enemyTeamId = (agent.getTeam() == 0) ? 1 : 0;
    if (checkIfAgentInZone(QPointF(agent.getPosition().x, agent.getPosition().y), flagPositions[enemyTeamId], 40)) {
        if (!flagStatus[enemyTeamId]) {
            flagStatus[enemyTeamId] = true; // Flag is now considered grabbed
            agent.setCarryingFlag(true); // Update agent's state to carrying flag
            // Update flagPositions to follow the agent
            flagPositions[enemyTeamId] = QPointF(agent.getPosition().x, agent.getPosition().y);
        }
    }
}

void FlagManager::update() {
    // Check for agents attempting to return a flag
    for (auto& [teamId, grabbed] : flagStatus) {
        if (grabbed) {
            // Check if any agent from the teamId is successfully returning the flag
            for (Agent* agent : gameField.getTeamAgents(teamId)) {
                if (isFlagSuccessfullyReturned(*agent)) {
                    // Flag has been successfully returned, no need to check other agents
                    break;
                }
            }
        }
    }
}

bool FlagManager::isFlagSuccessfullyReturned(const Agent& agent) {
    int teamId = agent.getTeam();
    if (agent.isCarryingFlag() && !agent.isTagged() &&
        checkIfAgentInZone(QPointF(agent.getPosition().x, agent.getPosition().y), this->flagPositions[teamId], 40)) {
        int enemyTeamId = (teamId == 0) ? 1 : 0;
        resetFlag(enemyTeamId); // Reset the enemy's flag status

        // Create a non-const reference to the Agent object
        Agent& mutableAgent = const_cast<Agent&>(agent);
        mutableAgent.setCarryingFlag(false); // Update agent's state

        return true;
    }
    return false;
}

void FlagManager::resetFlag(int teamId) {
    flagStatus[teamId] = false; // Flag is returned to base
    // Reset the flag position to its original location
    flagPositions[teamId] = (teamId == 0) ? QPointF(70, 290) : QPointF(710, 290);
}

bool FlagManager::checkIfAgentInZone(const QPointF& agentPos, const QPointF& zoneCenter, qreal zoneRadius) {
    qreal distance = std::sqrt(std::pow(agentPos.x() - zoneCenter.x(), 2) + std::pow(agentPos.y() - zoneCenter.y(), 2));
    return distance <= zoneRadius;
}

void FlagManager::handleTaggedAgent(Agent& agent) {
    if (agent.isCarryingFlag()) {
        resetFlag(agent.getTeam());
        agent.setCarryingFlag(false);
    }
}