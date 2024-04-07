#ifndef GAMEFIELD_H
#define GAMEFIELD_H

#include <QGraphicsView>
#include <QPointF>
#include <vector>

class Pathfinder;
class TagManager;
class FlagManager;
class GameManager;
class Agent;
class QGraphicsTextItem;

class GameField : public QGraphicsView {
    Q_OBJECT

public:
    explicit GameField(QWidget* parent = nullptr);
    ~GameField();

    double getDistance(const QPointF& point1, const QPointF& point2) const;
    bool isInHomeZone(const QPointF& position, int teamId) const;
    bool isInEnemyFlagZone(const QPoint& position, int teamId) const;
    bool canTag(const Agent* tagger, const Agent* target) const;

    std::vector<Agent*> getTeamAgents(int teamId) const;
    std::vector<Agent*> getOpponentAgents(const Agent* agent) const;

    QPointF getHomeZoneCenter(int teamId) const;
    qreal getHomeZoneRadius(int teamId) const;

    bool isWithinBounds(const QPoint& position) const;

    const std::vector<Agent*>& getBlueAgents() const;
    const std::vector<Agent*>& getRedAgents() const;

public slots:
    void handleGameTimerTimeout();
    void handleFlagGrabAttempt(int agentId);
    void handleTagAttempt(int taggerId, int targetId);
    void handleFlagCapture(int agentId, bool isBlueTeam);
    void handleFlagReturned(bool isBlueTeam);
    void handleAgentTagged(int agentId);

private:
    void setupScene();
    void updateAgents();
    void stopGame();
    Agent* findAgentById(int agentId);

    Pathfinder* pathfinder;
    QGraphicsScene* scene;
    int cellSize;
    QGraphicsTextItem* blueScoreTextItem;
    QGraphicsTextItem* redScoreTextItem;
    TagManager* tagManager;
    FlagManager* flagManager;
    GameManager* gameManager;

    std::vector<Agent*> blueAgents;
    std::vector<Agent*> redAgents;
};

#endif