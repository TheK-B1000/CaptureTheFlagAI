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

// Define a method to find a path from a start position to a goal position.
std::vector<std::pair<int, int>> Pathfinder::findPath(int startX, int startY, int goalX, int goalY) {
    // Declare a priority queue that will store pairs of heuristic scores and positions, sorted by the score in ascending order.
    std::priority_queue<std::pair<double, std::pair<int, int>>,
        std::vector<std::pair<double, std::pair<int, int>>>,
        std::greater<std::pair<double, std::pair<int, int>>>> openSet;

    // A hash map to store the cost of the cheapest path from start to a position.
    std::unordered_map<std::pair<int, int>, double, pair_hash> gScore;

    // A hash map to store the parent node of each visited position to reconstruct the path later.
    std::unordered_map<std::pair<int, int>, std::pair<int, int>, pair_hash> cameFrom;

    // Add the starting position to the open set with a heuristic score of 0.
    openSet.push({ 0.0, {startX, startY} });

    // Set the gScore for the starting position to 0.
    gScore[{startX, startY}] = 0.0;

    // Log the start of pathfinding, with the start and goal coordinates.
    qDebug() << "Pathfinder: Starting pathfinding from" << startX << startY << "to" << goalX << goalY;

    // Continue the search until there are no more positions to explore.
    while (!openSet.empty()) {
        // Get the position from the open set with the lowest f-score (g-score + heuristic).
        std::pair<int, int> current = openSet.top().second;

        // Remove this position from the open set.
        openSet.pop();

        // Check if the current position is the goal.
        if (current.first == goalX && current.second == goalY) {
            // Log that the goal has been reached.
            qDebug() << "Pathfinder: Goal reached!";

            // Create a vector to store the path from start to goal.
            std::vector<std::pair<int, int>> path;

            // Trace back from goal to start using the 'cameFrom' map.
            while (current != std::make_pair(startX, startY)) {
                // Add the current position to the path.
                path.push_back(current);
                // Move to the next position.
                current = cameFrom[current];
            }
            // Reverse the path to get the correct order from start to goal.
            std::reverse(path.begin(), path.end());

            // Log the generated path.
            qDebug() << "Pathfinder: Generated path:";
            for (const auto& point : path) {
                qDebug() << point.first << point.second;
            }

            // Return the generated path.
            return path;
        }

        // Explore all neighbors of the current position.
        for (const auto& neighbor : getNeighbors(current.first, current.second)) {
            // Calculate a tentative g-score for the neighbor.
            double tentativeGScore = gScore[current] + 1;

            // If this neighbor has not been visited or we found a cheaper path to it, update its score and parent.
            if (gScore.find(neighbor) == gScore.end() || tentativeGScore < gScore[neighbor]) {
                // Set the current position as the parent of the neighbor.
                cameFrom[neighbor] = current;
                // Update the g-score of the neighbor.
                gScore[neighbor] = tentativeGScore;
                // Calculate the f-score of the neighbor (tentative g-score + heuristic).
                double fScore = tentativeGScore + calculateHeuristic(neighbor.first, neighbor.second, goalX, goalY);

                // Add the neighbor to the open set with its f-score.
                openSet.push({ fScore, neighbor });
            }
        }
    }

    // Log that no path was found if the open set is empty.
    qDebug() << "Pathfinder: No path found!";

    // Return an empty path, indicating failure to find a path.
    return std::vector<std::pair<int, int>>();
}

std::vector<std::pair<int, int>> Pathfinder::getNeighbors(int x, int y) {
    std::vector<std::pair<int, int>> neighbors;
    const int dx[] = { -1, 1, 0, 0 };
    const int dy[] = { 0, 0, -1, 1 };

    for (int i = 0; i < 4; ++i) {
        int newX = x + dx[i];
        int newY = y + dy[i];

        if (newX >= 0 && newX < cols && newY >= 0 && newY < rows) {
            // Check if the neighbor position is not occupied by another AI agent
            if (std::find(dynamicObstacles.begin(), dynamicObstacles.end(), std::make_pair(newX, newY)) == dynamicObstacles.end()) {
                neighbors.push_back({ newX, newY });
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