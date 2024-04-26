#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <utility>
#include <string>

class GameManager {
public:
    GameManager(int gameFieldWidth, int gameFieldHeight);

    std::pair<int, int> getFlagPosition(const std::string& side) const;
    void setFlagPosition(const std::string& side, int x, int y);
    void setTeamZonePosition(const std::string& side, int x, int y);
    bool isFlag(int x, int y) const;
    bool isTeamZone(int x, int y) const;
    std::pair<int, int> getEnemyFlagPosition(const std::string& side) const;
    std::pair<int, int> getTeamZonePosition(const std::string& side) const;

    void resetGame();

private:
    int gameFieldWidth, gameFieldHeight;
    std::pair<int, int> blueFlagPosition;
    std::pair<int, int> redFlagPosition;
    std::pair<int, int> blueTeamZonePosition;
    std::pair<int, int> redTeamZonePosition;
    int maxTime;
    int currentTime;
    bool gameOver;
};

#endif 