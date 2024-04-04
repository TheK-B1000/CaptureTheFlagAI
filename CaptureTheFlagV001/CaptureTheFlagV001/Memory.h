#ifndef MEMORY_H
#define MEMORY_H

#include <vector>
#include <unordered_map>
#include <chrono>

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
    void updateOpponentInfo(int opponentX, int opponentY, bool hasFlag, std::pair<int, int> direction);
    std::pair<bool, std::pair<int, int>> getOpponentInfo(int opponentX, int opponentY) const;
    std::chrono::duration<double> getTimeSinceLastSeen(int opponentX, int opponentY);
    std::pair<int, int> getLastKnownDirection(int opponentX, int opponentY);
    std::pair<int, int> getLastKnownPosition(int opponentX, int opponentY);
    const std::unordered_map<std::pair<int, int>, std::tuple<bool, std::pair<int, int>, std::chrono::system_clock::time_point>, pair_hash>& getOpponentInfo() const;

private:
    std::unordered_map<std::pair<int, int>, std::tuple<bool, std::pair<int, int>, std::chrono::system_clock::time_point>, pair_hash> opponentInfo;
};

#endif