#ifndef PATHFINDER_H
#define PATHFINDER_H

#include <vector>
#include <utility>
#include <unordered_map>
#include "Memory.h"

class Pathfinder {
public:
    Pathfinder(int gameFieldWidth, int gameFieldHeight);
    void setDynamicObstacles(const std::vector<std::pair<int, int>>& obstacles);
    std::vector<std::pair<int, int>> findPath(int startX, int startY, int goalX, int goalY);
    std::pair<int, int> getRandomFreePosition();

private:
    int gameFieldWidth;
    int gameFieldHeight;
    std::vector<std::pair<int, int>> dynamicObstacles;

    double calculateHeuristic(int x1, int y1, int x2, int y2);
    std::vector<std::pair<int, int>> getNeighbors(int x, int y);
    bool isValidPosition(int x, int y);
};

#endif // PATHFINDER_H