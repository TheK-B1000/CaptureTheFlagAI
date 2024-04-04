#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "GameField.h"
#include "FlagManager.h"
#include "TagManager.h"
#include <vector>
#include <unordered_map>

class GameManager {
public:
    GameManager(GameField& gameField);

    void startGame();
    void endGame();
    void updateGame();
    void updateAgents();

    const std::vector<Agent*>& getBlueAgents() const;
    const std::vector<Agent*>& getRedAgents() const;
    const std::vector<Agent*>& getAgents() const;

private:
    GameField& gameField;
    bool gameRunning;
    std::vector<Agent> agents;
    FlagManager flagManager;
    TagManager tagManager;
    std::unordered_map<int, int> scores;
    void resetGameElements();
    void initializeScores();
    bool shouldEndGame();
};

#endif