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
    GameField(QWidget* parent, const std::vector<std::vector<int>>& grid);
    ~GameField();

    // Getter functions
    const std::vector<std::shared_ptr<Agent>>& getBlueAgents() const;
    const std::vector<std::shared_ptr<Agent>>& getRedAgents() const;
    GameManager* getGameManager() const { return gameManager; }
    int getCols() const { return cols; }
    const std::vector<std::vector<int>>& getGrid() const { return grid; }
    int getRows() const { return rows; }
    Pathfinder* getPathfinder() const { return pathfinder; }
    int getTaggingDistance() const { return taggingDistance; }
    QGraphicsScene* getScene() const { return scene; }

    void clearAgents();
    void setupAgents(int blueCount, int redCount, int cols, int rows, GameManager* gameManager);
    bool isValidPosition(int x, int y);

    void runTestCase1();
    void runTestCase2(int agentCount, GameManager* gameManager);
    void runTestCase3();

private slots:
    void updateAgents();
    std::vector<std::pair<int, int>> getAgentPositions(const std::vector<std::shared_ptr<Agent>>& agents) const;
    void handleGameTimerTimeout();
    void handleFlagCapture(const QString& team);

private:
    void setupScene();
    QGraphicsPolygonItem* findFlagItem(const QString& team);
    void updateGridFromZones(QGraphicsEllipseItem* blueZone, QGraphicsEllipseItem* redZone);

    QGraphicsScene* scene;
    std::vector<std::shared_ptr<Agent>> blueAgents;
    std::vector<std::shared_ptr<Agent>> redAgents;
    int gameFieldWidth;
    int gameFieldHeight;
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
    int cols;
    std::vector<std::vector<int>> grid;
    std::pair<int, int> pixelToGrid(int pixelX, int pixelY);
    int rows;
    GameManager* gameManager;
    QGraphicsRectItem* gameField;

    void updateAgentPositions();
    void updateAgentItemPositions(QGraphicsItem* item, const std::shared_ptr<Agent>& agent, int x, int y);
    void checkTagging();
    QGraphicsItem* getAgentItem(Agent* agent);
    void updateAgentItem(QGraphicsItem* item, const std::vector<std::shared_ptr<Agent>>& agents, QColor color);
    void updateSceneItems();
    void resetEnemyFlag(const QString& team);
    void updateScoreDisplay();
    void updateTimeDisplay();
    void stopGame();
    void declareWinner();

};

#endif