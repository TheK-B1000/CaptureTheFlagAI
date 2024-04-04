#ifndef PATHFINDER_H
#define PATHFINDER_H

#include <vector>
#include <unordered_map>
#include <utility>
#include <cstdlib>
#include <algorithm>


class Pathfinder {
private:
    std::vector<std::vector<int>> grid;
    int rows, cols;
    std::vector<std::pair<int, int>> dynamicObstacles;

    double calculateHeuristic(int x1, int y1, int x2, int y2);
    std::vector<std::pair<int, int>> getNeighbors(int x, int y);

public:
    Pathfinder(const std::vector<std::vector<int>>& grid);

    void setDynamicObstacles(const std::vector<std::pair<int, int>>& obstacles);
    std::vector<std::pair<int, int>> findPath(int startX, int startY, int goalX, int goalY);
    std::pair<int, int> getRandomFreePosition();

    std::vector<std::vector<int>> getGrid() const {
        return grid;
    }

    int getRows() const {
        return rows;
    }

    int getCols() const {
        return cols;
    }
};

#endif