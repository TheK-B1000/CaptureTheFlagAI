#ifndef FLAGMANAGER_H
#define FLAGMANAGER_H

#include <QObject>
#include <QPoint>

class FlagManager : public QObject
{
    Q_OBJECT

public:
    explicit FlagManager(QObject* parent = nullptr);

    bool checkFlagGrabRequest(int agentId, const QPoint& requestorPosition, bool isRequestorTagged);
    void returnFlag(bool isBlueTeam);

signals:
    void flagGrabbed(int agentId, bool isBlueTeam);
    void flagReturned(bool isBlueTeam);

private:
    QPoint blueFlagPosition;
    QPoint redFlagPosition;
    bool blueFlagGrabbed;
    bool redFlagGrabbed;
};

#endif 

