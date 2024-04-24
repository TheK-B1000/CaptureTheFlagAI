#ifndef AGENT_H
#define AGENT_H

#include <string>
#include <vector>
#include <utility>
#include "Pathfinder.h"
#include "Brain.h"
#include "Memory.h"
#include "GameManager.h"
#include <QObject>

const int FIELD_WIDTH = 20; // Field width in meters
const int GRID_SIZE = 100; // Grid size for discretization

class Agent : public QObject {
    Q_OBJECT

private:
    int x, y;
    int cols, rows;
    std::vector<std::vector<int>>& grid;
    Pathfinder* pathfinder;
    Brain* brain;
    Memory* memory;
    GameManager* gameManager;
    bool _isCarryingFlag;
    bool _isTagged;
    int cooldownTimer;
    static const int cooldownDuration = 30;
    float taggingDistance;
    std::vector<std::pair<int, int>> path;
    bool _isEnabled;
    int previousX, previousY;
    int stuckTimer;
    static const int stuckThreshold = 5;
    std::string side;

public:
    Agent(int x, int y, std::string side, int cols, int rows, std::vector<std::vector<int>>& grid, Pathfinder* pathfinder, float taggingDistance,
        Brain* brain, Memory* memory, GameManager* gameManager, std::vector<Agent*> blueAgents, std::vector<Agent*> redAgents);
    void update(const std::vector<std::pair<int, int>>& otherAgentsPositions, std::vector<Agent*>& otherAgents, const std::vector<std::unique_ptr<Agent>>& blueAgents, const std::vector<std::unique_ptr<Agent>>& redAgents);
    void updateMemory(const std::vector<std::pair<int, int>>& otherAgentsPositions);
    void handleFlagInteractions(const std::vector<std::unique_ptr<Agent>>& blueAgents, const std::vector<std::unique_ptr<Agent>>& redAgents);
    void handleCooldownTimer();
    bool isOpponentCarryingFlag() const;
    std::pair<int, int> getEnemyFlagPosition() const;
    float distanceToEnemyFlag() const;
    float distanceToNearestEnemy(const std::vector<std::pair<int, int>>& otherAgentsPositions) const;
    bool isValidPosition(int newX, int newY) const;
    void respawnInTeamArea();
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
    bool checkInTeamZone() const;
    std::pair<int, int> getDirectionToOpponent(int opponentX, int opponentY) const;
    void setIsTagged(bool val);
    bool isTagged() const;
    bool isCarryingFlag() const;
    void setCarryingFlag(bool carrying);
    int getX() const { return x; }
    int getY() const { return y; }
    void setX(int newX);
    void setY(int newY);
    void setEnabled(bool enabled);
    void decrementCooldownTimer();
    Brain* getBrain() const { return brain; }
    Memory* getMemory() const { return memory; }
    bool isTeamCarryingFlag(const std::vector<std::unique_ptr<Agent>>& blueAgents, const std::vector<std::unique_ptr<Agent>>& redAgents);
    std::string getSide() const { return side; }
    float getTaggingDistance() const { return taggingDistance; }
    int getCooldownTimer() const { return cooldownTimer; }
    int getCooldownDuration() const { return cooldownDuration; }
    void setCooldownTimer(int value) { cooldownTimer = value; }
    bool isInFavorablePosition();
    std::vector<std::pair<int, int>> getEnemyAgentPositions() const;

signals:
    void blueFlagCaptured();
    void redFlagCaptured();
    void blueFlagReset();
    void redFlagReset();
};

#endif