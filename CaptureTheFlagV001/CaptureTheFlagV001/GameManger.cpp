#include "GameManager.h"

GameManager::GameManager(int cols, int rows)
    : cols(cols), rows(rows),
    blueFlagPosition(3, 14), // Set the correct initial position for the blue flag
    redFlagPosition(35, 14), // Set the correct initial position for the red flag
    blueTeamZonePosition(0, rows / 2),
    redTeamZonePosition(cols - 1, rows / 2) {}

std::pair<int, int> GameManager::getFlagPosition(const std::string& side) const {
    if (side == "blue") {
        return blueFlagPosition;
    }
    else if (side == "red") {
        return redFlagPosition;
    }
    else {
        // Return a default value or throw an exception for an invalid side
        return std::make_pair(-1, -1);
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

void GameManager::setTeamZonePosition(const std::string& side, int x, int y) {
    if (side == "blue") {
        blueTeamZonePosition = std::make_pair(x, y);
    }
    else if (side == "red") {
        redTeamZonePosition = std::make_pair(x, y);
    }
}

bool GameManager::isFlag(int x, int y) const {
    return (x == blueFlagPosition.first && y == blueFlagPosition.second) ||
        (x == redFlagPosition.first && y == redFlagPosition.second);
}

bool GameManager::isTeamZone(int x, int y) const {
    const int teamZoneRadius = 40;
    const std::pair<int, int> blueTeamZoneCenter = getTeamZonePosition("blue");
    const std::pair<int, int> redTeamZoneCenter = getTeamZonePosition("red");

    // Calculate squared distances from the position (x, y) to the team zone centers
    const int blueDistSquared = (x - blueTeamZoneCenter.first) * (x - blueTeamZoneCenter.first) +
        (y - blueTeamZoneCenter.second) * (y - blueTeamZoneCenter.second);
    const int redDistSquared = (x - redTeamZoneCenter.first) * (x - redTeamZoneCenter.first) +
        (y - redTeamZoneCenter.second) * (y - redTeamZoneCenter.second);

    // Check if the position is within the radius of either team zone
    return blueDistSquared <= teamZoneRadius * teamZoneRadius ||
        redDistSquared <= teamZoneRadius * teamZoneRadius;
}

void GameManager::resetGame() {
    // Reset flag positions to their initial values
    blueFlagPosition = std::make_pair(0, rows / 2);
    redFlagPosition = std::make_pair(cols - 1, rows / 2);

    // Reset game state variables
    currentTime = maxTime;
    gameOver = false;
}