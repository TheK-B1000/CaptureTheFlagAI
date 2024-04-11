#include "GameField.h"
#include <QPainter>
#include <QRandomGenerator>
#include <QTimer>
#include <QGraphicsTextItem>
#include <QFont>

GameField::GameField(QWidget * parent) : QGraphicsView(parent) {
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
    pathfinder = new Pathfinder(grid);
    cellSize = 40;
    taggingDistance = 100; // Set the tagging distance

    // Initialize blue agents
    for (int i = 0; i < 4; i++) {
        int x, y;
        do {
            x = QRandomGenerator::global()->bounded(1, 4);
            y = QRandomGenerator::global()->bounded(1, cols - 1);
        } while (grid[x][y] == 1); // Avoid obstacle positions
        Brain* blueBrain = new Brain(); // Create a brain for each blue agent
        Memory* blueMemory = new Memory(); // Create a memory for each blue agent
        Agent* agent = new Agent(x, y, "blue", cols, grid, rows, pathfinder, taggingDistance, blueBrain, blueMemory);
        blueAgents.push_back(agent);
    }

    // Initialize red agents
    for (int i = 0; i < 4; i++) {
        int x, y;
        do {
            x = QRandomGenerator::global()->bounded(6, 9);
            y = QRandomGenerator::global()->bounded(1, cols - 1);
        } while (grid[x][y] == 1); // Avoid obstacle positions
        Brain* redBrain = new Brain(); // Create a brain for each red agent
        Memory* redMemory = new Memory(); // Create a memory for each red agent
        Agent* agent = new Agent(x, y, "red", cols, grid, rows, pathfinder, taggingDistance, redBrain, redMemory);
        redAgents.push_back(agent);
    }

    // Set up the scene
    setupScene();

    // Set up the score displays
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

    // Start the game timer
    int gameDuration = 300; // 5 minutes in seconds
    timeRemaining = gameDuration;
    blueScore = 0;
    redScore = 0;

    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &GameField::handleGameTimerTimeout);
    gameTimer->start(1000);
}

GameField::~GameField() {
    delete pathfinder;
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

void GameField::updateAgents() {
    std::vector<std::pair<int, int>> bluePositions;
    std::vector<std::pair<int, int>> redPositions;

    for (Agent* agent : blueAgents) {
        bluePositions.emplace_back(agent->getX(), agent->getY());
    }
    for (Agent* agent : redAgents) {
        redPositions.emplace_back(agent->getX(), agent->getY());
    }

    for (Agent* agent : blueAgents) {
        agent->update(redPositions, redAgents);
    }
    for (Agent* agent : redAgents) {
        agent->update(bluePositions, blueAgents);
    }

    updateScoreDisplay();
    updateAgentItemsPositions();
    this->viewport()->update();
}

void GameField::handleGameTimerTimeout() {
    timeRemaining--;
    updateTimeDisplay();

    if (timeRemaining <= 0) {
        stopGame();
        declareWinner();
    }
    else {
        updateAgents();
    }
}

void GameField::stopGame() {
    gameTimer->stop();

    for (Agent* agent : blueAgents) {
        agent->setEnabled(false);
    }

    for (Agent* agent : redAgents) {
        agent->setEnabled(false);
    }
}

void GameField::declareWinner() {
    QGraphicsTextItem* winnerText = new QGraphicsTextItem();
    winnerText->setFont(QFont("Arial", 24));
    winnerText->setPos(300, 250);

    if (blueScore > redScore) {
        winnerText->setPlainText("Game Over! Blue Team Wins!");
        winnerText->setDefaultTextColor(Qt::blue);
    }
    else if (redScore > blueScore) {
        winnerText->setPlainText("Game Over! Red Team Wins!");
        winnerText->setDefaultTextColor(Qt::red);
    }
    else 
    {
        winnerText->setPlainText("Game Over! It's a Draw!");
        winnerText->setDefaultTextColor(Qt::black);
    }

    scene->addItem(winnerText);
}

void GameField::updateAgentPositions() {
    std::vector<std::pair<int, int>> bluePositions;
    std::vector<std::pair<int, int>> redPositions;

    for (Agent* agent : blueAgents) {
        bluePositions.emplace_back(agent->getX(), agent->getY());
    }
    for (Agent* agent : redAgents) {
        redPositions.emplace_back(agent->getX(), agent->getY());
    }

    for (Agent* agent : blueAgents) {
        agent->update(redPositions, redAgents);
    }
    for (Agent* agent : redAgents) {
        agent->update(bluePositions, blueAgents);
    }
}

void GameField::updateAgentItemsPositions() {
    for (Agent* agent : blueAgents) {
        QGraphicsItem* item = getAgentItem(agent);
        if (item) {
            item->setPos(agent->getX() * cellSize, agent->getY() * cellSize);
            QGraphicsPolygonItem* agentItem = qgraphicsitem_cast<QGraphicsPolygonItem*>(item);
            if (agentItem) {
                if (agent->isTagged()) {
                    QPen pen(Qt::yellow, 3);
                    agentItem->setPen(pen);
                }
                else {
                    QPen pen(Qt::black, 1);
                    agentItem->setPen(pen);
                }

                if (agent->isCarryingFlag()) {
                    agentItem->setBrush(Qt::cyan);
                }
                else {
                    agentItem->setBrush(Qt::blue);
                }
            }
        }
    }

    for (Agent* agent : redAgents) {
        QGraphicsItem* item = getAgentItem(agent);
        if (item) {
            item->setPos(agent->getX() * cellSize, agent->getY() * cellSize);
            QGraphicsPolygonItem* agentItem = qgraphicsitem_cast<QGraphicsPolygonItem*>(item);
            if (agentItem) {
                if (agent->isTagged()) {
                    QPen pen(Qt::yellow, 3);
                    agentItem->setPen(pen);
                }
                else {
                    QPen pen(Qt::black, 1);
                    agentItem->setPen(pen);
                }

                if (agent->isCarryingFlag()) {
                    agentItem->setBrush(Qt::magenta);
                }
                else {
                    agentItem->setBrush(Qt::red);
                }
            }
        }
    }
}

void GameField::checkTagging() {
    for (Agent* agent : blueAgents) {
        if (!agent->isTagged()) {
            for (Agent* enemyAgent : redAgents) {
                if (!enemyAgent->isTagged() && agent->distanceTo(enemyAgent) <= taggingDistance) {
                    agent->setIsTagged(true);
                    break;
                }
            }
        }
    }

    for (Agent* agent : redAgents) {
        if (!agent->isTagged()) {
            for (Agent* enemyAgent : blueAgents) {
                if (!enemyAgent->isTagged() && agent->distanceTo(enemyAgent) <= taggingDistance) {
                    agent->setIsTagged(true);
                    break;
                }
            }
        }
    }
}

void GameField::updateAgentItem(QGraphicsItem* item, const std::vector<Agent*>& agents, QColor color) {
    if (QGraphicsEllipseItem* agentItem = qgraphicsitem_cast<QGraphicsEllipseItem*>(item)) {
        for (Agent* agent : agents) {
            if (agentItem->brush().color() == color) {
                int x = agent->getX() * cellSize;
                int y = agent->getY() * cellSize;
                agentItem->setRect(x - 10, y - 10, 20, 20);
            }
        }
    }
}

void GameField::updateSceneItems() {
    for (QGraphicsItem* item : scene->items()) {
        updateAgentItem(item, blueAgents, Qt::blue);
        updateAgentItem(item, redAgents, Qt::red);
    }
}

QGraphicsItem* GameField::getAgentItem(Agent* agent) {
    for (QGraphicsItem* item : scene->items()) {
        if (item->data(0).toString() == QString::number(reinterpret_cast<quintptr>(agent))) {
            return item;
        }
    }
    return nullptr;
}


void GameField::resetEnemyFlag(const QString& team) {
    if (team == "red") {
        if (redFlagItem) {
            redFlagItem->setPos(705, 285);
        }
        for (Agent* agent : blueAgents) {
            agent->setCarryingFlag(false);
        }
    }
    else if (team == "blue") {
        if (blueFlagItem) {
            blueFlagItem->setPos(65, 285);
        }
        for (Agent* agent : redAgents) {
            agent->setCarryingFlag(false);
        }
    }
}

QGraphicsEllipseItem* GameField::findFlagItem(const QString& team) {
    for (QGraphicsItem* item : scene->items()) {
        if (QGraphicsEllipseItem* ellipseItem = qgraphicsitem_cast<QGraphicsEllipseItem*>(item)) {
            if ((team == "red" && ellipseItem->brush().color() == Qt::red) ||
                (team == "blue" && ellipseItem->brush().color() == Qt::blue)) {
                return ellipseItem;
            }
        }
    }
    return nullptr;
}

void GameField::handleFlagCapture(const QString& team) {
    if (team == "blue") {
        // Blue team captured the flag
        blueScore++;
        resetEnemyFlag("red");
    }
    else if (team == "red") {
        // Red team captured the flag
        redScore++;
        resetEnemyFlag("blue");
    }

    updateScoreDisplay();
}

void GameField::updateScoreDisplay() {
    blueScoreTextItem->setPlainText("Blue Score: " + QString::number(blueScore));
    redScoreTextItem->setPlainText("Red Score: " + QString::number(redScore));
}

void GameField::updateTimeDisplay() {
    timeRemainingTextItem->setPlainText("Time Remaining: " + QString::number(timeRemaining));
}

void GameField::setupScene() {
    scene = new QGraphicsScene(this);
    setScene(scene);
    setSceneRect(0, 0, 800, 600);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QGraphicsRectItem* blueArea = new QGraphicsRectItem(5, 10, 400, 580);
    blueArea->setPen(QPen(Qt::blue, 2));
    scene->addItem(blueArea);

    QGraphicsRectItem* redArea = new QGraphicsRectItem(410, 10, 390, 580);
    redArea->setPen(QPen(Qt::red, 2));
    scene->addItem(redArea);

    QGraphicsEllipseItem* blueFlag = new QGraphicsEllipseItem(70, 290, 20, 20);
    blueFlag->setBrush(Qt::blue);
    scene->addItem(blueFlag);
    blueFlagItem = blueFlag;

    QGraphicsEllipseItem* redFlag = new QGraphicsEllipseItem(710, 290, 20, 20);
    redFlag->setBrush(Qt::red);
    scene->addItem(redFlag);
    redFlagItem = redFlag;

    for (Agent* agent : blueAgents) {
        QGraphicsPolygonItem* blueAgent = new QGraphicsPolygonItem();
        QPolygon blueTriangle;
        int x = agent->getX() * cellSize;
        int y = agent->getY() * cellSize;
        blueTriangle << QPoint(x, y) << QPoint(x - 10, y + 20) << QPoint(x + 10, y + 20);
        blueAgent->setPolygon(blueTriangle);
        scene->addItem(blueAgent);
    }

    for (Agent* agent : redAgents) {
        QGraphicsPolygonItem* redAgent = new QGraphicsPolygonItem();
        QPolygon redTriangle;
        int x = agent->getX() * cellSize;
        int y = agent->getY() * cellSize;
        redTriangle << QPoint(x, y) << QPoint(x - 10, y + 20) << QPoint(x + 10, y + 20);
        redAgent->setPolygon(redTriangle);
        scene->addItem(redAgent);
    }
}