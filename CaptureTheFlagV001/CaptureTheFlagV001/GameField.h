#ifndef GAMEFIELD_H
#define GAMEFIELD_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QTimer>
#include "Agent.h"
#include "Pathfinder.h"
#include "TagManager.h"
#include "FlagManager.h"
#include "GameManager.h"

class GameField : public QGraphicsView {
    Q_OBJECT

public:
    GameField(QWidget* parent = nullptr);
    ~GameField();

    static double getDistance(const QPointF& point1, const QPointF& point2);
    bool isInHomeZone(const QPointF& position, int teamId) const;
    static double getDistance(const Position& point1, const Position& point2);
    bool isInHomeZone(const Position& position, int teamId) const;

    std::vector<Agent*> getTeamAgents(int teamId) const;

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
    Position getHomeZoneCenter(int teamId) const;
    qreal getHomeZoneRadius(int teamId) const;

    void setupScene();
    void stopGame();
};

#endif