#include "Agent.h"
#include "Brain.h"
#include "Memory.h"
#include <cmath>
#include <iostream>

Agent::Agent(int x, int y, std::string side, int cols, const std::vector<std::vector<int>>& grid, int rows, Pathfinder* pathfinder, float taggingDistance, Brain* brain, Memory* memory)
    : x(x), y(y), side(side), cols(cols), grid(grid), rows(rows), pathfinder(pathfinder), taggingDistance(taggingDistance), brain(brain), memory(memory),
    _isCarryingFlag(false), _isTagged(false), cooldownTimer(0), _isEnabled(true), previousX(x), previousY(y), stuckTimer(0) {}

void Agent::update(const std::vector<std::pair<int, int>>& otherAgentsPositions, std::vector<Agent*>& otherAgents) {
    if (!_isEnabled) {
        return;
    }

    if (_isTagged && checkInHomeZone()) {
        _isTagged = false;
    }
    else if (_isTagged) {
        moveTowardsHomeZone();
    }

    handleFlagInteractions();

    BrainDecision decision = brain->makeDecision(_isCarryingFlag, isOpponentCarryingFlag(), _isTagged, checkInHomeZone(), distanceToEnemyFlag(), distanceToNearestEnemy(otherAgentsPositions));

    switch (decision) {
    case BrainDecision::Explore:
        exploreField();
        break;
    case BrainDecision::GrabFlag:
        moveTowardsEnemyFlag();
        break;
    case BrainDecision::CaptureFlag:
        moveTowardsHomeZone();
        break;
    case BrainDecision::RecoverFlag:
        chaseOpponentWithFlag(otherAgentsPositions);
        break;
    case BrainDecision::TagEnemy:
        tagEnemy(otherAgents);
        break;
    case BrainDecision::ReturnToHomeZone:
        moveTowardsHomeZone();
        break;
    default:
        exploreField();
        break;
    }

    if (!path.empty()) {
        moveTo(path.front());
        path.erase(path.begin());
    }

    if (path.empty() && (x == previousX && y == previousY)) {
        stuckTimer++;
        if (stuckTimer >= 5) {
            std::pair<int, int> targetPosition = pathfinder->getRandomFreePosition();
            path = pathfinder->findPath(x, y, targetPosition.first, targetPosition.second);
            stuckTimer = 0;
        }
    }
    else {
        stuckTimer = 0;
    }

    previousX = x;
    previousY = y;
    handleCooldownTimer();
}

void Agent::handleFlagInteractions() {
    if (!_isCarryingFlag && !_isTagged && distanceToEnemyFlag() <= 10) {
        grabFlag();
    }
    if (_isCarryingFlag && _isTagged) {
        resetFlag();
    }
    if (_isCarryingFlag && checkInHomeZone()) {
        captureFlag();
    }
}

void Agent::handleCooldownTimer() {
    if (cooldownTimer > 0) {
        --cooldownTimer;
    }
}

float Agent::distanceToEnemyFlag() const {
    int enemyFlagX = (side == "blue") ? cols - 1 : 0;
    int enemyFlagY = rows / 2;
    return std::hypot(enemyFlagX - x, enemyFlagY - y);
}

float Agent::distanceToNearestEnemy(const std::vector<std::pair<int, int>>& otherAgentsPositions) const {
    float minDistance = std::numeric_limits<float>::max();
    for (const auto& position : otherAgentsPositions) {
        float distance = std::hypot(position.first - x, position.second - y);
        minDistance = std::min(minDistance, distance);
    }
    return minDistance;
}

void Agent::exploreField() {
    if (path.empty()) {
        std::pair<int, int> targetPosition = pathfinder->getRandomFreePosition();
        path = pathfinder->findPath(x, y, targetPosition.first, targetPosition.second);
    }
    if (!path.empty()) {
        moveTo(path.front());
        path.erase(path.begin());
    }
}

void Agent::moveTowardsEnemyFlag() {
    int enemyFlagX = (side == "blue") ? cols - 1 : 0;
    int enemyFlagY = rows / 2;
    path = pathfinder->findPath(x, y, enemyFlagX, enemyFlagY);
    if (!path.empty()) {
        moveTo(path.front());
        path.erase(path.begin());
    }
}

void Agent::moveTowardsHomeZone() {
    std::pair<int, int> homePosition = getHomeZonePosition();
    path = pathfinder->findPath(x, y, homePosition.first, homePosition.second);
    if (!path.empty()) {
        moveTo(path.front());
        path.erase(path.begin());
    }
}

void Agent::chaseOpponentWithFlag(const std::vector<std::pair<int, int>>& otherAgentsPositions) {
    std::pair<int, int> opponentWithFlag = memory->getOpponentWithFlag();
    if (opponentWithFlag.first != -1 && opponentWithFlag.second != -1) {
        std::pair<int, int> predictedPosition = memory->getPredictedOpponentPosition(opponentWithFlag);
        if (grid[predictedPosition.second][predictedPosition.first] != 1) {
            path = pathfinder->findPath(x, y, predictedPosition.first, predictedPosition.second);
            if (!path.empty()) {
                moveTo(path.front());
                path.erase(path.begin());
            }
        }
    }
}

void Agent::tagEnemy(std::vector<Agent*>& otherAgents) {
    if (_isTagged || cooldownTimer > 0) {
        return;
    }

    Agent* nearestEnemy = nullptr;
    float minDistance = std::numeric_limits<float>::max();

    for (Agent* agent : otherAgents) {
        if (agent->side != side && !agent->isTagged()) {
            float distance = std::hypot(agent->x - x, agent->y - y);
            if (distance < minDistance && distance <= taggingDistance) {
                nearestEnemy = agent;
                minDistance = distance;
            }
        }
    }

    if (nearestEnemy != nullptr) {
        nearestEnemy->setIsTagged(true);
        cooldownTimer = 30;
    }
}

bool Agent::grabFlag() {
    if (!_isCarryingFlag && !_isTagged && distanceToEnemyFlag() <= 10) {
        _isCarryingFlag = true;
        return true;
    }
    return false;
}

bool Agent::captureFlag() {
    if (_isCarryingFlag && checkInHomeZone() && !_isTagged) {
        _isCarryingFlag = false;
        std::cout << side << " flag captured!" << std::endl;
        resetFlag();
        return true;
    }
    return false;
}

void Agent::resetFlag() {
    _isCarryingFlag = false;
    std::cout << (side == "blue" ? "Red" : "Blue") << " flag reset!" << std::endl;
}

bool Agent::checkInHomeZone() const {
    int homeZoneX = (side == "blue") ? 0 : cols / 2;
    int homeZoneWidth = cols / 2;
    return (x >= homeZoneX && x < homeZoneX + homeZoneWidth);
}

std::pair<int, int> Agent::getHomeZonePosition() const {
    int homeX = (side == "blue") ? 0 : cols - 1;
    int homeY = rows / 2;
    return std::make_pair(homeX, homeY);
}

void Agent::moveTo(const std::pair<int, int>& position) {
    x = position.first;
    y = position.second;
}

void Agent::setIsTagged(bool val) {
    _isTagged = val;
}

bool Agent::isTagged() const {
    return _isTagged;
}

bool Agent::isCarryingFlag() const {
    return _isCarryingFlag;
}

bool Agent::isOpponentCarryingFlag() const {
    return memory->isAnyOpponentCarryingFlag();
}
void Agent::setEnabled(bool enabled) {
    _isEnabled = enabled;
}


