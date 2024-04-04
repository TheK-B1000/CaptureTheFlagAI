#include "Agent.h"

Agent::Agent(int id, int teamId, const Position& startPos)
    : id(id),
    teamId(teamId),
    currentPosition(startPos),
    tagged(false),
    onCooldown(false),
    x(startPos.x),
    y(startPos.y),
    cols(0),
    rows(0),
    grid(std::vector<std::vector<int>>()),
    pathfinder(nullptr) {}


void Agent::moveTo(const Position& position) {
    // Find the path to the target position using the pathfinder
    std::vector<Position> path = pathfinder->findPath(currentPosition, position);

    // Move the agent along the path
    if (!path.empty()) {
        currentPosition = path[0];
        x = currentPosition.x;
        y = currentPosition.y;
    }
}

void Agent::update() {
    // Update agent state here
    // Could involve finding a path and moving along it
}

bool Agent::isTagged() const {
    // Return whether the agent is currently tagged
    return tagged;
}

bool Agent::isOnCooldown() const {
    // Return whether the agent is currently on cooldown
    return onCooldown;
}

int Agent::getTeam() const {
    // Return the agent's team ID
    return teamId;
}

Position Agent::getPosition() const {
    // Return the agent's current position
    return currentPosition;
}

void Agent::setTagged(bool tagged) {
    // Set the agent's tagged status
    this->tagged = tagged;
}

void Agent::startCooldown(double duration) {
    // Start the cooldown for the agent
    onCooldown = true;
    // Implement a timer or scheduling mechanism to end the cooldown after the specified duration
}

void Agent::reset()
{
}

void Agent::setPosition(const Position& position) {
    currentPosition = position;
    x = currentPosition.x; 
    y = currentPosition.y;
}