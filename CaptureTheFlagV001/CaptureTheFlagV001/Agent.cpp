#include "Agent.h"

Agent::Agent(int id, QObject* parent)
    : QObject(parent), id(id), tagged(false) {
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

int Agent::getX() const {
    return position.x();
}

int Agent::getY() const {
    return position.y();
}

int Agent::getId() const
{
    return id;
}

QPoint Agent::getPosition() const
{
    return position;
}

void Agent::setPosition(const QPoint& position)
{
    this->position = position;
}

bool Agent::isTagged() const
{
    return tagged;
}

void Agent::setTagged(bool tagged)
{
    this->tagged = tagged;
}
