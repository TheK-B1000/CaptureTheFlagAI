#include "Pathfinder.h"
#include "GameField.h"
#include "Agent.h"
#include <queue>
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <iostream>
#include <QDebug>
#include <qlogging.h>

Pathfinder::Pathfinder(const std::vector<std::vector<int>>& grid) : grid(grid) {
    rows = grid.size();
    cols = grid[0].size();
}

void Pathfinder::setDynamicObstacles(const std::vector<std::pair<int, int>>& obstacles) {
    dynamicObstacles = obstacles;
}

double Pathfinder::calculateHeuristic(int x1, int y1, int x2, int y2) {
    int dx = std::abs(x1 - x2);
    int dy = std::abs(y1 - y2);
    return static_cast<double>(dx + dy);
}

std::vector<std::pair<int, int>> Pathfinder::findPath(int startX, int startY, int goalX, int goalY) {
    std::priority_queue<std::pair<double, std::pair<int, int>>, std::vector<std::pair<double, std::pair<int, int>>>, std::greater<std::pair<double, std::pair<int, int>>>> openSet;
    std::unordered_map<std::pair<int, int>, double, pair_hash> gScore;
    std::unordered_map<std::pair<int, int>, std::pair<int, int>, pair_hash> cameFrom;

    openSet.push({ 0.0, {startX, startY} });
    gScore[{startX, startY}] = 0.0;

    qDebug() << "Pathfinder: Starting pathfinding from" << startX << startY << "to" << goalX << goalY;

    while (!openSet.empty()) {
        std::pair<int, int> current = openSet.top().second;
        openSet.pop();

        if (current.first == goalX && current.second == goalY) {
            qDebug() << "Pathfinder: Goal reached!";

            std::vector<std::pair<int, int>> path;
            while (current != std::make_pair(startX, startY)) {
                path.push_back(current);
                current = cameFrom[current];
            }
            std::reverse(path.begin(), path.end());

            qDebug() << "Pathfinder: Generated path:";
            for (const auto& point : path) {
                qDebug() << point.first << point.second;
            }

            return path;
        }

        for (const auto& neighbor : getNeighbors(current.first, current.second)) {
            double tentativeGScore = gScore[current] + 1;

            if (gScore.find(neighbor) == gScore.end() || tentativeGScore < gScore[neighbor]) {
                cameFrom[neighbor] = current;
                gScore[neighbor] = tentativeGScore;
                double fScore = tentativeGScore + calculateHeuristic(neighbor.first, neighbor.second, goalX, goalY);

                openSet.push({ fScore, neighbor });
            }
        }
    }

    qDebug() << "Pathfinder: No path found!";
    return std::vector<std::pair<int, int>>();
}

std::vector<std::pair<int, int>> Pathfinder::getNeighbors(int x, int y) {
    std::vector<std::pair<int, int>> neighbors;
    const int dx[] = { -1, 1, 0, 0 };
    const int dy[] = { 0, 0, -1, 1 };
    GameManager gameManager(cols, rows);

    for (int i = 0; i < 4; ++i) {
        int newX = x + dx[i];
        int newY = y + dy[i];

        if (newX >= 0 && newX < cols && newY >= 0 && newY < rows) {
            // Check if the neighbor position is not occupied by another AI agent
            if (std::find(dynamicObstacles.begin(), dynamicObstacles.end(), std::make_pair(newX, newY)) == dynamicObstacles.end()) {
                // Check if the neighbor position is not a flag or a team zone
                if (!gameManager.isFlag(newX, newY) && !gameManager.isTeamZone(newX, newY)) {
                    neighbors.push_back({ newX, newY });
                }
            }
        }
    }

    return neighbors;
}

std::pair<int, int> Pathfinder::getRandomFreePosition() {
    std::vector<std::pair<int, int>> freePositions;
    for (int x = 0; x < cols; ++x) {
        for (int y = 0; y < rows; ++y) {
            if (grid[y][x] != 1 && std::find(dynamicObstacles.begin(), dynamicObstacles.end(), std::make_pair(x, y)) == dynamicObstacles.end()) {
                freePositions.push_back({ x, y });
            }
        }
    }
    if (!freePositions.empty()) {
        int randomIndex = std::rand() % freePositions.size();
        return freePositions[randomIndex];
    }
    return { -1, -1 };
}