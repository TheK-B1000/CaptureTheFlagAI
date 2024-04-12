#include "GameField.h"
#include <QPainter>
#include <QPolygon>
#include <QRandomGenerator>
#include <QTimer>
#include <QGraphicsItem>
#include <QGraphicsTextItem>
#include <QFont>

GameField::GameField(QWidget* parent) : QGraphicsView(parent) {
    setRenderHint(QPainter::Antialiasing);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Create the Pathfinder object
    std::vector<std::vector<int>> grid = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 1, 0, 0, 1, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 1, 0, 0, 1, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    };
    int rows = grid.size();
    int cols = grid[0].size();
    this->grid = grid;
    this->rows = rows;
    this->cols = cols;
    pathfinder = new Pathfinder(grid);
    cellSize = 40;
    taggingDistance = 100; // Set the tagging distance

    // Initialize game components
    gameManager = new GameManager(this);
    flagManager = new FlagManager(this);
    tagManager = new TagManager(this);

    // Initialize blue agents
    initializeBlueAgents();

    // Initialize red agents
    initializeRedAgents();

    // Set up the scene
    setupScene();

    // Set up the score displays
    setupScoreDisplays();

    // Start a timer to update agents
    int gameDuration = 300; // 10 minutes in seconds
    timeRemaining = gameDuration;
    blueScore = 0;
    redScore = 0;

    gameTimer = new QTimer(this);
    gameTimer->start(1000);
}

void GameField::initializeBlueAgents() {
    for (int i = 0; i < 4; i++) {
        int x, y;
        do {
            x = QRandomGenerator::global()->bounded(1, 4);
            y = QRandomGenerator::global()->bounded(1, cols - 1);
        } while (grid[x][y] == 1); // Avoid obstacle positions
        Brain* blueBrain = new Brain(); // Create a brain for each blue agent
        Memory* blueMemory = new Memory(); // Create a memory for each blue agent
        Agent* agent = new Agent(x, y, "blue", cols, grid, rows, pathfinder, taggingDistance, blueBrain, blueMemory, blueAgents, redAgents);
        blueAgents.push_back(agent);
    }
}

void GameField::initializeRedAgents() {
    for (int i = 0; i < 4; i++) {
        int x, y;
        do {
            x = QRandomGenerator::global()->bounded(6, 9);
            y = QRandomGenerator::global()->bounded(1, cols - 1);
        } while (grid[x][y] == 1); // Avoid obstacle positions
        Brain* redBrain = new Brain(); // Create a brain for each red agent
        Memory* redMemory = new Memory(); // Create a memory for each red agent
        Agent* agent = new Agent(x, y, "red", cols, grid, rows, pathfinder, taggingDistance, redBrain, redMemory, blueAgents, redAgents);
        redAgents.push_back(agent);
    }
}

void GameField::setupScoreDisplays() {
    blueScoreTextItem = new QGraphicsTextItem();
    blueScoreTextItem->setPlainText("Blue Score: 0");
    blueScoreTextItem->setDefaultTextColor(Qt::blue);
    blueScoreTextItem->setFont(QFont("Arial", 16));
    blueScoreTextItem->setPos(10, 10);
    scene->addItem(blueScoreTextItem);

    redScoreTextItem = new QGraphicsTextItem();
    redScoreTextItem->setPlainText("Red Score: 0");
    redScoreTextItem->setDefaultTextColor(Qt::red);
    redScoreTextItem->setFont(QFont("Arial", 16));
    redScoreTextItem->setPos(600, 10);
    scene->addItem(redScoreTextItem);

    timeRemainingTextItem = new QGraphicsTextItem();
    timeRemainingTextItem->setPlainText("Time Remaining: 600");
    timeRemainingTextItem->setDefaultTextColor(Qt::black);
    timeRemainingTextItem->setFont(QFont("Arial", 16));
    timeRemainingTextItem->setPos(300, 10);
    scene->addItem(timeRemainingTextItem);
}

GameField::~GameField() {
    delete pathfinder;
    delete gameManager;
    delete flagManager;
    delete tagManager;

    for (Agent* agent : blueAgents) {
        delete agent->getBrain();
        delete agent->getMemory();
        delete agent;
    }
    for (Agent* agent : redAgents) {
        delete agent->getBrain();
        delete agent->getMemory();
        delete agent;
    }
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

    // Add obstacles
    QGraphicsRectItem* obstacle1 = new QGraphicsRectItem(195, 140, 40, 20);
    obstacle1->setBrush(Qt::gray);
    scene->addItem(obstacle1);

    QGraphicsRectItem* obstacle2 = new QGraphicsRectItem(195, 290, 40, 20);
    obstacle2->setBrush(Qt::gray);
    scene->addItem(obstacle2);

    QGraphicsRectItem* obstacle3 = new QGraphicsRectItem(195, 440, 40, 20);
    obstacle3->setBrush(Qt::gray);
    scene->addItem(obstacle3);

    QGraphicsRectItem* obstacle4 = new QGraphicsRectItem(565, 140, 40, 20);
    obstacle4->setBrush(Qt::gray);
    scene->addItem(obstacle4);

    QGraphicsRectItem* obstacle5 = new QGraphicsRectItem(565, 290, 40, 20);
    obstacle5->setBrush(Qt::gray);
    scene->addItem(obstacle5);

    QGraphicsRectItem* obstacle6 = new QGraphicsRectItem(565, 440, 40, 20);
    obstacle6->setBrush(Qt::gray);
    scene->addItem(obstacle6);

    // Add agents
    for (Agent* agent : blueAgents) {
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

    for (Agent* agent : redAgents) {
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
