#ifndef GAMEFIELD_H
#define GAMEFIELD_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QTimer>
#include <QPointer>
#include "Agent.h"
#include "GameManager.h"
#include "Pathfinder.h"

class GameField : public QGraphicsView {
    Q_OBJECT

public:
    GameField(QWidget* parent, int width, int height);
    ~GameField();

    // Getter functions
    std::shared_ptr<GameManager> getGameManager() const { return gameManager; }
    std::shared_ptr<Pathfinder> getPathfinder() const { return pathfinder; }
    int getTaggingDistance() const { return taggingDistance; }
    QGraphicsScene* getScene() const { return scene; }

    void clearAgents();
    void setupAgents(int blueCount, int redCount, int cols, int rows, const std::shared_ptr<GameManager>& gameManager);
    void runTestCase1();
    void runTestCase2(int agentCount, const std::shared_ptr<GameManager>& gameManager);
    void runTestCase3();
    void handleFlagCapture(const QString& side, const std::shared_ptr<GameManager>& gameManager);
    void updateAgents(int elapsedTime);

private slots:
    void updateAgentItemPositions(QGraphicsItem* item, const std::shared_ptr<Agent>& agent);
    std::vector<std::pair<int, int>> getAgentPositions(const std::vector<std::shared_ptr<Agent>>& agents) const;
    void handleGameTimerTimeout();

private:
    void setupScene();
    QGraphicsPolygonItem* findFlagItem(const QString& team);

    QGraphicsScene* scene;
    std::vector<std::shared_ptr<Agent>> blueAgents;
    std::vector<std::shared_ptr<Agent>> redAgents;
    int gameFieldWidth;
    int gameFieldHeight;
    std::shared_ptr<GameManager> gameManager;
    std::shared_ptr<Pathfinder> pathfinder;
    int blueScore;
    int redScore;
    int timeRemaining;
    QTimer* gameTimer;
    float taggingDistance;
    QGraphicsTextItem* timeRemainingTextItem;
    QPointer<QGraphicsTextItem> blueScoreTextItem;
    QPointer<QGraphicsTextItem> redScoreTextItem;
    QGraphicsRectItem* gameField;

    void checkTagging();
    QGraphicsItem* getAgentItem(Agent* agent);
    void updateAgentItem(QGraphicsItem* item, const std::vector<std::shared_ptr<Agent>>& agents, QColor color);
    void resetEnemyFlag(const QString& team, const std::shared_ptr<GameManager>& gameManager);
    void updateSceneItems();
    void updateScoreDisplay();
    void updateTimeDisplay();
    void stopGame();
    void declareWinner();
};

#endif