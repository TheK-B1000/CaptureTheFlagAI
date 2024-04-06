#include "GameManager.h"
#include "GameField.h" 

#include <iostream>

GameManager::GameManager(GameField& gameField, QObject* parent)
    : QObject(parent),
    gameField(gameField),
    gameRunning(false) {}

void GameManager::startGame() {
    gameRunning = true;
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

    std::vector<Agent*> agentPointers;
    for (Agent& agent : agents) {
        agentPointers.push_back(&agent);
    }

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

    for (const auto& [teamId, score] : scores) {
        if (score >= 3) {
            return true;
        }
    }
    return false;
}

void GameManager::updateAgents() {
    // Implement this function if needed
}

const std::vector<Agent*>& GameManager::getBlueAgents() const {
    std::vector<Agent*> blueAgents;
    return blueAgents;
}

const std::vector<Agent*>& GameManager::getRedAgents() const {
    std::vector<Agent*> redAgents;
    return redAgents;
}