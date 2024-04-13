#ifndef MEMORY_H
#define MEMORY_H

#include <unordered_map>
#include <vector>
#include <chrono>
#include <limits>
#include <tuple>

struct pair_hash {
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2>& p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        return h1 ^ h2;
    }
};

class Memory {
public:
    void updateOpponentInfo(int x, int y, bool hasFlag, int dirX, int dirY);
    bool hasOpponentFlag(int x, int y) const;
    std::pair<int, int> getOpponentDirection(int x, int y) const;
    double getTimeSinceLastSeen(int x, int y) const;
    std::pair<int, int> getLastKnownPosition(int x, int y) const;
    const std::unordered_map<std::pair<int, int>, std::tuple<bool, std::pair<int, int>, std::chrono::system_clock::time_point>, pair_hash>& getOpponentInfo() const;

private:
    std::unordered_map<std::pair<int, int>, std::tuple<bool, std::pair<int, int>, std::chrono::system_clock::time_point>, pair_hash> opponentInfo;
};

#endif
