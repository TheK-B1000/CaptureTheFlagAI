#ifndef AGENT_H
#define AGENT_H

#include <QObject>
#include <QPoint>

class Agent : public QObject
{
    Q_OBJECT

public:
    explicit Agent(int id, QObject* parent = nullptr);

    void move(const QPoint& newPosition);
    void attemptTag(int targetId);
    void attemptFlagGrab();

    int getId() const;
    QPoint getPosition() const;
    bool isTagged() const;
    void setTagged(bool tagged);
    int getX() const;
    int getY() const;


signals:
    void moved(int agentId, const QPoint& newPosition);
    void tagAttempted(int agentId, int targetId);
    void flagGrabAttempted(int agentId);

private:
    int id;
    QPoint position;
    bool tagged;
};

#endif 


