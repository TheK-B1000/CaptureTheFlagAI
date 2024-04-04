#ifndef FLAGMANAGER_H
#define FLAGMANAGER_H

#include "GameField.h"
#include "Agent.h"
#include <unordered_map>

class GameField;

class FlagManager {
public:
    FlagManager(GameField& gameField);

    void attemptFlagGrab(Agent& agent);
    void update();
    bool isFlagSuccessfullyReturned(const Agent& agent);
    void handleTaggedAgent(Agent& agent);
    void resetFlag(int teamId);

private:
    GameField& gameField;
    std::unordered_map<int, bool> flagStatus;
    std::unordered_map<int, QPointF> flagPositions;

    bool checkIfAgentInZone(const QPointF& agentPos, const QPointF& zoneCenter, qreal zoneRadius);
};

#endif
