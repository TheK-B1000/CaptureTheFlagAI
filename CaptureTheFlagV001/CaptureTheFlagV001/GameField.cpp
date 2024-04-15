#include "GameField.h"
#include <QPainter>
#include <QPolygon>
#include <QRandomGenerator>
#include <QTimer>
#include <QGraphicsItem>
#include <QGraphicsTextItem>
#include <QFont>
#include "GameManager.h"

GameField::GameField(QWidget* parent, const std::vector<std::vector<int>>& grid)
    : QGraphicsView(parent), grid(grid), gameManager(new GameManager(cols, rows)) {

    setRenderHint(QPainter::Antialiasing);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Create the Pathfinder object
    rows = grid.size();
    cols = grid[0].size();
    pathfinder = new Pathfinder(grid);
    cellSize = 40;
    taggingDistance = 100; // Set the tagging distance

    setupAgents(4, 4, cols, gameManager);

    // Set up the scene
    setupScene();

    // Set up the score displays
    QGraphicsTextItem* blueScoreText = new QGraphicsTextItem();
    blueScoreTextItem = blueScoreText;
    blueScoreTextItem->setPlainText("Blue Score: 0");
    blueScoreTextItem->setDefaultTextColor(Qt::blue);
    blueScoreTextItem->setFont(QFont("Arial", 16));
    blueScoreTextItem->setPos(10, 10);
    scene->addItem(blueScoreTextItem);

    QGraphicsTextItem* redScoreText = new QGraphicsTextItem();
    redScoreTextItem = redScoreText;
    redScoreTextItem->setPlainText("Red Score: 0");
    redScoreTextItem->setDefaultTextColor(Qt::red);
    redScoreTextItem->setFont(QFont("Arial", 16));
    redScoreTextItem->setPos(600, 10);
    scene->addItem(redScoreTextItem);

    // Add time remaining display
    QGraphicsTextItem* timeRemainingText = new QGraphicsTextItem();
    timeRemainingTextItem = timeRemainingText;
    timeRemainingTextItem->setPlainText("Time Remaining: 600");
    timeRemainingTextItem->setDefaultTextColor(Qt::black);
    timeRemainingTextItem->setFont(QFont("Arial", 16));
    timeRemainingTextItem->setPos(300, 10);
    scene->addItem(timeRemainingTextItem);

    // Start a timer to update agents
    int gameDuration = 300; // 10 minutes in seconds
    timeRemaining = gameDuration;
    blueScore = 0;
    redScore = 0;

    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &GameField::handleGameTimerTimeout);
    gameTimer->start(100);
}

void GameField::clearAgents() {
    // Delete all existing agents
    for (Agent* agent : blueAgents) {
        delete agent->getBrain();
        delete agent->getMemory();
        delete agent;
    }
    blueAgents.clear();

    for (Agent* agent : redAgents) {
        delete agent->getBrain();
        delete agent->getMemory();
        delete agent;
    }
    redAgents.clear();
}

void GameField::setupAgents(int blueCount, int redCount, int cols, GameManager* gameManager) {
    // Initialize blue agents
    for (int i = 0; i < blueCount; i++) {
        int x, y;
        do {
            x = QRandomGenerator::global()->bounded(1, rows / 2);
            y = QRandomGenerator::global()->bounded(1, cols - 1);
        } while (grid[y][x] == 1);
        Brain* blueBrain = new Brain(); // Create a brain for each blue agent
        Memory* blueMemory = new Memory(); // Create a memory for each blue agent
        Agent* agent = new Agent(x, y, "blue", cols, grid, rows, pathfinder, taggingDistance, blueBrain, blueMemory, gameManager, blueAgents, redAgents);
        blueAgents.push_back(agent);
        connect(agent, &Agent::blueFlagCaptured, this, [this]() { handleFlagCapture("blue"); });
        connect(agent, &Agent::redFlagReset, this, [this]() { resetEnemyFlag("red"); });
    }

    // Initialize red agents
    for (int i = 0; i < redCount; i++) {
        int x, y;
        do {
            x = QRandomGenerator::global()->bounded(rows / 2, rows - 1);
            y = QRandomGenerator::global()->bounded(1, cols - 1);
        } while (grid[y][x] == 1);
        Brain* redBrain = new Brain(); // Create a brain for each red agent
        Memory* redMemory = new Memory(); // Create a memory for each red agent
        Agent* agent = new Agent(x, y, "red", cols, grid, rows, pathfinder, taggingDistance, redBrain, redMemory, gameManager, blueAgents, redAgents);
        redAgents.push_back(agent);
        connect(agent, &Agent::redFlagCaptured, this, [this]() { handleFlagCapture("red"); });
        connect(agent, &Agent::blueFlagReset, this, [this]() { resetEnemyFlag("blue"); });
    }
}

void GameField::runTestCase1() {
    // Test case 1: Default game setup (4 blue agents, 4 red agents)
}

void GameField::runTestCase2(int agentCount, GameManager* gameManager) {
    clearAgents();

    int blueCount = agentCount / 2;
    int redCount = agentCount - blueCount;

    setupAgents(blueCount, redCount, grid[0].size(), gameManager);

    setupScene(); // Set up the scene first
    updateSceneItems(); // Update the scene items after setting up the scene
}

void GameField::runTestCase3() {
    // Test case 3: Change the position of team zones and flags
    QGraphicsEllipseItem* blueFlag = findFlagItem("blue");
    QGraphicsEllipseItem* redFlag = findFlagItem("red");

    if (blueFlag && redFlag) {
        // Move the blue team zone to the top-left corner
        QGraphicsEllipseItem* blueZone = nullptr;
        for (QGraphicsItem* item : scene->items()) {
            if (QGraphicsEllipseItem* ellipseItem = qgraphicsitem_cast<QGraphicsEllipseItem*>(item)) {
                QPen pen = ellipseItem->pen();
                if (pen.color() == Qt::blue && pen.width() == 3) {
                    blueZone = ellipseItem;
                    break;
                }
            }
        }

        if (blueZone) {
            QRectF blueZoneRect(0, 0, 100, 100);
            blueZone->setRect(blueZoneRect);

            // Move the blue flag to the center of the new team zone position
            QPointF blueFlagPos = blueZoneRect.center() - QPointF(10, 10);
            blueFlag->setPos(blueFlagPos);
        }

        // Move the red team zone to the bottom-right corner
        QGraphicsEllipseItem* redZone = nullptr;
        for (QGraphicsItem* item : scene->items()) {
            if (QGraphicsEllipseItem* ellipseItem = qgraphicsitem_cast<QGraphicsEllipseItem*>(item)) {
                QPen pen = ellipseItem->pen();
                if (pen.color() == Qt::red && pen.width() == 3) {
                    redZone = ellipseItem;
                    break;
                }
            }
        }

        if (redZone) {
            QRectF redZoneRect(700, 500, 100, 100);
            redZone->setRect(redZoneRect);

            // Move the red flag to the center of the new team zone position
            QPointF redFlagPos = redZoneRect.center() - QPointF(10, 10);
            redFlag->setPos(redFlagPos);
        }

        // Update the grid to reflect the new team zone positions
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                QPointF cellCenter(j * cellSize + cellSize / 2, i * cellSize + cellSize / 2);
                if (blueZone && blueZone->contains(cellCenter)) {
                    grid[i][j] = 2; // Mark cells within the blue team zone
                }
                else if (redZone && redZone->contains(cellCenter)) {
                    grid[i][j] = 3; // Mark cells within the red team zone
                }
                else {
                    grid[i][j] = 0; // Mark cells outside team zones as empty
                }
            }
        }

        updateAgentItemsPositions();
    }
}

void GameField::handleFlagCapture(const QString& team) {
    if (team == "blue") {
        blueScore++;
        resetEnemyFlag("red");
    }
    else if (team == "red") {
        redScore++;
        resetEnemyFlag("blue");
    }

    updateScoreDisplay();
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
    updateAgentPositions();
    checkTagging();
    updateAgentItemsPositions();
    this->viewport()->update();
}

void GameField::handleGameTimerTimeout() {
    timeRemaining--;
    updateTimeDisplay();

    qDebug() << "Time remaining:" << timeRemaining;
    qDebug() << "Blue score:" << blueScore;
    qDebug() << "Red score:" << redScore;

    if (timeRemaining <= 0) {
        qDebug() << "Stopping the game";
        stopGame();
        declareWinner();
    }
    else {
        // Update agents every N iterations to reduce excessive updating
        const int updateFrequency = 5; // Adjust this value based on your game's requirements
        static int updateCounter = 0;

        if (updateCounter % updateFrequency == 0) {
            qDebug() << "Updating agents";
            updateAgents();
        }

        updateCounter++;
    }
}

void GameField::stopGame() {
    qDebug() << "Stopping the game timer";
    gameTimer->stop();

    qDebug() << "Disabling blue agents";
    for (Agent* agent : blueAgents) {
        agent->setEnabled(false);
    }

    qDebug() << "Disabling red agents";
    for (Agent* agent : redAgents) {
        agent->setEnabled(false);
    }
}

void GameField::declareWinner() {
    QGraphicsTextItem* winnerText = new QGraphicsTextItem();
    winnerText->setFont(QFont("Arial", 24));
    winnerText->setPos(300, 250);

    if (blueScore > redScore) {
        // Blue team wins
        winnerText->setPlainText("Game Over! Blue Team Wins!");
        winnerText->setDefaultTextColor(Qt::blue);
    }
    else if (redScore > blueScore) {
        // Red team wins
        winnerText->setPlainText("Game Over! Red Team Wins!");
        winnerText->setDefaultTextColor(Qt::red);
    }
    else {
        // It's a tie
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
        else {
            // Handle the case when the agent item is not found in the scene
            qDebug() << "Agent item not found in the scene for blue agent at position (" << agent->getX() << ", " << agent->getY() << ")";
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
        else {
            // Handle the case when the agent item is not found in the scene
            qDebug() << "Agent item not found in the scene for red agent at position (" << agent->getX() << ", " << agent->getY() << ")";
        }
    }
}

void GameField::checkTagging() {
    for (Agent* agent : blueAgents) {
        if (agent->isOnEnemySide() && !agent->isTagged()) {
            for (Agent* enemyAgent : redAgents) {
                if (enemyAgent->checkInTeamZone() && !enemyAgent->isTagged() && agent->distanceTo(enemyAgent) <= taggingDistance) {
                    agent->setIsTagged(true);
                    break;
                }
            }
        }
    }

    for (Agent* agent : redAgents) {
        if (agent->isOnEnemySide() && !agent->isTagged()) {
            for (Agent* enemyAgent : blueAgents) {
                if (enemyAgent->checkInTeamZone() && !enemyAgent->isTagged() && agent->distanceTo(enemyAgent) <= taggingDistance) {
                    agent->setIsTagged(true);
                    break;
                }
            }
        }
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

void GameField::updateAgentItem(QGraphicsItem* item, const std::vector<Agent*>& agents, QColor color) {
    if (QGraphicsPolygonItem* agentItem = qgraphicsitem_cast<QGraphicsPolygonItem*>(item)) {
        for (Agent* agent : agents) {
            if (agentItem->brush().color() == color) {
                int x = agent->getX() * cellSize;
                int y = agent->getY() * cellSize;
                const int squareSize = 20;
                QPolygon square;
                square << QPoint(x - squareSize / 2, y - squareSize / 2)
                    << QPoint(x + squareSize / 2, y - squareSize / 2)
                    << QPoint(x + squareSize / 2, y + squareSize / 2)
                    << QPoint(x - squareSize / 2, y + squareSize / 2);
                agentItem->setPolygon(square);
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

void GameField::resetEnemyFlag(const QString& team) {
    if (team == "red") {
        // Reset the red flag position
        QGraphicsEllipseItem* redFlag = findFlagItem("red");
        if (redFlag) {
            redFlag->setPos(705, 285);
        }
        else {
            // Handle the case when the red flag item is not found in the scene
            qDebug() << "Red flag item not found in the scene";
        }

        // Reset the carrying flag state of blue agents
        for (Agent* agent : blueAgents) {
            agent->setCarryingFlag(false);
        }
    }
    else if (team == "blue") {
        // Reset the blue flag position
        QGraphicsEllipseItem* blueFlag = findFlagItem("blue");
        if (blueFlag) {
            blueFlag->setPos(65, 285);
        }
        else {
            // Handle the case when the blue flag item is not found in the scene
            qDebug() << "Blue flag item not found in the scene";
        }

        // Reset the carrying flag state of red agents
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

    // Calculate the number of rows and columns based on the scene size and cell size
    rows = sceneRect().height() / cellSize;
    cols = sceneRect().width() / cellSize;

    // Create the grid based on the calculated rows and columns
    grid.resize(rows, std::vector<int>(cols, 0));

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