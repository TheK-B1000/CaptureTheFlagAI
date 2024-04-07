#include "GameField.h"
#include <QPainter>
#include <QPolygon>
#include <QRandomGenerator>
#include <QTimer>
#include <QGraphicsItem>
#include <QGraphicsTextItem>
#include <QFont>
#include "Pathfinder.h"
#include "TagManager.h"
#include "FlagManager.h"
#include "GameManager.h"
#include "Agent.h"

GameField::GameField(QWidget* parent)
    : QGraphicsView(parent),
    pathfinder(new Pathfinder(*this)),
    cellSize(20),
    blueScoreTextItem(nullptr),
    redScoreTextItem(nullptr),
    tagManager(new TagManager(this)),
    flagManager(new FlagManager(this)),
    gameManager(nullptr) {

    std::vector<Agent*> blueAgents;
    std::vector<Agent*> redAgents;

    // Create and add blue agents
    for (int i = 0; i < 5; ++i) {
        Agent* blueAgent = new Agent(i, this);
        blueAgent->setPosition(QPoint(50 + i * 50, 100)); // Set initial position
        connect(blueAgent, &Agent::flagGrabAttempted, this, &GameField::handleFlagGrabAttempt);
        connect(blueAgent, &Agent::tagAttempted, this, &GameField::handleTagAttempt);
        blueAgents.push_back(blueAgent);
    }

    // Create and add red agents
    for (int i = 0; i < 5; ++i) {
        Agent* redAgent = new Agent(i + 5, this);
        redAgent->setPosition(QPoint(50 + i * 50, 500)); // Set initial position
        connect(redAgent, &Agent::flagGrabAttempted, this, &GameField::handleFlagGrabAttempt);
        connect(redAgent, &Agent::tagAttempted, this, &GameField::handleTagAttempt);
        redAgents.push_back(redAgent);
    }

    // Set up the game manager
    gameManager = new GameManager(this, blueAgents, redAgents);

    // Set up the game timer
    QTimer* gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &GameField::handleGameTimerTimeout);
    gameTimer->start(1000); // Update every second (adjust as needed)

    // Set up the scene
    setupScene();

    // Connect signals from FlagManager and TagManager
    connect(flagManager, &FlagManager::flagGrabbed, this, &GameField::handleFlagCapture);
    connect(flagManager, &FlagManager::flagReturned, this, &GameField::handleFlagReturned);
    connect(tagManager, &TagManager::agentTagged, this, &GameField::handleAgentTagged);
}

GameField::~GameField() {
    // Clean up resources
    delete pathfinder;
    delete scene;
    delete tagManager;
    delete flagManager;
    delete gameManager;

    // Delete agents
    for (Agent* agent : blueAgents) {
        delete agent;
    }
    for (Agent* agent : redAgents) {
        delete agent;
    }
}

double GameField::getDistance(const QPointF& point1, const QPointF& point2) const {
    return std::sqrt(std::pow(point1.x() - point2.x(), 2) + std::pow(point1.y() - point2.y(), 2));
}

bool GameField::isInHomeZone(const QPointF& position, int teamId) const {
    QPointF homeCenter = getHomeZoneCenter(teamId);
    qreal homeRadius = getHomeZoneRadius(teamId);
    return getDistance(position, homeCenter) <= homeRadius;
}

bool GameField::isInEnemyFlagZone(const QPoint& position, int teamId) const {
    int enemyTeamId = (teamId == 0) ? 1 : 0;
    QPointF enemyFlagPosition = flagManager->getFlagPosition(enemyTeamId == 0);
    qreal flagZoneRadius = 30; // Adjust the flag zone radius as needed
    return getDistance(position, enemyFlagPosition) <= flagZoneRadius;
}

bool GameField::canTag(const Agent* tagger, const Agent* target) const {
    QPoint taggerPosition = tagger->getPosition();
    QPoint targetPosition = target->getPosition();
    qreal tagDistance = 20; // Adjust the tag distance as needed
    return getDistance(taggerPosition, targetPosition) <= tagDistance;
}

std::vector<Agent*> GameField::getTeamAgents(int teamId) const {
    if (teamId == 0) {
        return blueAgents;
    }
    else if (teamId == 1) {
        return redAgents;
    }
    return std::vector<Agent*>();
}

std::vector<Agent*> GameField::getOpponentAgents(const Agent* agent) const {
    int agentId = agent->getId();
    std::vector<Agent*> opponents;

    for (Agent* blueAgent : blueAgents) {
        if (blueAgent->getId() != agentId) {
            opponents.push_back(blueAgent);
        }
    }

    for (Agent* redAgent : redAgents) {
        if (redAgent->getId() != agentId) {
            opponents.push_back(redAgent);
        }
    }

    return opponents;
}

QPointF GameField::getHomeZoneCenter(int teamId) const {
    if (teamId == 0) {
        return QPointF(200, 300); // Center of blue zone
    }
    else {
        return QPointF(600, 300); // Center of red zone
    }
}

qreal GameField::getHomeZoneRadius(int teamId) const {
    Q_UNUSED(teamId); // Suppress unused parameter warning as radius is same for both
    return 30; // Radius of the circular zones
}

void GameField::updateAgents() {
    // Update agent positions and behavior
    gameManager->updateGame();

    for (Agent* agent : blueAgents) {
        QList<QGraphicsItem*> agentItems = scene->items();
        for (QGraphicsItem* item : agentItems) {
            if (item->data(0).toULongLong() == reinterpret_cast<quintptr>(agent)) {
                QGraphicsPolygonItem* agentItem = static_cast<QGraphicsPolygonItem*>(item);
                int x = agent->getX() * cellSize;
                int y = agent->getY() * cellSize;
                QPolygon agentTriangle;
                agentTriangle << QPoint(x, y) << QPoint(x - 10, y + 20) << QPoint(x + 10, y + 20);
                agentItem->setPolygon(agentTriangle);
                break;
            }
        }
    }

    // Update graphical representation of red agents
    for (Agent* agent : redAgents) {
        QList<QGraphicsItem*> agentItems = scene->items();
        for (QGraphicsItem* item : agentItems) {
            if (item->data(0).toULongLong() == reinterpret_cast<quintptr>(agent)) {
                QGraphicsPolygonItem* agentItem = static_cast<QGraphicsPolygonItem*>(item);
                int x = agent->getX() * cellSize;
                int y = agent->getY() * cellSize;
                QPolygon agentTriangle;
                agentTriangle << QPoint(x, y) << QPoint(x - 10, y + 20) << QPoint(x + 10, y + 20);
                agentItem->setPolygon(agentTriangle);
                break;
            }
        }
    }
}

void GameField::handleGameTimerTimeout() {
    updateAgents();
    // Other game logic here
}

void GameField::handleFlagGrabAttempt(int agentId) {
    Agent* agent = findAgentById(agentId);
    if (agent) {
        bool isTagged = agent->isTagged();
        bool success = flagManager->checkFlagGrabRequest(agentId, agent->getPosition(), isTagged);
        if (success) {
            // Handle successful flag grab
            // ...
        }
    }
}

void GameField::handleTagAttempt(int taggerId, int targetId) {
    Agent* tagger = findAgentById(taggerId);
    Agent* target = findAgentById(targetId);
    if (tagger && target) {
        bool success = tagManager->checkTagRequest(taggerId, targetId, tagger->getPosition(), target->getPosition());
        if (success) {
            // Handle successful tag
            target->setTagged(true);
        }
    }
}

void GameField::handleFlagCapture(int agentId, bool isBlueTeam) {
    // Update the score text items based on the team that captured the flag
    if (isBlueTeam) {
        int blueScore = blueScoreTextItem->toPlainText().split(": ")[1].toInt() + 1;
        blueScoreTextItem->setPlainText(QString("Blue Score: %1").arg(blueScore));
    }
    else {
        int redScore = redScoreTextItem->toPlainText().split(": ")[1].toInt() + 1;
        redScoreTextItem->setPlainText(QString("Red Score: %1").arg(redScore));
    }

    // Check if the game should be stopped based on a score condition
    int maxScore = 50; // Set the maximum score to end the game
    if (blueScoreTextItem->toPlainText().split(": ")[1].toInt() >= maxScore ||
        redScoreTextItem->toPlainText().split(": ")[1].toInt() >= maxScore) {
        stopGame();
    }
}

void GameField::handleFlagReturned(bool isBlueTeam) {
    // Reset the flag state for the corresponding team
    flagManager->returnFlag(isBlueTeam);
}

void GameField::handleAgentTagged(int agentId) {
    Agent* agent = findAgentById(agentId);
    if (agent) {
        agent->setTagged(true);
    }
}

Agent* GameField::findAgentById(int agentId) {
    for (Agent* agent : blueAgents) {
        if (agent->getId() == agentId) {
            return agent;
        }
    }

    for (Agent* agent : redAgents) {
        if (agent->getId() == agentId) {
            return agent;
        }
    }

    return nullptr;
}

void GameField::stopGame() {
    // Stop the game timer
    QTimer* gameTimer = findChild<QTimer*>();
    if (gameTimer) {
        gameTimer->stop();
    }

    // Display a game over message
    QGraphicsTextItem* gameOverTextItem = new QGraphicsTextItem();
    gameOverTextItem->setPos(scene->sceneRect().center() - QPointF(100, 0));
    gameOverTextItem->setDefaultTextColor(Qt::white);
    gameOverTextItem->setFont(QFont("Arial", 24, QFont::Bold));
    gameOverTextItem->setPlainText("Game Over");
    scene->addItem(gameOverTextItem);
}

void GameField::setupScene() {
    scene = new QGraphicsScene(this);
    setScene(scene);
    setSceneRect(0, 0, 800, 600);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Add team areas
    QGraphicsRectItem* blueArea = new QGraphicsRectItem(10, 10, 380, 580);
    blueArea->setPen(QPen(Qt::blue, 2));
    scene->addItem(blueArea);

    QGraphicsRectItem* redArea = new QGraphicsRectItem(410, 10, 380, 580);
    redArea->setPen(QPen(Qt::red, 2));
    scene->addItem(redArea);

    // Add flags
    QGraphicsEllipseItem* blueFlag = new QGraphicsEllipseItem(190, 290, 20, 20);
    blueFlag->setBrush(Qt::blue);
    scene->addItem(blueFlag);

    QGraphicsEllipseItem* redFlag = new QGraphicsEllipseItem(590, 290, 20, 20);
    redFlag->setBrush(Qt::red);
    scene->addItem(redFlag);

    // Add team zones (circular areas around flags)
    QGraphicsEllipseItem* blueZone = new QGraphicsEllipseItem(170, 270, 60, 60);
    QPen bluePen(Qt::blue);
    bluePen.setWidth(3);
    blueZone->setPen(bluePen);
    blueZone->setBrush(Qt::NoBrush);
    scene->addItem(blueZone);

    QGraphicsEllipseItem* redZone = new QGraphicsEllipseItem(570, 270, 60, 60);
    QPen redPen(Qt::red);
    redPen.setWidth(3);
    redZone->setPen(redPen);
    redZone->setBrush(Qt::NoBrush);
    scene->addItem(redZone);

    // Add score text items
    blueScoreTextItem = new QGraphicsTextItem();
    blueScoreTextItem->setPos(50, 20);
    blueScoreTextItem->setDefaultTextColor(Qt::blue);
    blueScoreTextItem->setFont(QFont("Arial", 16));
    blueScoreTextItem->setPlainText("Blue Score: 0");
    scene->addItem(blueScoreTextItem);

    redScoreTextItem = new QGraphicsTextItem();
    redScoreTextItem->setPos(550, 20);
    redScoreTextItem->setDefaultTextColor(Qt::red);
    redScoreTextItem->setFont(QFont("Arial", 16));
    redScoreTextItem->setPlainText("Red Score: 0");
    scene->addItem(redScoreTextItem);

    for (Agent* agent : blueAgents) {
        QGraphicsPolygonItem* blueAgent = new QGraphicsPolygonItem();
        QPolygon blueTriangle;
        int x = agent->getX() * cellSize;
        int y = agent->getY() * cellSize;
        blueTriangle << QPoint(x, y) << QPoint(x - 10, y + 20) << QPoint(x + 10, y + 20);
        blueAgent->setPolygon(blueTriangle);
        blueAgent->setBrush(Qt::blue);
        blueAgent->setData(0, QString::number(reinterpret_cast<quintptr>(agent)));
        scene->addItem(blueAgent); // Add the agent to the scene
    }

    // Add red agents
    for (Agent* agent : redAgents) {
        QGraphicsPolygonItem* redAgent = new QGraphicsPolygonItem();
        QPolygon redTriangle;
        int x = agent->getX() * cellSize;
        int y = agent->getY() * cellSize;
        redTriangle << QPoint(x, y) << QPoint(x - 10, y + 20) << QPoint(x + 10, y + 20);
        redAgent->setPolygon(redTriangle);
        redAgent->setBrush(Qt::red);
        redAgent->setData(0, QString::number(reinterpret_cast<quintptr>(agent)));
        scene->addItem(redAgent); // Add the agent to the scene
    }
}

bool GameField::isWithinBounds(const QPoint& position) const {
    return sceneRect().contains(position);
}

const std::vector<Agent*>& GameField::getBlueAgents() const {
    return gameManager->getBlueAgents();
}

const std::vector<Agent*>& GameField::getRedAgents() const {
    return gameManager->getRedAgents();
}