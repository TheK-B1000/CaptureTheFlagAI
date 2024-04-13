#include "GameManager.h"

GameManager::GameManager(int cols, int rows)
    : cols(cols), rows(rows),
    blueFlagPosition(0, rows / 2),
    redFlagPosition(cols - 1, rows / 2) {}

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