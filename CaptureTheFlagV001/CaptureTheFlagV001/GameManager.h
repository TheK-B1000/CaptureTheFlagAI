#pragma once

#include "GameField.h"
#include "Agent.h"
#include "FlagManager.h"
#include "TagManager.h"

#include <vector>
#include <unordered_map>
#include <QObject>

class GameManager : public QObject {
    Q_OBJECT

public:
    GameManager(GameField& gameField, QObject* parent = nullptr);

    void startGame();
    void endGame();
    void updateGame();

    const std::vector<Agent*>& getBlueAgents() const;
    const std::vector<Agent*>& getRedAgents() const;

private:
    void initializeScores();
    bool shouldEndGame();
    void updateAgents();

    GameField& gameField;
    bool gameRunning;
    std::vector<Agent> agents;
    FlagManager flagManager;
    TagManager tagManager;
    std::unordered_map<int, int> scores; 
};
