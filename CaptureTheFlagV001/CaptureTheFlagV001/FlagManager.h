#ifndef FLAGMANAGER_H
#define FLAGMANAGER_H

#include <vector>

class Agent;

class FlagManager {
public:
    static bool grabFlag(Agent* agent);
    static bool captureFlag(Agent* agent);
    static void resetFlag(Agent* agent);
};

#endif