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

GameField::GameField(QWidget* parent)
    : QGraphicsView(parent),
    pathfinder(nullptr),
    cellSize(20),
    blueScoreTextItem(nullptr),
    redScoreTextItem(nullptr),
    tagManager(nullptr),
    flagManager(nullptr),
    gameManager(nullptr) {
    // Set up the scene
    setupScene();

    // Set up the game manager
    gameManager = new GameManager(*this);

    // Set up the tag manager
    tagManager = new TagManager(*this, *flagManager);

    // Set up the flag manager
    flagManager = new FlagManager(*this);

    // Set up the game timer
    QTimer* gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &GameField::handleGameTimerTimeout);
    gameTimer->start(1000); // Update every second (adjust as needed)
}

GameField::~GameField() {
    // Clean up resources
    delete pathfinder;
    delete scene;
    delete tagManager;
    delete flagManager;
    delete gameManager;
}

double GameField::getDistance(const Position& point1, const Position& point2) {
    return std::sqrt(std::pow(point1.x - point2.x, 2) + std::pow(point1.y - point2.y, 2));
}

bool GameField::isInHomeZone(const Position& position, int teamId) const {
    Position homeCenter = getHomeZoneCenter(teamId);
    qreal homeRadius = getHomeZoneRadius(teamId);
    return getDistance(position, homeCenter) <= homeRadius;
}

std::vector<Agent*> GameField::getTeamAgents(int teamId) const {
    // Retrieve the agents from the GameManager
    if (teamId == 0) {
        return gameManager->getBlueAgents();
    }
    else if (teamId == 1) {
        return gameManager->getRedAgents();
    }

    // If teamId is neither 0 nor 1, return an empty vector
    return std::vector<Agent*>();
}

Position GameField::getHomeZoneCenter(int teamId) const {
    if (teamId == 0) {
        return Position(40 + 80 / 2, 260 + 80 / 2); // Center of blue zone
    }
    else {
        return Position(680 + 80 / 2, 260 + 80 / 2); // Center of red zone
    }
}

qreal GameField::getHomeZoneRadius(int teamId) const {
    Q_UNUSED(teamId); // Suppress unused parameter warning as radius is same for both
    return 80 / 2; // Radius of the circular zones
}

void GameField::updateAgents() {
    // Update agent positions and behavior
    gameManager->updateAgents();
    tagManager->checkTags(gameManager->getAgents());
    flagManager->update();
}

void GameField::handleGameTimerTimeout() {
    updateAgents();
    // Other game logic here
}

void GameField::handleFlagCapture(const QString& team) {
    // Update the score text items based on the team that captured the flag
    if (team == "Blue") {
        int blueScore = blueScoreTextItem->toPlainText().split(": ")[1].toInt() + 1;
        blueScoreTextItem->setPlainText(QString("Blue Score: %1").arg(blueScore));
    }
    else if (team == "Red") {
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
    QGraphicsRectItem* blueArea = new QGraphicsRectItem(5, 10, 400, 580);
    blueArea->setPen(QPen(Qt::blue, 2));
    scene->addItem(blueArea);

    QGraphicsRectItem* redArea = new QGraphicsRectItem(410, 10, 390, 580);
    redArea->setPen(QPen(Qt::red, 2));
    scene->addItem(redArea);

    // Add flags
    QGraphicsEllipseItem* blueFlag = new QGraphicsEllipseItem(70, 290, 20, 20);
    blueFlag->setBrush(Qt::blue);
    scene->addItem(blueFlag);

    QGraphicsEllipseItem* redFlag = new QGraphicsEllipseItem(710, 290, 20, 20);
    redFlag->setBrush(Qt::red);
    scene->addItem(redFlag);

    // Add team zones (circular areas around flags)
    QGraphicsEllipseItem* blueZone = new QGraphicsEllipseItem(40, 260, 80, 80);
    QPen bluePen(Qt::blue);
    bluePen.setWidth(3);
    blueZone->setPen(bluePen);
    blueZone->setBrush(Qt::NoBrush);
    scene->addItem(blueZone);

    QGraphicsEllipseItem* redZone = new QGraphicsEllipseItem(680, 260, 80, 80);
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

    // Add agents
    for (Agent* agent : gameManager->getBlueAgents()) {
        QGraphicsPolygonItem* blueAgent = new QGraphicsPolygonItem();
        QPolygon blueTriangle;
        int x = agent->getX() * cellSize;
        int y = agent->getY() * cellSize;
        blueTriangle << QPoint(x, y) << QPoint(x - 10, y + 20) << QPoint(x + 10, y + 20);
        blueAgent->setPolygon(blueTriangle);
        blueAgent->setBrush(Qt::blue);
        blueAgent->setData(0, QString::number(reinterpret_cast<quintptr>(agent)));
        scene->addItem(blueAgent);
    }

    for (Agent* agent : gameManager->getRedAgents()) {
        QGraphicsPolygonItem* redAgent = new QGraphicsPolygonItem();
        QPolygon redTriangle;
        int x = agent->getX() * cellSize;
        int y = agent->getY() * cellSize;
        redTriangle << QPoint(x, y) << QPoint(x - 10, y + 20) << QPoint(x + 10, y + 20);
        redAgent->setPolygon(redTriangle);
        redAgent->setBrush(Qt::red);
        redAgent->setData(0, QString::number(reinterpret_cast<quintptr>(agent)));
        scene->addItem(redAgent);
    }
}