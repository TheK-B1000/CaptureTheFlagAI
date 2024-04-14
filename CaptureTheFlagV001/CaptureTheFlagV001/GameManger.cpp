#include "GameManager.h"

GameManager::GameManager(int cols, int rows)
    : cols(cols), rows(rows),
    blueFlagPosition(0, rows / 2),
    redFlagPosition(cols - 1, rows / 2) {}

std::pair<int, int> GameManager::getFlagPosition(const std::string& side) const {
    if (side == "blue") {
        return blueFlagPosition;
    }
    else if (side == "red") {
        return redFlagPosition;
    }
}

void GameManager::setFlagPosition(const std::string& side, int x, int y) {
    if (side == "blue") {
        blueFlagPosition = std::make_pair(x, y);
    }
    else if (side == "red") {
        redFlagPosition = std::make_pair(x, y);
    }
}

std::pair<int, int> GameManager::getEnemyFlagPosition(const std::string& side) const {
    return (side == "blue") ? redFlagPosition : blueFlagPosition;
}

std::pair<int, int> GameManager::getTeamZonePosition(const std::string& side) const {
    int teamZoneRadius = 40; // Radius of the team zone (half of the team zone diameter)

    // Get the current flag position based on the agent's side
    std::pair<int, int> flagPosition = getFlagPosition(side);

    // Calculate the center position of the team zone based on the flag position
    int teamZoneCenterX = flagPosition.first;
    int teamZoneCenterY = flagPosition.second;

    // Ensure the team zone center position is within the grid boundaries
    teamZoneCenterX = std::max(teamZoneRadius, std::min(teamZoneCenterX, cols - teamZoneRadius - 1));
    teamZoneCenterY = std::max(teamZoneRadius, std::min(teamZoneCenterY, rows - teamZoneRadius - 1));

    return std::make_pair(teamZoneCenterX, teamZoneCenterY);
}