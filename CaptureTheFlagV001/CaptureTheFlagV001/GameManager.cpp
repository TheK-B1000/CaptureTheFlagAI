#include "GameManager.h"
#include "GameField.h" 

#include <iostream>

GameManager::GameManager(GameField* gameField, const std::vector<Agent*>& blueAgents, const std::vector<Agent*>& redAgents, QObject* parent)
    : QObject(parent), gameField(*gameField), gameRunning(false),
    flagManager(this), tagManager(this), blueAgents(blueAgents), redAgents(redAgents) {
}

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

const std::vector<Agent*>& GameManager::getBlueAgents() const {
    return blueAgents;
}

const std::vector<Agent*>& GameManager::getRedAgents() const {
    return redAgents;
}