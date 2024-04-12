#include "GameManager.h"
#include "GameField.h"
#include <QDebug>
#include <QFont>

GameManager::GameManager(GameField* gameField)
    : gameField(gameField), timeRemaining(600), blueScore(0), redScore(0) {
    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &GameManager::handleGameTimerTimeout);
    gameTimer->start(1000);
}

void GameManager::handleGameTimerTimeout() {
    timeRemaining--;

    qDebug() << "Time remaining:" << timeRemaining;
    qDebug() << "Blue score:" << blueScore;
    qDebug() << "Red score:" << redScore;

    if (timeRemaining <= 0) {
        qDebug() << "Stopping the game";
    }
    else {
        qDebug() << "Updating agents";
        updateAgents();
    }
}

void GameManager::updateAgents() {
    const std::vector<Agent*>& blueAgents = gameField->getBlueAgents();
    const std::vector<Agent*>& redAgents = gameField->getRedAgents();

    std::vector<std::pair<int, int>> bluePositions;
    std::vector<std::pair<int, int>> redPositions;

    for (Agent* agent : blueAgents) {
        bluePositions.emplace_back(agent->getX(), agent->getY());
    }

    for (Agent* agent : redAgents) {
        redPositions.emplace_back(agent->getX(), agent->getY());
    }

    std::vector<Agent*> blueAgentsCopy(blueAgents.begin(), blueAgents.end());
    std::vector<Agent*> redAgentsCopy(redAgents.begin(), redAgents.end());

    for (Agent* agent : blueAgentsCopy) {
        agent->update(redPositions, redAgentsCopy);
    }

    for (Agent* agent : redAgentsCopy) {
        agent->update(bluePositions, blueAgentsCopy);
    }

    gameField->getTagManager()->checkTagging(blueAgentsCopy, redAgentsCopy);
    gameField->viewport()->update();
}


