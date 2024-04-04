// GameManager.cpp

#include "GameManager.h"
#include <iostream>

GameManager::GameManager(GameField& gameField)
    : gameField(gameField), gameRunning(false),
    flagManager(gameField),
    tagManager(gameField, flagManager) {
    // Initialize agents and other necessary game components here
    // Example: Create agents for each team
    agents.emplace_back(0, 0, QPointF(50, 300)); // Blue team agent
    agents.emplace_back(1, 1, QPointF(750, 300)); // Red team agent


}


void GameManager::startGame() {
    gameRunning = true;
    resetGameElements();
    initializeScores();
}

void GameManager::endGame() {
    gameRunning = false;
    // Process the end of the game, such as determining the winner
    int winningTeam = -1;
    int highestScore = 0;
    for (const auto& [teamId, score] : scores) {
        if (score > highestScore) {
            highestScore = score;
            winningTeam = teamId;
        }
    }
    if (winningTeam != -1) {
        std::cout << "Team " << winningTeam << " wins with a score of " << highestScore << "!" << std::endl;
    }
    else {
        std::cout << "The game ended in a tie!" << std::endl;
    }
}

void GameManager::updateGame() {
    if (!gameRunning) return;

    // Update the game state for this tick
    for (Agent& agent : agents) {
        agent.update();

        // Handle interactions with flags and tagging
        flagManager.attemptFlagGrab(agent);
        if (flagManager.isFlagSuccessfullyReturned(agent)) {
            scores[agent.getTeam()]++; // Increment score for the agent's team
        }
    }

    // Create a temporary vector of Agent* pointers
    std::vector<Agent*> agentPointers;
    for (Agent& agent : agents) {
        agentPointers.push_back(&agent);
    }

    // Update managers
    tagManager.checkTags(agentPointers);
    flagManager.update();

    // Check if game should end
    if (shouldEndGame()) {
        endGame();
    }
}


void GameManager::initializeScores() {
    // Initialize scores for each team
    scores.clear();
    scores[0] = 0; // Blue team score
    scores[1] = 0; // Red team score
}

bool GameManager::shouldEndGame() {
    // Determine if game conditions indicate the end of the game
    // Example: End the game if any team reaches a score of 3
    for (const auto& [teamId, score] : scores) {
        if (score >= 3) {
            return true;
        }
    }
    return false;
}

void GameManager::updateAgents() {

}