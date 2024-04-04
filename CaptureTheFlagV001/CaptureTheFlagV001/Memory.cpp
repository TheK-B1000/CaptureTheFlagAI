#include "Memory.h"

void Memory::updateOpponentInfo(int opponentX, int opponentY, bool hasFlag, std::pair<int, int> direction) {
    opponentInfo[std::make_pair(opponentX, opponentY)] = std::make_tuple(hasFlag, direction, std::chrono::system_clock::now());
}

std::pair<bool, std::pair<int, int>> Memory::getOpponentInfo(int opponentX, int opponentY) const {
    auto it = opponentInfo.find(std::make_pair(opponentX, opponentY));
    if (it != opponentInfo.end()) {
        return std::make_pair(std::get<0>(it->second), std::get<1>(it->second));
    }
    return std::make_pair(false, std::make_pair(0, 0));
}

std::chrono::duration<double> Memory::getTimeSinceLastSeen(int opponentX, int opponentY) {
    auto it = opponentInfo.find(std::make_pair(opponentX, opponentY));
    if (it != opponentInfo.end()) {
        return std::chrono::system_clock::now() - std::get<2>(it->second);
    }
    return std::chrono::duration<double>::max();
}

std::pair<int, int> Memory::getLastKnownDirection(int opponentX, int opponentY) {
    auto it = opponentInfo.find(std::make_pair(opponentX, opponentY));
    if (it != opponentInfo.end()) {
        return std::get<1>(it->second);
    }
    return std::make_pair(0, 0);
}

std::pair<int, int> Memory::getLastKnownPosition(int opponentX, int opponentY) {
    return std::make_pair(opponentX, opponentY);
}

const std::unordered_map<std::pair<int, int>, std::tuple<bool, std::pair<int, int>, std::chrono::system_clock::time_point>, pair_hash>& Memory::getOpponentInfo() const {
    return opponentInfo;
}