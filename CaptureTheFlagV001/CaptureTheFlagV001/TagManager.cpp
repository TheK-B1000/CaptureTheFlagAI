#include "TagManager.h"

TagManager::TagManager(GameField& gameField, FlagManager& flagManager)
    : gameField(gameField), flagManager(flagManager) {}

void TagManager::checkTags(const std::vector<Agent*>& agents) {
    for (auto& tagger : agents) {
        if (tagger->isTagged() || tagger->isOnCooldown()) {
            continue;
        }

        Agent* closestTarget = nullptr;
        double minDistance = std::numeric_limits<double>::max();

        for (auto& target : agents) {
            if (tagger->getTeam() != target->getTeam() && canTag(*tagger, *target)) {
                QPointF taggerPos = QPointF(tagger->getPosition().x, tagger->getPosition().y);
                QPointF targetPos = QPointF(target->getPosition().x, target->getPosition().y);
                double distance = gameField.getDistance(taggerPos, targetPos);

                if (distance < minDistance) {
                    minDistance = distance;
                    closestTarget = target;
                }
            }
        }

        if (closestTarget != nullptr) {
            applyTag(*tagger, *closestTarget);
        }
    }
}

bool TagManager::canTag(const Agent& tagger, const Agent& target) const {
    if (!gameField.isInHomeZone(QPointF(tagger.getPosition().x, tagger.getPosition().y), tagger.getTeam()) ||
        gameField.isInHomeZone(QPointF(target.getPosition().x, target.getPosition().y), target.getTeam())) {
        return false;
    }

    // Check if the distance between the tagger and target is within the tagging range
    double distance = gameField.getDistance(QPointF(tagger.getPosition().x, tagger.getPosition().y),
        QPointF(target.getPosition().x, target.getPosition().y));
    if (distance > TAG_RANGE) {
        return false;
    }

    // All conditions for tagging are met
    return true;
}


void TagManager::applyTag(Agent& tagger, Agent& target) {
    target.setTagged(true); // Mark the target as tagged
    tagger.startCooldown(TAG_COOLDOWN); // Start the cooldown for the tagger
}