// Pathfinder.cpp

#include "Pathfinder.h"
#include <unordered_set>
#include <algorithm>

Pathfinder::Pathfinder() {}

std::vector<Position> Pathfinder::findPath(const Position& start, const Position& goal) {
    std::unordered_set<Position, PositionHash> openSet;
    openSet.insert(start);

    std::map<Position, Position> cameFrom;

    std::map<Position, double> gScore;
    gScore[start] = 0;

    std::map<Position, double> fScore;
    fScore[start] = heuristicCostEstimate(start, goal);

    while (!openSet.empty()) {
        Position current = *std::min_element(openSet.begin(), openSet.end(),
            [&](const Position& a, const Position& b) {
                return fScore[a] < fScore[b];
            });

        if (current == goal) {
            return reconstructPath(cameFrom, current);
        }

        openSet.erase(current);

        for (const Position& neighbor : getNeighborPositions(current)) {
            double tentativeGScore = gScore[current] + 1; // Assuming a constant cost of 1 for each move

            if (gScore.find(neighbor) == gScore.end() || tentativeGScore < gScore[neighbor]) {
                cameFrom[neighbor] = current;
                gScore[neighbor] = tentativeGScore;
                fScore[neighbor] = gScore[neighbor] + heuristicCostEstimate(neighbor, goal);

                if (openSet.find(neighbor) == openSet.end()) {
                    openSet.insert(neighbor);
                }
            }
        }
    }

    // If no path is found, return an empty vector
    return std::vector<Position>();
}

std::vector<Position> Pathfinder::reconstructPath(std::map<Position, Position>& cameFrom, Position current) {
    std::vector<Position> totalPath = { current };

    while (cameFrom.find(current) != cameFrom.end()) {
        current = cameFrom[current];
        totalPath.insert(totalPath.begin(), current);
    }

    return totalPath;
}

double Pathfinder::heuristicCostEstimate(const Position& start, const Position& goal) {
    // Manhattan distance heuristic
    return std::abs(start.x - goal.x) + std::abs(start.y - goal.y);
}

std::vector<Position> Pathfinder::getNeighborPositions(const Position& current) {
    std::vector<Position> neighbors;

    // Define the possible moves (up, down, left, right)
    std::vector<std::pair<int, int>> moves = { {0, 1}, {0, -1}, {1, 0}, {-1, 0} };

    for (const auto& move : moves) {
        Position neighbor(current.x + move.first, current.y + move.second);
        // Check if the neighbor position is valid (within the game field)
        if (isValidPosition(neighbor)) {
            neighbors.push_back(neighbor);
        }
    }

    return neighbors;
}

bool Pathfinder::isValidPosition(const Position& position) {
    // Check if the position is within the blue team area
    if (position.x >= 5 && position.x <= 405 && position.y >= 10 && position.y <= 590) {
        return true;
    }

    // Check if the position is within the red team area
    if (position.x >= 410 && position.x <= 800 && position.y >= 10 && position.y <= 590) {
        return true;
    }

    return false;
}