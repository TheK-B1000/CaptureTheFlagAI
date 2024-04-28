#include "Agent.h"
#include "Brain.h"
#include "Memory.h"
#include "GameManager.h"
#include <cmath>
#include <filesystem>
#include <iostream>
#include <QDebug>
#include <QGraphicsView>
#include <memory>

Agent::Agent(int x, int y, std::string side, int gameFieldWidth, int gameFieldHeight, const std::shared_ptr<Pathfinder>& pathfinder, float taggingDistance, const std::shared_ptr<Brain>& brain, const std::shared_ptr<Memory>& memory, const std::shared_ptr<GameManager>& gameManager,
    std::vector<std::shared_ptr<Agent>>& blueAgents, std::vector<std::shared_ptr<Agent>>& redAgents)
    : x(x), y(y), side(side), gameFieldWidth(gameFieldWidth), gameFieldHeight(gameFieldHeight), pathfinder(pathfinder), taggingDistance(taggingDistance), brain(brain), memory(memory), gameManager(gameManager),
    _isCarryingFlag(false), _isTagged(false), cooldownTimer(0), _isEnabled(true), previousX(x), previousY(y), stuckTimer(0) {}

void Agent::update(const std::vector<std::pair<int, int>>& otherAgentsPositions, std::vector<Agent*>& otherAgents, const std::vector<std::shared_ptr<Agent>>& blueAgents, const std::vector<std::shared_ptr<Agent>>& redAgents, int elapsedTime) {
    // Updates memory of agent with position of other agents
    updateMemory(otherAgentsPositions);

    float movementSpeed = 100.0f; // Adjust the speed as needed
    float movementDistance = movementSpeed * elapsedTime / 1000.0f;

    // is ai agent activated
    if (!_isEnabled) {
        return;
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
        //exploreField();
        break;
    case BrainDecision::GrabFlag:
        qDebug() << "Moving towards enemy flag";
        //moveTowardsEnemyFlag();
        break;
    case BrainDecision::CaptureFlag:
        qDebug() << "Moving towards home zone";
        //moveTowardsHomeZone();
        break;
    case BrainDecision::RecoverFlag:
        qDebug() << "Chasing opponent with flag";
        //chaseOpponentWithFlag(otherAgentsPositions);
        break;
    case BrainDecision::TagEnemy:
        qDebug() << "Tagging enemy";
        //tagEnemy(otherAgents);
        break;
    case BrainDecision::ReturnToHomeZone:
        qDebug() << "Returning to home zone";
        //moveTowardsHomeZone();
        break;
    default:
        qDebug() << "Exploring field";
        //exploreField();
        break;
    }

    handleFlagInteractions(blueAgents, redAgents);
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

void Agent::handleFlagInteractions(const std::vector<std::shared_ptr<Agent>>& blueAgents, const std::vector<std::shared_ptr<Agent>>& redAgents) {
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

bool Agent::isTeamCarryingFlag(const std::vector<std::shared_ptr<Agent>>& blueAgents, const std::vector<std::shared_ptr<Agent>>& redAgents) {
    for (const auto& teammate : (getSide() == "blue" ? blueAgents : redAgents)) {
        if (teammate.get() != this && teammate->isCarryingFlag()) {
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
    return std::hypot(enemyFlagPos.first - x, enemyFlagPos.second - y);
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
    if (!path.empty()) {
        std::pair<int, int> nextStep = path.front();
        int newX = nextStep.first;
        int newY = nextStep.second;

        // Check if the new position is within the game field boundaries
        if (isValidPosition(newX, newY)) {
            x = newX;
            y = newY;
            path.erase(path.begin());
        }
        else {
            // The new position is outside the game field boundaries
            // Find an alternative path or direction to explore
            findAlternativePath();
        }
    }
    else {
        // Generate a new random target position within the game field boundaries
        int targetX = std::rand() % gameFieldWidth;
        int targetY = std::rand() % (gameFieldHeight - 1);  // Avoid the bottom row

        // Calculate a new path to the target position
        path = pathfinder->findPath(x, y, targetX, targetY);
    }
}

void Agent::moveTowardsEnemyFlag() {
    std::pair<int, int> flagPos = gameManager->getEnemyFlagPosition(side);
    path = pathfinder->findPath(x, y, flagPos.first, flagPos.second);

    if (path.empty()) {
        qDebug() << "No path to enemy flag found.";
        return;
    }

    // Log the entire path for debugging
    qDebug() << "Path to the enemy flag:";
    for (const auto& step : path) {
        qDebug() << "Step: (" << step.first << ", " << step.second << ")";
    }

    // Attempt to follow the path
    while (!path.empty()) {
        std::pair<int, int> nextStep = path.front();
        path.erase(path.begin());

        if (!isValidPosition(nextStep.first, nextStep.second)) {
            qDebug() << "Invalid position reached, stopping movement.";
            break;
        }

        // Move agent to the next step
        x = nextStep.first;
        y = nextStep.second;
        qDebug() << "Moved to (" << x << ", " << y << ")";

        if (distanceToEnemyFlag() <= 10) {
            qDebug() << "Flag within reach, attempting to grab.";
            if (grabFlag()) {
                qDebug() << "Flag captured!";
                break;
            }
        }
    }
}

void Agent::moveTowardsHomeZone() {
    std::pair<int, int> homePos = gameManager->getTeamZonePosition(side);
    path = pathfinder->findPath(x, y, homePos.first, homePos.second);

    if (!path.empty()) {
        std::pair<int, int> nextStep = path.front();
        int newX = nextStep.first;
        int newY = nextStep.second;

        // Validate the new position before updating
        if (isValidPosition(newX, newY)) {
            qDebug() << "Agent at (" << x << ", " << y << ") moving to (" << newX << ", " << newY << ") towards the home zone.";
            x = newX;
            y = newY;
            path.erase(path.begin());
        }
        else {
            // The new position is outside the game field boundaries
            // Adjust the position to the nearest valid position or prevent the movement
            // Example: Respawn the agent in the team area
            findAlternativePath();
        }
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
        int opponentX = std::max(0, std::min(opponentWithFlag.first, gameFieldWidth - 1));
        int opponentY = std::max(0, std::min(opponentWithFlag.second, gameFieldHeight - 1));

        path = pathfinder->findPath(x, y, opponentX, opponentY);
        if (!path.empty()) {
            std::pair<int, int> nextStep = path.front();
            int newX = std::max(0, std::min(nextStep.first, gameFieldWidth - 1));
            int newY = std::max(0, std::min(nextStep.second, gameFieldHeight - 1));

            // Check if the new position is within the game field boundaries
            if (newX >= 0 && newX < gameFieldWidth && newY >= 0 && newY < gameFieldHeight) {
                qDebug() << "Agent at (" << x << ", " << y << ") moving to (" << newX << ", " << newY << ") to chase the opponent with the flag.";
                x = newX;
                y = newY;
                path.erase(path.begin());
            }
            else {
                qDebug() << "Next position (" << newX << ", " << newY << ") is outside the game field boundaries.";
                findAlternativePath();
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
    if (gameManager == nullptr) {
        // Handle the case when the GameManager object is not initialized
        qDebug() << "Error: GameManager is not initialized";
        return false;
    }
    int teamZoneRadius = 40;

    // Get the current flag position based on the agent's side
    std::pair<int, int> flagPosition = gameManager->getFlagPosition(side);

    // Calculate the distance between the agent and the flag position
    int distanceToFlag = std::sqrt(std::pow(x - flagPosition.first, 2) + std::pow(y - flagPosition.second, 2));

    // Check if the agent is within the team zone
    return distanceToFlag <= teamZoneRadius;
}

void Agent::findAlternativePath() {
    // Check if the agent is near the bottom boundary
    if (y >= gameFieldHeight - 1) {
        // Move horizontally towards the enemy flag
        int flagX = gameManager->getEnemyFlagPosition(side).first;
        if (x < flagX) {
            x++;
        }
        else if (x > flagX) {
            x--;
        }
    }
    else {
        // Move vertically towards the enemy flag
        int flagY = gameManager->getEnemyFlagPosition(side).second;
        if (y < flagY) {
            y++;
        }
        else if (y > flagY) {
            y--;
        }
    }
}

bool Agent::isValidPosition(int newX, int newY) const {
    return newX >= 0 && newX < gameFieldWidth && newY >= 0 && newY < gameFieldHeight;
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