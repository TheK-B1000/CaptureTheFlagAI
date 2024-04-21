#include "Agent.h"
#include "Brain.h"
#include "Memory.h"
#include "GameManager.h"
#include <cmath>
#include <filesystem>
#include <iostream>
#include <QDebug>
#include <QGraphicsView>

Agent::Agent(int x, int y, std::string side, int cols, std::vector<std::vector<int>> grid, int rows, Pathfinder* pathfinder, float taggingDistance, Brain* brain, Memory* memory, GameManager* gameManager, std::vector<Agent*> blueAgents, std::vector<Agent*> redAgents)
    : x(x), y(y), side(side), cols(cols), grid(std::move(grid)), rows(rows), pathfinder(pathfinder), taggingDistance(taggingDistance), brain(brain), memory(memory), gameManager(gameManager), blueAgents(blueAgents), redAgents(redAgents),
    _isCarryingFlag(false), _isTagged(false), cooldownTimer(0), _isEnabled(true), previousX(x), previousY(y), stuckTimer(0) {}

void Agent::update(const std::vector<std::pair<int, int>>& otherAgentsPositions, std::vector<Agent*>& otherAgents) {
    // Updates memory of agent with position of other agents
    updateMemory(otherAgentsPositions);

    // is ai agent activated
    if (!_isEnabled) {
        return;
    }

    if (!_isCarryingFlag && !_isTagged && distanceToEnemyFlag() <= 10) {
        std::cout << "Agent " << side << " is attempting to grab the enemy flag." << std::endl;
    }

    if (_isCarryingFlag && checkInTeamZone()) {
        std::cout << "Agent " << side << " is attempting to capture the flag." << std::endl;
    }

    // is ai agent tagged
    if (_isTagged) {
        if (checkInTeamZone()) {
            // at base no longer tagged
            _isTagged = false;
        }
        else {
            // go to base to remove tag
            moveTowardsHomeZone();
            return;
        }
    }

    // if tagged while holding the flag the flag is reset
    if (_isCarryingFlag && _isTagged) {
        resetFlag();
    }

    // Ai makes decisions
    qDebug() << "Agent at (" << x << ", " << y << ") making decision...";
    BrainDecision decision = brain->makeDecision(_isCarryingFlag, isOpponentCarryingFlag(), _isTagged, checkInTeamZone(), distanceToEnemyFlag(), distanceToNearestEnemy(otherAgentsPositions));

    switch (decision) {
    case BrainDecision::Explore:
        qDebug() << "Exploring field";
        exploreField();
        break;
    case BrainDecision::GrabFlag:
        qDebug() << "Moving towards enemy flag";
        moveTowardsEnemyFlag();
        break;
    case BrainDecision::CaptureFlag:
        qDebug() << "Moving towards home zone";
        moveTowardsHomeZone();
        break;
    case BrainDecision::RecoverFlag:
        qDebug() << "Chasing opponent with flag";
        chaseOpponentWithFlag(otherAgentsPositions);
        break;
    case BrainDecision::TagEnemy:
        qDebug() << "Tagging enemy";
        tagEnemy(otherAgents);
        break;
    case BrainDecision::ReturnToHomeZone:
        qDebug() << "Returning to home zone";
        moveTowardsHomeZone();
        break;    
    default:
        qDebug() << "Exploring field";
        exploreField();
        break;
    }

    if (!path.empty()) {
        std::pair<int, int> nextStep = path.front();
        x = nextStep.first;
        y = nextStep.second;
        path.erase(path.begin());
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
    // checks every ai agents position
    for (const auto& position : otherAgentsPositions) {
        // retrieve opponent info from the map
        const auto& opponentInfoIt = memory->getOpponentInfo().find(position);
        if (opponentInfoIt != memory->getOpponentInfo().end()) {
            // retrieve the tuple value from the map
            const auto& opponentInfo = opponentInfoIt->second;
            // update memory with info
            memory->updateOpponentInfo(position.first, position.second, std::get<0>(opponentInfo), std::get<1>(opponentInfo).first, std::get<1>(opponentInfo).second);
        }
    }
}

void Agent::handleFlagInteractions() {
    // checks if not carrying flag, not tagged, and is within a 10 unit distance or in opponent team zone
    if (!_isCarryingFlag && !_isTagged && distanceToEnemyFlag() <= 10) {
        // checks to make sure no team ai agent is already holding a flag
        if (!isTeamCarryingFlag(blueAgents, redAgents)) {
            grabFlag();
        }
    }

    // if agent has flag and in home team zone
    if (_isCarryingFlag && checkInTeamZone()) {
        captureFlag();
    }

    // checks if ai agent is tagged while carrying the flag
    if (_isCarryingFlag && _isTagged) {
        resetFlag();
    }
}

// Checks if team is already holding the enemy flag
bool Agent::isTeamCarryingFlag(const std::vector<Agent*>& blueAgents, const std::vector<Agent*>& redAgents) {
    for (const Agent* teammate : (side == "blue" ? blueAgents : redAgents)) {
        if (teammate != this && teammate->isCarryingFlag()) {
            return true;
        }
    }
    return false;
}

// prevent ai agents from spam tagging
void Agent::handleCooldownTimer() {
    if (cooldownTimer > 0) {
        --cooldownTimer;
    }
}

bool Agent::isInFavorablePosition() {
    // Set a fixed proximity threshold value
    const float proximityThreshold = 10.0f;

    // Check if the agent is close enough to the enemy flag
    float distanceToFlag = distanceToEnemyFlag();
    if (distanceToFlag <= proximityThreshold) {
        return true;
    }

    // Check if there are no opponents nearby
    float distanceToNearestEnemy = this->distanceToNearestEnemy(getEnemyAgentPositions());
    if (distanceToNearestEnemy > proximityThreshold) {
        return true;
    }

    // If none of the above conditions are met, the agent is not in a favorable position
    return false;
}

std::vector<std::pair<int, int>> Agent::getEnemyAgentPositions() const {
    std::vector<std::pair<int, int>> enemyPositions;

    const auto& opponentInfoMap = memory->getOpponentInfo();
    for (const auto& entry : opponentInfoMap) {
        const auto& position = entry.first;
        double timeSinceLastSeen = memory->getTimeSinceLastSeen(position.first, position.second);

        // Consider the opponent's position if it was seen recently
        if (timeSinceLastSeen <= 5.0) {  // Adjust the time threshold as needed
            enemyPositions.push_back(position);
        }
    }

    return enemyPositions;
}

bool Agent::isOpponentCarryingFlag() const {
    const auto& opponentInfoMap = memory->getOpponentInfo();
    // iterate over opponenet info to find an opponent carrying the flag
    return std::any_of(opponentInfoMap.begin(), opponentInfoMap.end(), [](const auto& entry) {
        return std::get<0>(entry.second);
        });
}

std::pair<int, int> Agent::getEnemyFlagPosition() const {
    return gameManager->getEnemyFlagPosition(side);
}

float Agent::distanceToEnemyFlag() const {
    // Get the current position of the enemy flag
    std::pair<int, int> enemyFlagPos = getEnemyFlagPosition();

    // Calculate the distance between the agent and the enemy flag
    return std::hypot(enemyFlagPos.first - x, enemyFlagPos.second - y) * FIELD_WIDTH / GRID_SIZE;
}

float Agent::distanceToNearestEnemy(const std::vector<std::pair<int, int>>& otherAgentsPositions) const {
    float minDistance = std::numeric_limits<float>::max();
    // checks the distance between each enemy
    for (const auto& position : otherAgentsPositions) {
        float distance = std::hypot(position.first - x, position.second - y);
        minDistance = std::min(minDistance, distance);
    }
    return minDistance;
}

void Agent::exploreField() {
    // Check if the agent has reached the current target position
    if (!path.empty() && (x != path.back().first || y != path.back().second)) {
        // Agent has not reached the target position, continue following the current path
        std::pair<int, int> nextStep = path.front();
        x = nextStep.first;
        y = nextStep.second;
        path.erase(path.begin());
    }
    else {
        // Agent has reached the target position or the path is empty
        // Generate a new random target position within the game field boundaries
        int minX = 0, minY = 0, maxX = cols - 1, maxY = rows - 1;
        std::pair<int, int> targetPosition;
        do {
            targetPosition = pathfinder->getRandomFreePosition();
        } while (targetPosition.first < minX || targetPosition.first > maxX ||
            targetPosition.second < minY || targetPosition.second > maxY);

        // Calculate a new path to the target position
        path = pathfinder->findPath(x, y, targetPosition.first, targetPosition.second);
    }
}

void Agent::moveTowardsEnemyFlag() {
    std::pair<int, int> flagPos = gameManager->getEnemyFlagPosition(side);
    path = pathfinder->findPath(x, y, flagPos.first, flagPos.second);

    if (!path.empty()) {
        std::pair<int, int> nextStep = path.front();
        x = nextStep.first;
        y = nextStep.second;
        path.erase(path.begin());

        if (distanceToEnemyFlag() <= 1) {
            grabFlag();
        }
    }
    else {
        exploreField();
    }
}

void Agent::moveTowardsHomeZone() {
    std::pair<int, int> homePos = gameManager->getTeamZonePosition(side);
    int homeX = std::max(0, std::min(homePos.first, pathfinder->getCols() - 1));
    int homeY = std::max(0, std::min(homePos.second, pathfinder->getRows() - 1));

    path = pathfinder->findPath(x, y, homeX, homeY);
    if (!path.empty()) {
        std::pair<int, int> nextStep = path.front();
        int newX = std::max(0, std::min(nextStep.first, pathfinder->getCols() - 1));
        int newY = std::max(0, std::min(nextStep.second, pathfinder->getRows() - 1));
        x = newX;
        y = newY;
        path.erase(path.begin());
    }
}

void Agent::chaseOpponentWithFlag(const std::vector<std::pair<int, int>>& otherAgentsPositions) {
    std::pair<int, int> opponentWithFlag = std::make_pair(-1, -1);
    double minTimeSinceLastSeen = std::numeric_limits<double>::max();

    for (const auto& position : otherAgentsPositions) {
        bool hasFlag = memory->hasOpponentFlag(position.first, position.second);
        if (hasFlag) {
            double timeSinceLastSeen = memory->getTimeSinceLastSeen(position.first, position.second);
            if (timeSinceLastSeen < minTimeSinceLastSeen) {
                minTimeSinceLastSeen = timeSinceLastSeen;
                opponentWithFlag = position;
            }
        }
    }

    if (opponentWithFlag.first != -1 && opponentWithFlag.second != -1) {
        int opponentX = std::max(0, std::min(opponentWithFlag.first, pathfinder->getCols() - 1));
        int opponentY = std::max(0, std::min(opponentWithFlag.second, pathfinder->getRows() - 1));

        path = pathfinder->findPath(x, y, opponentX, opponentY);
        if (!path.empty()) {
            std::pair<int, int> nextStep = path.front();
            int newX = std::max(0, std::min(nextStep.first, pathfinder->getCols() - 1));
            int newY = std::max(0, std::min(nextStep.second, pathfinder->getRows() - 1));
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

    // checks over each agent
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
    return (side == "blue" && x >= 410) || (side == "red" && x < 410);
}

bool Agent::grabFlag() {
    if (!_isCarryingFlag && !_isTagged && distanceToEnemyFlag() <= 10) {
        _isCarryingFlag = true;
        return true;
    }
    return false;
}

bool Agent::captureFlag() {
    if (_isCarryingFlag && checkInTeamZone() && !_isTagged) {
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

bool Agent::checkInTeamZone() const {
    int teamZoneRadius = 40; 

    // Get the current flag position based on the agent's side
    std::pair<int, int> flagPosition = gameManager->getFlagPosition(side);

    // Calculate the distance between the agent and the flag position
    int distanceToFlag = std::sqrt(std::pow(x - flagPosition.first, 2) + std::pow(y - flagPosition.second, 2));

    // Check if the agent is within the team zone
    return distanceToFlag <= teamZoneRadius;
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