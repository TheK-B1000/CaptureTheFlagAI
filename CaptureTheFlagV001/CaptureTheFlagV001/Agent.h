#ifndef AGENT_H
#define AGENT_H

#include <string>
#include <vector>
#include <utility>
#include "Pathfinder.h"
#include "Brain.h"
#include "Memory.h"

class Agent {
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
    float taggingDistance;
    std::vector<std::pair<int, int>> path;
    bool _isEnabled;
    int previousX, previousY;
    int stuckTimer;
    std::string side;

public:
    Agent(int x, int y, std::string side, int cols, const std::vector<std::vector<int>>& grid, int rows, Pathfinder* pathfinder, float taggingDistance, Brain* brain, Memory* memory);

    void update(const std::vector<std::pair<int, int>>& otherAgentsPositions, std::vector<Agent*>& otherAgents);
    void handleFlagInteractions();
    void handleCooldownTimer();
    bool isOpponentCarryingFlag() const;
    float distanceToEnemyFlag() const;
    float distanceToNearestEnemy(const std::vector<std::pair<int, int>>& otherAgentsPositions) const;
    void exploreField();
    void moveTowardsEnemyFlag();
    void moveTowardsHomeZone();
    void chaseOpponentWithFlag(const std::vector<std::pair<int, int>>& otherAgentsPositions);
    void tagEnemy(std::vector<Agent*>& otherAgents);
    bool grabFlag();
    bool captureFlag();
    void resetFlag();
    bool checkInHomeZone() const;
    void setIsTagged(bool val);
    bool isTagged() const;
    bool isCarryingFlag() const;
    std::pair<int, int> getHomeZonePosition() const;
    int getX() const { return x; }
    int getY() const { return y; }
    void moveTo(const std::pair<int, int>& position);
    void setEnabled(bool enabled);

    float distanceTo(const Agent* otherAgent) const {
        return std::hypot(otherAgent->x - x, otherAgent->y - y);
    }

    void setCarryingFlag(bool carrying) {
        _isCarryingFlag = carrying;
    }

    Brain* getBrain() const { return brain; }
    Memory* getMemory() const { return memory; }
    std::string getSide() const { return side; }
};

#endif