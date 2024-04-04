#ifndef TAGMANAGER_H
#define TAGMANAGER_H

#include "GameField.h" 
#include "Agent.h"
#include "FlagManager.h"
#include <vector>

class TagManager {
public:
    TagManager(GameField& gameField, FlagManager& flagManager);
    void checkTags(const std::vector<Agent*>& agents);

private:
    GameField& gameField;
    FlagManager& flagManager;
    bool canTag(const Agent& tagger, const Agent& target) const;
    void applyTag(Agent& tagger, Agent& target);

    static constexpr double TAG_RANGE = 10.0;
    static constexpr double TAG_COOLDOWN = 30.0;
};

#endif