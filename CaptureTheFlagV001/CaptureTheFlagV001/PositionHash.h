#ifndef POSITION_HASH_H
#define POSITION_HASH_H

#include "Position.h"
#include <functional>

struct PositionHash {
    std::size_t operator()(const Position& position) const {
        return std::hash<int>()(position.x) ^ std::hash<int>()(position.y);
    }
};

#endif 
