#include "FlagManager.h"
#include <QVector2D>

FlagManager::FlagManager(QObject* parent)
    : QObject(parent), blueFlagGrabbed(false), redFlagGrabbed(false)
{
    blueFlagPosition = QPoint(100, 100);
    redFlagPosition = QPoint(700, 500);
}

bool FlagManager::checkFlagGrabRequest(int agentId, const QPoint& requestorPosition, bool isRequestorTagged)
{
    if (isRequestorTagged)
        return false;

    if (QVector2D(requestorPosition - blueFlagPosition).length() <= 50 && !blueFlagGrabbed) {
        blueFlagGrabbed = true;
        emit flagGrabbed(agentId, true);
        return true;
    }
    else if (QVector2D(requestorPosition - redFlagPosition).length() <= 50 && !redFlagGrabbed) {
        redFlagGrabbed = true;
        emit flagGrabbed(agentId, false);
        return true;
    }

    return false;
}

void FlagManager::returnFlag(bool isBlueTeam)
{
    if (isBlueTeam) {
        blueFlagGrabbed = false;
        emit flagReturned(true);
    }
    else {
        redFlagGrabbed = false;
        emit flagReturned(false);
    }
}