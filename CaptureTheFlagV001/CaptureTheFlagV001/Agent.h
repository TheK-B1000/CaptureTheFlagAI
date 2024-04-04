#ifndef AGENT_H
#define AGENT_H

#include <string>
#include <vector>
#include <utility>
#include "Pathfinder.h"
#include "Position.h" 
#include <QObject>

enum class AgentMode {
    Offensive,
    Defensive
};

class Agent : public QObject {
    Q_OBJECT

public:
    Agent(int id, int teamId, const Position& startPos);
    void moveTo(const Position& position);
    void update(); 
    int getX() const { return x; }
    int getY() const { return y; }
    bool isTagged() const;
    bool isOnCooldown() const;
    int getTeam() const;
    Position getPosition() const;
    void setTagged(bool tagged);
    void startCooldown(double duration);
    bool isCarryingFlag() const { return carryingFlag; }
    void setCarryingFlag(bool flag) { carryingFlag = flag; }
    void reset();
    void setPosition(const Position& position);


private:
    int id;
    Position currentPosition;
    int x, y;
    std::string side;
    int cols, rows;
    int teamId;
    bool tagged;
    bool onCooldown;
    const std::vector<std::vector<int>>& grid;
    Pathfinder* pathfinder;
    std::vector<Agent*> blueAgents;
    std::vector<Agent*> redAgents;
    bool carryingFlag;
};

#endif
