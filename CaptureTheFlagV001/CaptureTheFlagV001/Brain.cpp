#include "Brain.h"

Brain::Brain() : flagCaptured(false), score(0), proximityThreshold(10.0f) {}

BrainDecision Brain::makeDecision(bool hasFlag, bool opponentHasFlag, bool isTagged, bool inHomeZone, float distanceToFlag, float distanceToNearestEnemy) {
    if (isTagged) {
        return BrainDecision::ReturnToHomeZone;
    }

    if (hasFlag && inHomeZone) {
        if (!flagCaptured) {
            flagCaptured = true;
            score++;
        }
        return BrainDecision::CaptureFlag;
    }

    if (distanceToFlag <= proximityThreshold && !hasFlag && !opponentHasFlag) {
        return BrainDecision::GrabFlag;
    }

    if (distanceToNearestEnemy <= proximityThreshold && !isTagged && opponentHasFlag) {
        return BrainDecision::TagEnemy;
    }

    float flagCaptureScore = hasFlag ? 1.0f : 1.0f - (distanceToFlag / 100.0f);
    float flagRecoveryScore = opponentHasFlag ? 1.0f - (distanceToNearestEnemy / 100.0f) : 0.0f;
    float exploreScore = hasFlag || opponentHasFlag || inHomeZone ? 0.0f : 0.5f;

    if (flagCaptureScore >= flagRecoveryScore && flagCaptureScore >= exploreScore) {
        return BrainDecision::CaptureFlag;
    }
    else if (flagRecoveryScore >= flagCaptureScore && flagRecoveryScore >= exploreScore) {
        return BrainDecision::RecoverFlag;
    }
    else {
        return BrainDecision::Explore;
    }
}