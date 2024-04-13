#include "Memory.h"

void Memory::updateOpponentInfo(int x, int y, bool hasFlag, int dirX, int dirY) {
    opponentInfo[std::make_pair(x, y)] = { hasFlag, std::make_pair(dirX, dirY), std::chrono::system_clock::now() };
}

bool Memory::hasOpponentFlag(int x, int y) const {
    auto it = opponentInfo.find(std::make_pair(x, y));
    return it != opponentInfo.end() && std::get<0>(it->second);
}

std::pair<int, int> Memory::getOpponentDirection(int x, int y) const {
    auto it = opponentInfo.find(std::make_pair(x, y));
    return it != opponentInfo.end() ? std::get<1>(it->second) : std::make_pair(0, 0);
}

double Memory::getTimeSinceLastSeen(int x, int y) const {
    auto it = opponentInfo.find(std::make_pair(x, y));
    return it != opponentInfo.end() ? std::chrono::duration<double>(std::chrono::system_clock::now() - std::get<2>(it->second)).count() : std::numeric_limits<double>::max();
}

std::pair<int, int> Memory::getLastKnownPosition(int x, int y) const {
    return std::make_pair(x, y);
}

const std::unordered_map<std::pair<int, int>, std::tuple<bool, std::pair<int, int>, std::chrono::system_clock::time_point>, pair_hash>& Memory::getOpponentInfo() const {
    return opponentInfo;
}