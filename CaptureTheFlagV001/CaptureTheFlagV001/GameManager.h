#pragma once

#include "Agent.h"
#include "FlagManager.h"
#include "TagManager.h"

#include <vector>
#include <unordered_map>
#include <QObject>

class GameField;

class GameManager : public QObject {
    Q_OBJECT

public:
    GameManager(GameField* gameField, const std::vector<Agent*>& blueAgents, const std::vector<Agent*>& redAgents, QObject* parent = nullptr);

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
    std::vector<Agent*> blueAgents;
    std::vector<Agent*> redAgents;
};
