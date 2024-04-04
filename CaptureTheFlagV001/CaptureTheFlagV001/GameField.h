#ifndef GAMEFIELD_H
#define GAMEFIELD_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QTimer>
#include "Agent.h"
#include "Pathfinder.h"

class GameField : public QGraphicsView {
    Q_OBJECT

public:
    GameField(QWidget* parent = nullptr);
    ~GameField();

private slots:
    void updateAgents();
    void handleGameTimerTimeout();
    void handleFlagCapture(const QString& team);

private:
    QGraphicsScene* scene;
    std::vector<Agent*> blueAgents;
    std::vector<Agent*> redAgents;
    Pathfinder* pathfinder;
    int cellSize;
    int blueScore;
    int redScore;
    int timeRemaining;
    QTimer* gameTimer;
    float taggingDistance;
    QGraphicsTextItem* timeRemainingTextItem;
    QGraphicsTextItem* blueScoreTextItem;
    QGraphicsTextItem* redScoreTextItem;

    void updateAgentPositions();
    void updateAgentItemsPositions();
    void checkTagging();
    QGraphicsItem* getAgentItem(Agent* agent);
    void updateAgentItem(QGraphicsItem* item, const std::vector<Agent*>& agents, QColor color);
    void updateSceneItems();
    void resetEnemyFlag(const QString& team);
    QGraphicsEllipseItem* findFlagItem(const QString& team);
    void updateScoreDisplay();
    void updateTimeDisplay();
    void setupScene();
    void stopGame();
    void declareWinner();
};

#endif