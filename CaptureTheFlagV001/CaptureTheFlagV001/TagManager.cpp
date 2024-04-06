#include "TagManager.h"
#include <QVector2D>

TagManager::TagManager(QObject* parent)
    : QObject(parent), cooldown(100)
{
}

bool TagManager::checkTagRequest(int taggerId, int targetId, const QPoint& taggerPosition, const QPoint& targetPosition)
{
    if (QVector2D(taggerPosition - targetPosition).length() <= 20) {
        emit agentTagged(targetId);
        return true;
    }
    return false;
}