#include "Agent.h"

Agent::Agent(int id, QObject* parent)
    : QObject(parent), id(id), tagged(false)
{
}

void Agent::move(const QPoint& newPosition)
{
    position = newPosition;
    emit moved(id, newPosition);
}

void Agent::attemptTag(int targetId)
{
    emit tagAttempted(id, targetId);
}

void Agent::attemptFlagGrab()
{
    emit flagGrabAttempted(id);
}

int Agent::getId() const
{
    return id;
}

QPoint Agent::getPosition() const
{
    return position;
}

bool Agent::isTagged() const
{
    return tagged;
}

void Agent::setTagged(bool tagged)
{
    this->tagged = tagged;
}
