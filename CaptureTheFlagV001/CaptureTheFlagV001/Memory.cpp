#include "Memory.h"
#include <chrono>

void Memory::updateOpponentInfo(int opponentX, int opponentY, bool hasFlag, std::pair<int, int> direction) {
    opponentInfo[std::make_pair(opponentX, opponentY)] = std::make_tuple(hasFlag, direction, std::chrono::system_clock::now());
}

std::tuple<bool, std::pair<int, int>, std::chrono::system_clock::time_point> Memory::getOpponentInfo(int opponentX, int opponentY) const {
    auto it = opponentInfo.find(std::make_pair(opponentX, opponentY));
    if (it != opponentInfo.end()) {
        return it->second;
    }
    return std::make_tuple(false, std::make_pair(0, 0), std::chrono::system_clock::time_point());
}

bool Memory::isAnyOpponentCarryingFlag() const {
    for (const auto& entry : opponentInfo) {
        if (std::get<0>(entry.second)) {
            return true;
        }
    }
    return false;
}

std::pair<int, int> Memory::getOpponentWithFlag() const {
    for (const auto& entry : opponentInfo) {
        if (std::get<0>(entry.second)) {
            return entry.first;
        }
    }
    return std::make_pair(-1, -1);
}

std::pair<int, int> Memory::getPredictedOpponentPosition(std::pair<int, int> opponentPosition) const {
    auto it = opponentInfo.find(opponentPosition);
    if (it != opponentInfo.end()) {
        std::pair<int, int> direction = std::get<1>(it->second);
        return std::make_pair(opponentPosition.first + direction.first, opponentPosition.second + direction.second);
    }
    return opponentPosition;
}