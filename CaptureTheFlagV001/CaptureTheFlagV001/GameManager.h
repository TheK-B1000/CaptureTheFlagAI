#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QObject>
#include <QTimer>
#include <QGraphicsTextItem>
#include "Agent.h"

class GameField;

class GameManager : public QObject {
    Q_OBJECT

public:
    GameManager(GameField* gameField);

public slots:
    void handleGameTimerTimeout();

private:
    GameField* gameField;
    QTimer* gameTimer;
    int timeRemaining;
    int blueScore;
    int redScore;
    QGraphicsTextItem* timeRemainingTextItem;
    QGraphicsTextItem* blueScoreTextItem;
    QGraphicsTextItem* redScoreTextItem;

    void updateAgents();
};

#endif