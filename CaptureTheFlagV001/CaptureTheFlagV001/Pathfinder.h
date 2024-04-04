// Pathfinder.h

#ifndef PATHFINDER_H
#define PATHFINDER_H

#include <vector>
#include <map>
#include "Position.h"
#include "PositionHash.h"

class Pathfinder {
public:
    Pathfinder();

    std::vector<Position> findPath(const Position& start, const Position& goal);

private:
    std::vector<Position> reconstructPath(std::map<Position, Position>& cameFrom, Position current);
    double heuristicCostEstimate(const Position& start, const Position& goal);
    std::vector<Position> getNeighborPositions(const Position& current);
    bool isValidPosition(const Position& position);
};

#endif 