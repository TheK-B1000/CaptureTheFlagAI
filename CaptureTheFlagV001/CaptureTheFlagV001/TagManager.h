#ifndef TAGMANAGER_H
#define TAGMANAGER_H

#include <QObject>
#include <QPoint>

class TagManager : public QObject
{
    Q_OBJECT

public:
    explicit TagManager(QObject* parent = nullptr);

    bool checkTagRequest(int taggerId, int targetId, const QPoint& taggerPosition, const QPoint& targetPosition);

signals:
    void agentTagged(int agentId);

private:
    int cooldown;
};

#endif 

