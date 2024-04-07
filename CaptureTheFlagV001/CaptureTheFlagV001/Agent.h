#ifndef AGENT_H
#define AGENT_H

#include <string>
#include <vector>
#include <utility>
#include "Pathfinder.h"
#include "Brain.h"
#include "Memory.h"
#include <QObject>

enum class AgentMode {
    Offensive,
    Defensive
};

class Agent : public QObject {
    Q_OBJECT

private:
    int x, y;
    int cols, rows;
    const std::vector<std::vector<int>>& grid;
    Pathfinder* pathfinder;
    Brain* brain;
    Memory* memory;
    bool _isCarryingFlag;
    bool _isTagged;
    int cooldownTimer;
    static const int cooldownDuration = 30;
    float taggingDistance;
    std::vector<std::pair<int, int>> path;
    bool _isEnabled;
    std::vector<Agent*> blueAgents;
    std::vector<Agent*> redAgents;
    int previousX, previousY;
    int stuckTimer;
    static const int stuckThreshold = 5;
    std::string side;


public:
    Agent(int x, int y, std::string side, int cols, const std::vector<std::vector<int>>& grid, int rows, Pathfinder* pathfinder, float taggingDistance, Brain* brain, Memory* memory, std::vector<Agent*> blueAgents, std::vector<Agent*> redAgents);
    void update(const std::vector<std::pair<int, int>>& otherAgentsPositions, std::vector<Agent*>& otherAgents);
    void updateMemory(const std::vector<std::pair<int, int>>& otherAgentsPositions);
    void handleFlagInteractions();
    void handleCooldownTimer();
    bool isOpponentCarryingFlag() const;
    float distanceToEnemyFlag() const;
    float distanceToNearestEnemy(const std::vector<std::pair<int, int>>& otherAgentsPositions) const;
    float distanceTo(const Agent* otherAgent) const;
    void exploreField();
    void moveTowardsEnemyFlag();
    void moveTowardsHomeZone();
    void chaseOpponentWithFlag(const std::vector<std::pair<int, int>>& otherAgentsPositions);
    void tagEnemy(std::vector<Agent*>& otherAgents);
    bool isOnEnemySide() const;
    bool grabFlag();
    bool captureFlag();
    void resetFlag();
    bool checkInHomeZone() const;
    std::pair<int, int> getDirectionToOpponent(int opponentX, int opponentY) const;
    void setIsTagged(bool val);
    bool isTagged() const;
    bool isCarryingFlag() const;
    void setCarryingFlag(bool carrying);
    void setEnabled(bool enabled);
    int getX() const { return x; }
    int getY() const { return y; }
    const std::vector<Agent*>& getBlueAgents() const { return blueAgents; }
    const std::vector<Agent*>& getRedAgents() const { return redAgents; }
    Brain* getBrain() const { return brain; }
    Memory* getMemory() const { return memory; }
    bool isTeamCarryingFlag(const std::vector<Agent*>& blueAgents, const std::vector<Agent*>& redAgents);
    std::string getSide() const { return side; }
    float getTaggingDistance() const { return taggingDistance; }
    int getCooldownTimer() const { return cooldownTimer; }
    int getCooldownDuration() const { return cooldownDuration; }
    void setCooldownTimer(int value) { cooldownTimer = value; }

signals:
    void blueFlagCaptured();
    void redFlagCaptured();
    void blueFlagReset();
    void redFlagReset();
};

#endif