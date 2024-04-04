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

    float flagCaptureScore = evaluateFlagCapture(hasFlag, distanceToFlag);
    float flagRecoveryScore = evaluateFlagRecovery(opponentHasFlag, distanceToNearestEnemy);
    float exploreScore = evaluateExplore(hasFlag, opponentHasFlag, inHomeZone);

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

float Brain::evaluateFlagCapture(bool hasFlag, float distanceToFlag) {
    if (hasFlag) {
        return 1.0f;
    }
    else {
        return 1.0f - (distanceToFlag / 100.0f);
    }
}

float Brain::evaluateFlagRecovery(bool opponentHasFlag, float distanceToNearestEnemy) {
    if (opponentHasFlag) {
        return 1.0f - (distanceToNearestEnemy / 100.0f);
    }
    else {
        return 0.0f;
    }
}

float Brain::evaluateExplore(bool hasFlag, bool opponentHasFlag, bool inHomeZone) {
    if (hasFlag || opponentHasFlag || inHomeZone) {
        return 0.0f;
    }
    else {
        return 0.5f;
    }
}