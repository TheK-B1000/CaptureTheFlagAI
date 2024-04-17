#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <utility>
#include <string>

class GameManager {
public:
    GameManager(int cols, int rows);

    std::pair<int, int> getFlagPosition(const std::string& side) const;
    void setFlagPosition(const std::string& side, int x, int y);
    std::pair<int, int> getEnemyFlagPosition(const std::string& side) const;
    std::pair<int, int> getTeamZonePosition(const std::string& side) const;

    void resetGame();

private:
    int cols;
    int rows;
    std::pair<int, int> blueFlagPosition;
    std::pair<int, int> redFlagPosition;
    int maxTime;
    int currentTime;
    bool gameOver;
};

#endif 