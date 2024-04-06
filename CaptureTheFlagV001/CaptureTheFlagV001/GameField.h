#ifndef GAMEFIELD_H
#define GAMEFIELD_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsTextItem>
#include <QTimer>
#include <vector>
#include <QPointF>
#include "Agent.h"
#include "Pathfinder.h"
#include "TagManager.h"
#include "FlagManager.h"

class GameManager;

class GameField : public QGraphicsView {
    Q_OBJECT

public:
    explicit GameField(QWidget* parent = nullptr);
    virtual ~GameField();

    double getDistance(const QPointF& point1, const QPointF& point2) const;
    bool isInHomeZone(const QPointF& position, int teamId) const;
    std::vector<Agent*> getTeamAgents(int teamId) const;
    bool isWithinBounds(const QPoint& position) const;
    const std::vector<Agent*>& getBlueAgents() const;
    const std::vector<Agent*>& getRedAgents() const;

private slots:
    void updateAgents();
    void handleGameTimerTimeout();
    void handleFlagCapture(const QString& team);

private:
    QGraphicsScene* scene;
    Pathfinder* pathfinder;
    int cellSize;
    QGraphicsTextItem* blueScoreTextItem;
    QGraphicsTextItem* redScoreTextItem;
    TagManager* tagManager;
    FlagManager* flagManager;
    GameManager* gameManager; 
    QPointF getHomeZoneCenter(int teamId) const;
    qreal getHomeZoneRadius(int teamId) const;
    void setupScene();
    void stopGame();
};

#endif 