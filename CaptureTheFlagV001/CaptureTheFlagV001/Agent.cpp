#include "Agent.h"
#include "Brain.h"
#include "Memory.h"
#include <cmath>
#include <filesystem>
#include <iostream>

Agent::Agent(int x, int y, std::string side, int cols, const std::vector<std::vector<int>>& grid, int rows, Pathfinder* pathfinder, float taggingDistance, Brain* brain, Memory* memory, GameManager* gameManager, std::vector<Agent*> blueAgents, std::vector<Agent*> redAgents, int gameFieldX, int gameFieldY, int gameFieldWidth, int gameFieldHeight)
    : x(x), y(y), side(side), cols(cols), grid(grid), rows(rows), pathfinder(pathfinder), taggingDistance(taggingDistance), brain(brain), memory(memory), gameManager(gameManager), blueAgents(blueAgents), redAgents(redAgents),
    _isCarryingFlag(false), _isTagged(false), cooldownTimer(0), _isEnabled(true), previousX(x), previousY(y), stuckTimer(0),
    gameFieldX(gameFieldX), gameFieldY(gameFieldY), gameFieldWidth(gameFieldWidth), gameFieldHeight(gameFieldHeight),
    SIDE_BOUNDARY_X(cols / 2) {}

void Agent::update(const std::vector<std::pair<int, int>>& otherAgentsPositions, std::vector<Agent*>& otherAgents) {
    updateMemory(otherAgentsPositions);

    if (!_isEnabled) {
        return;
    }

    if (_isTagged) {
        if (checkInHomeZone()) {
            _isTagged = false;
        }
        else {
            moveTowardsHomeZone();
            return;
        }
    }

    if (_isCarryingFlag && _isTagged) {
        resetFlag();
    }

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
    break;    default:
        exploreField();
        break;
    }

    if (!path.empty()) {
        std::pair<int, int> nextStep = path.front();
        if (isWithinField(nextStep.first, nextStep.second)) {
            x = nextStep.first;
            y = nextStep.second;
            path.erase(path.begin());
        }
        else {
            path.clear(); // Clear the path if the next step is outside the field
        }
    }

    if (path.empty() && (x == previousX && y == previousY)) {
        stuckTimer++;
        if (stuckTimer >= stuckThreshold) {
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
    handleFlagInteractions();
    handleCooldownTimer();
}


void Agent::updateMemory(const std::vector<std::pair<int, int>>& otherAgentsPositions) {
    for (const auto& position : otherAgentsPositions) {
        const auto& opponentInfo = memory->getOpponentInfo(position.first, position.second);
        memory->updateOpponentInfo(position.first, position.second, std::get<0>(opponentInfo), std::get<1>(opponentInfo));
    }
}

void Agent::handleFlagInteractions() {
    if (!_isCarryingFlag && !_isTagged && distanceToEnemyFlag() <= 10) {
        if (!isTeamCarryingFlag(blueAgents, redAgents)) {
            grabFlag();
        }
    }

    if (_isCarryingFlag && checkInHomeZone()) {
        captureFlag();
    }

    if (_isCarryingFlag && _isTagged) {
        resetFlag();
    }
}

bool Agent::isTeamCarryingFlag(const std::vector<Agent*>& blueAgents, const std::vector<Agent*>& redAgents) {
    for (const Agent* teammate : (side == "blue" ? blueAgents : redAgents)) {
        if (teammate != this && teammate->isCarryingFlag()) {
            return true;
        }
    }
    return false;
}

void Agent::handleCooldownTimer() {
    if (cooldownTimer > 0) {
        --cooldownTimer;
    }
}

bool Agent::isOpponentCarryingFlag() const {
    const auto& opponentInfoMap = memory->getOpponentInfo();
    return std::any_of(opponentInfoMap.begin(), opponentInfoMap.end(), [](const auto& entry) {
        return std::get<0>(entry.second);
        });
}

float Agent::distanceToEnemyFlag() const {
    int enemyFlagX = (side == "blue") ? cols - 1 : 0;
    int enemyFlagY = rows / 2;
    return std::hypot(enemyFlagX - x, enemyFlagY - y) * FIELD_WIDTH / GRID_SIZE;
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
        std::pair<int, int> targetPosition;
        do {
            targetPosition = pathfinder->getRandomFreePosition();
        } while (
            targetPosition.first < gameFieldX || targetPosition.first >= gameFieldX + gameFieldWidth ||
            targetPosition.second < gameFieldY || targetPosition.second >= gameFieldY + gameFieldHeight ||
            grid[targetPosition.second][targetPosition.first] == 1
            );
        path = pathfinder->findPath(x, y, targetPosition.first, targetPosition.second);
    }
    if (!path.empty()) {
        std::pair<int, int> nextStep = path.front();

        // Check if the next step is within the game field boundaries
        if (nextStep.first >= 0 && nextStep.first < cols && nextStep.second >= 0 && nextStep.second < rows) {
            // Update the agent's position with the next step's coordinates
            int newX = nextStep.first;
            int newY = nextStep.second;

            // Ensure the new position stays within the boundaries
            newX = std::max(0, std::min(newX, cols - 1));
            newY = std::max(0, std::min(newY, rows - 1));

            x = newX;
            y = newY;
            path.erase(path.begin());
        }
        else {
            // If the next step is outside the boundaries, clear the path and generate a new one
            path.clear();
        }
    }
}

void Agent::moveTowardsEnemyFlag() {
    std::pair<int, int> flagPos = gameManager->getEnemyFlagPosition(side);
    int enemyFlagX = flagPos.first;
    int enemyFlagY = flagPos.second;

    // Make sure the coordinates are within the grid boundaries
    enemyFlagX = std::max(0, std::min(enemyFlagX, cols - 1));
    enemyFlagY = std::max(0, std::min(enemyFlagY, rows - 1));
    path = pathfinder->findPath(x, y, enemyFlagX, enemyFlagY);
    if (!path.empty()) {
        std::pair<int, int> nextStep = path.front();
        int newX = nextStep.first;
        int newY = nextStep.second;

        // Make sure the new position is within the grid boundaries
        newX = std::max(0, std::min(newX, cols - 1));
        newY = std::max(0, std::min(newY, rows - 1));

        x = newX;
        y = newY;
        path.erase(path.begin());

        if (distanceToEnemyFlag() <= 10) {
            grabFlag();
        }
    }
}

void Agent::moveTowardsHomeZone() {
    std::pair<int, int> homePos = gameManager->getTeamZonePosition(side);
    int homeX = homePos.first;
    int homeY = homePos.second;

    // Make sure the coordinates are within the grid boundaries
    homeX = std::max(0, std::min(homeX, cols - 1));
    homeY = std::max(0, std::min(homeY, rows - 1));
    path = pathfinder->findPath(x, y, homeX, homeY);
    if (!path.empty()) {
        std::pair<int, int> nextStep = path.front();
        int newX = nextStep.first;
        int newY = nextStep.second;

        // Ensure the new position stays within the boundaries
        newX = std::max(0, std::min(newX, cols - 1));
        newY = std::max(0, std::min(newY, rows - 1));

        x = newX;
        y = newY;
        path.erase(path.begin());
    }
}

void Agent::chaseOpponentWithFlag(const std::vector<std::pair<int, int>>& otherAgentsPositions) {
    std::pair<int, int> opponentWithFlag = std::make_pair(-1, -1);
    double minTimeSinceLastSeen = std::numeric_limits<double>::max();

    for (const auto& position : otherAgentsPositions) {
        const auto& opponentInfo = memory->getOpponentInfo(position.first, position.second);
        if (std::get<0>(opponentInfo)) {
            double timeSinceLastSeen = memory->getTimeSinceLastSeen(position.first, position.second).count();
            if (timeSinceLastSeen < minTimeSinceLastSeen) {
                minTimeSinceLastSeen = timeSinceLastSeen;
                opponentWithFlag = position;
            }
        }
    }

    if (opponentWithFlag.first != -1 && opponentWithFlag.second != -1) {
        path = pathfinder->findPath(x, y, opponentWithFlag.first, opponentWithFlag.second);
        if (!path.empty()) {
            std::pair<int, int> nextStep = path.front();
            int newX = nextStep.first;
            int newY = nextStep.second;

            // Ensure the new position stays within the boundaries
            newX = std::max(0, std::min(newX, cols - 1));
            newY = std::max(0, std::min(newY, rows - 1));

            x = newX;
            y = newY;
            path.erase(path.begin());
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
        if (agent->side != side && !agent->isTagged() && agent->isOnEnemySide()) {
            float distance = std::hypot(agent->x - x, agent->y - y);
            if (distance < minDistance && distance <= taggingDistance) {
                nearestEnemy = agent;
                minDistance = distance;
            }
        }
    }

    if (nearestEnemy != nullptr) {
        nearestEnemy->setIsTagged(true);
        setCooldownTimer(getCooldownDuration());
    }
}

bool Agent::isOnEnemySide() const {
    return (side == "blue" && x >= SIDE_BOUNDARY_X) || (side == "red" && x < SIDE_BOUNDARY_X);
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

        if (side == "blue") {
            std::cout << "Blue flag captured!" << std::endl;
        }
        else {
            std::cout << "Red flag captured!" << std::endl;
        }

        resetFlag();

        return true;
    }
    return false;
}
void Agent::resetFlag() {
    _isCarryingFlag = false;

    if (side == "blue") {
        std::cout << "Red flag reset!" << std::endl;
    }
    else {
        std::cout << "Blue flag reset!" << std::endl;
    }
}

bool Agent::checkInHomeZone() const {
    if (side == "blue") {
        int homeZoneX = 0;
        int homeZoneY = 0;
        int homeZoneWidth = cols / 2;
        int homeZoneHeight = rows;
        return (x >= homeZoneX && x < homeZoneX + homeZoneWidth && y >= homeZoneY && y < homeZoneY + homeZoneHeight);
    }
    else {
        int homeZoneX = cols / 2;
        int homeZoneY = 0;
        int homeZoneWidth = cols / 2;
        int homeZoneHeight = rows;
        return (x >= homeZoneX && x < homeZoneX + homeZoneWidth && y >= homeZoneY && y < homeZoneY + homeZoneHeight);
    }
}

std::pair<int, int> Agent::getHomeZonePosition() const {
    int homeX = (side == "blue") ? 0 : cols - 1;
    int homeY = rows / 2;
    return std::make_pair(homeX, homeY);
}

void Agent::setX(int newX) {
    x = newX;
}

void Agent::setY(int newY) {
    y = newY;
}

void Agent::decrementCooldownTimer() {
    if (cooldownTimer > 0) {
        --cooldownTimer;
    }
}

bool Agent::isWithinField(int x, int y) const {
    return x >= gameFieldX && x < gameFieldX + gameFieldWidth && y >= gameFieldY && y < gameFieldY + gameFieldHeight;
}

std::pair<int, int> Agent::getDirectionToOpponent(int opponentX, int opponentY) const {
    int dx = opponentX - x;
    int dy = opponentY - y;
    return std::make_pair(dx, dy);
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

void Agent::setCarryingFlag(bool carrying) {
    _isCarryingFlag = carrying;
}

void Agent::setEnabled(bool enabled) {
    _isEnabled = enabled;
}

float Agent::distanceTo(const Agent* otherAgent) const {
    return std::hypot(otherAgent->x - x, otherAgent->y - y);
}