#ifndef PATHFINDER_H
#define PATHFINDER_H

#include <QObject>
#include <QPoint>
#include <QList>

class GameField;

class Pathfinder : public QObject
{
    Q_OBJECT

public:
    explicit Pathfinder(GameField& gameField, QObject* parent = nullptr);
    QList<QPoint> findPath(const QPoint& start, const QPoint& end);

private:
    QList<QPoint> getNeighbors(const QPoint& position);
    bool isValidPosition(const QPoint& position);
    bool isObstacle(const QPoint& position) const;
    int heuristic(const QPoint& position, const QPoint& end);

    GameField& gameField;
};

#endif 