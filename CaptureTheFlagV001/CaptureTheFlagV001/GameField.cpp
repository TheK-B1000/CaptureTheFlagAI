#include "GameField.h"
#include <QPainter>
#include <QPolygon>
#include <QRandomGenerator>
#include <QTimer>
#include <QGraphicsItem>
#include <QGraphicsTextItem>
#include <QFont>
#include <memory>
#include "GameManager.h"

GameField::GameField(QWidget* parent, const std::vector<std::vector<int>>& grid)
    : QGraphicsView(parent), grid(grid), gameFieldWidth(0), gameFieldHeight(0) {
    setRenderHint(QPainter::Antialiasing);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Create the Pathfinder object
    rows = grid.size();
    cols = grid[0].size();
    if (rows == 0 || cols == 0) {
        // Handle the case when the grid is empty
        qDebug() << "Error: Grid is empty";
        return;
    }

    // Print the grid representation for debugging
    qDebug() << "Grid representation:";
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            qDebug() << grid[i][j];
        }
    }

    cellSize = 20;
    taggingDistance = 100; // Set the tagging distance

    // Initialize the GameManager after setting rows and cols
    gameManager = new GameManager(cols, rows);

    // Create the Pathfinder object after setting rows and cols
    pathfinder = new Pathfinder(grid, rows, cols);

    qDebug() << "Grid dimensions:";
    qDebug() << "Rows:" << grid.size();
    for (int y = 0; y < grid.size(); y++) {
        qDebug() << "Row" << y << "Cols:" << grid[y].size();
    }

    // Set up the agents before setting up the scene
   // setupAgents(4, 4, cols, rows, gameManager);

    // Set up the scene after setting up the agents
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
    int gameDuration = 600; // 10 minutes in seconds
    timeRemaining = gameDuration;
    blueScore = 0;
    redScore = 0;

    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &GameField::handleGameTimerTimeout);
    gameTimer->start(100);
}

void GameField::clearAgents() {
    // Delete all existing agents
    blueAgents.clear();
    redAgents.clear();
}

void GameField::setupAgents(int blueCount, int redCount, int cols, int rows, GameManager* gameManager) {
    qDebug() << "setupAgents called with blueCount:" << blueCount << "and redCount:" << redCount;

    // Initialize blue agents
    for (int i = 0; i < blueCount; i++) {
        int x, y;
        int gridX, gridY;
        bool validPosition = false;

        while (!validPosition) {
            x = 20 + QRandomGenerator::global()->bounded(0, (cols / 2) - 1) * cellSize;
            y = 100 + QRandomGenerator::global()->bounded(0, rows - 1) * cellSize;

            // Adjust x and y to grid coordinates
            gridX = (x - 20) / cellSize;
            gridY = (y - 100) / cellSize;

            if (gridX >= 0 && gridX < cols / 2 && gridY >= 0 && gridY < rows && grid[gridY][gridX] != 1) {
                validPosition = true;
            }
        }

        // Construct the blue agent and add to the list
        auto blueBrain = std::make_shared<Brain>();
        auto blueMemory = std::make_shared<Memory>();
        auto agent = std::make_shared<Agent>(gridX, gridY, "blue", cols, rows, grid, pathfinder, taggingDistance, blueBrain, blueMemory, gameManager, blueAgents, redAgents);
        blueAgents.push_back(agent);
        grid[gridY][gridX] = 1;

        // Connect signals
        connect(agent.get(), &Agent::redFlagCaptured, this, [this]() { handleFlagCapture("red"); });
        connect(agent.get(), &Agent::blueFlagReset, this, [this]() { resetEnemyFlag("blue"); });
    }

    // Initialize red agents
    for (int i = 0; i < redCount; i++) {
        int x, y;
        int gridX, gridY;
        bool validPosition = false;

        while (!validPosition) {
            x = 600 + QRandomGenerator::global()->bounded(0, cols - 2) * cellSize;
            y = 100 + QRandomGenerator::global()->bounded(0, rows - 2) * cellSize;

            // Adjust x and y to grid coordinates
            gridX = (x - 600) / cellSize;
            gridY = (y - 100) / cellSize;

            if (gridX >= cols / 2 && gridX < cols && gridY >= 0 && gridY < rows && grid[gridY][gridX] != 1) {
                validPosition = true;
            }
        }

        // Construct the red agent and add to the list
        auto redBrain = std::make_shared<Brain>();
        auto redMemory = std::make_shared<Memory>();
        auto agent = std::make_shared<Agent>(gridX, gridY, "red", cols, rows, grid, pathfinder, taggingDistance, redBrain, redMemory, gameManager, blueAgents, redAgents);
        redAgents.push_back(agent);
        grid[gridY][gridX] = 1;

        // Connect signals
        connect(agent.get(), &Agent::redFlagCaptured, this, [this]() { handleFlagCapture("red"); });
        connect(agent.get(), &Agent::blueFlagReset, this, [this]() { resetEnemyFlag("blue"); });
    }
}


void GameField::runTestCase1() {
    // Test case 1: Default game setup (4 blue agents, 4 red agents)
}

void GameField::runTestCase2(int agentCount, GameManager* gameManager) {
    clearAgents();
    int blueCount = agentCount / 2;
    int redCount = agentCount - blueCount;
    setupAgents(blueCount, redCount, grid[0].size(), grid.size(), gameManager);
    setupScene();
    updateSceneItems();

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
    int gameDuration = 600; // 10 minutes in seconds
    timeRemaining = gameDuration;
    blueScore = 0;
    redScore = 0;

    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &GameField::handleGameTimerTimeout);
    gameTimer->start(100);
}

void GameField::runTestCase3() {
    // Test case 3: Change the position of team zones and flags
    QGraphicsPolygonItem* blueFlag = findFlagItem("blue");
    QGraphicsPolygonItem* redFlag = findFlagItem("red");

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
            QPointF blueFlagCenter = blueZoneRect.center();
            QPolygon blueTriangle;
            blueTriangle << QPoint(blueFlagCenter.x() - 10, blueFlagCenter.y() - 20)
                << QPoint(blueFlagCenter.x(), blueFlagCenter.y())
                << QPoint(blueFlagCenter.x() + 10, blueFlagCenter.y() - 20);
            blueFlag->setPolygon(blueTriangle);
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
            QPointF redFlagCenter = redZoneRect.center();
            QPolygon redTriangle;
            redTriangle << QPoint(redFlagCenter.x() - 10, redFlagCenter.y() - 20)
                << QPoint(redFlagCenter.x(), redFlagCenter.y())
                << QPoint(redFlagCenter.x() + 10, redFlagCenter.y() - 20);
            redFlag->setPolygon(redTriangle);
        }

        // Update the grid to reflect the new team zone positions
        updateGridFromZones(blueZone, redZone);

        // Update the agent item positions
        for (const auto& agent : blueAgents) {
            QGraphicsItem* item = getAgentItem(agent.get());
            updateAgentItemPositions(item, agent, agent->getX(), agent->getY());
        }

        for (const auto& agent : redAgents) {
            QGraphicsItem* item = getAgentItem(agent.get());
            updateAgentItemPositions(item, agent, agent->getX(), agent->getY());
        }
    }
    else {
        qDebug() << "Error: Red or blue flag item not found in the scene";
    }
}

void GameField::updateGridFromZones(QGraphicsEllipseItem* blueZone, QGraphicsEllipseItem* redZone) {
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
}

void GameField::handleFlagCapture(const QString& side) {
    if (side == "blue") {
        blueScore++;
        resetEnemyFlag("red");
    }
    else if (side == "red") {
        redScore++;
        resetEnemyFlag("blue");
    }

    updateScoreDisplay();
}

void GameField::updateAgents() {
    std::vector<Agent*> blueAgentPointers;
    for (const auto& agent : blueAgents) {
        blueAgentPointers.push_back(agent.get());
    }

    std::vector<Agent*> redAgentPointers;
    for (const auto& agent : redAgents) {
        redAgentPointers.push_back(agent.get());
    }

    // Update blue agents
    for (const auto& agent : blueAgents) {
        agent->update(getAgentPositions(redAgents), redAgentPointers, blueAgents, redAgents);
    }

    // Update red agents
    for (const auto& agent : redAgents) {
        agent->update(getAgentPositions(blueAgents), blueAgentPointers, blueAgents, redAgents);
    }

    qDebug() << "Updating agent positions";
    updateAgentPositions();

    qDebug() << "Checking tagging";
    checkTagging();

    qDebug() << "Updating agent item positions";
    for (const auto& agent : blueAgents) {
        QGraphicsItem* item = getAgentItem(agent.get());
        updateAgentItemPositions(item, agent, agent->getX(), agent->getY());
    }

    for (const auto& agent : redAgents) {
        QGraphicsItem* item = getAgentItem(agent.get());
        updateAgentItemPositions(item, agent, agent->getX(), agent->getY());
    }

    this->viewport()->update();
}

std::vector<std::pair<int, int>> GameField::getAgentPositions(const std::vector<std::shared_ptr<Agent>>& agents) const {
    std::vector<std::pair<int, int>> positions;
    for (const auto& agent : agents) {
        positions.emplace_back(agent->getX(), agent->getY());
    }
    return positions;
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
    for (const auto& agent : blueAgents) {
        agent->setEnabled(false);
    }

    qDebug() << "Disabling red agents";
    for (const auto& agent : redAgents) {
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

    for (const auto& agent : blueAgents) {
        QGraphicsItem* item = getAgentItem(agent.get());
        updateAgentItemPositions(item, agent, agent->getX(), agent->getY());
    }

    for (const auto& agent : redAgents) {
        QGraphicsItem* item = getAgentItem(agent.get());
        updateAgentItemPositions(item, agent, agent->getX(), agent->getY());
    }

    // Print blue agent positions for debugging
    qDebug() << "Blue agent positions:";
    for (const auto& pos : bluePositions) {
        qDebug() << "(" << pos.first << ", " << pos.second << ")";
    }

    // Print red agent positions for debugging
    qDebug() << "Red agent positions:";
    for (const auto& pos : redPositions) {
        qDebug() << "(" << pos.first << ", " << pos.second << ")";
    }
}

void GameField::updateAgentItemPositions(QGraphicsItem* item, const std::shared_ptr<Agent>& agent, int x, int y) {
    if (item) {
        qDebug() << "Agent at (" << x << ", " << y << ") - Item found with updateAgentItemPositions";
        item->setPos(x * cellSize, y * cellSize);

        QGraphicsPolygonItem* agentItem = qgraphicsitem_cast<QGraphicsPolygonItem*>(item);
        if (agentItem) {
            // Update the agent item's appearance based on its state
            if (agent->isTagged()) {
                QPen pen(Qt::yellow, 3);
                agentItem->setPen(pen);
            }
            else {
                QPen pen(Qt::black, 1);
                agentItem->setPen(pen);
            }

            if (agent->isCarryingFlag()) {
                agentItem->setBrush(agent->getSide() == "blue" ? Qt::cyan : Qt::magenta);
            }
            else {
                agentItem->setBrush(agent->getSide() == "blue" ? Qt::blue : Qt::red);
            }
        }
    }
    else {
        qDebug() << "Agent item not found in the scene for agent at position (" << x << ", " << y << ") with updateAgentItemPositions";
    }
}

void GameField::checkTagging() {
    for (const auto& agent : blueAgents) {
        if (agent->isOnEnemySide() && !agent->isTagged()) {
            for (const auto& enemyAgent : redAgents) {
                if (enemyAgent->checkInTeamZone() && !enemyAgent->isTagged() && agent->distanceTo(enemyAgent.get()) <= taggingDistance) {
                    agent->setIsTagged(true);
                    break;
                }
            }
        }
    }

    for (const auto& agent : redAgents) {
        if (agent->isOnEnemySide() && !agent->isTagged()) {
            for (const auto& enemyAgent : blueAgents) {
                if (enemyAgent->checkInTeamZone() && !enemyAgent->isTagged() && agent->distanceTo(enemyAgent.get()) <= taggingDistance) {
                    agent->setIsTagged(true);
                    break;
                }
            }
        }
    }
}

QGraphicsItem* GameField::getAgentItem(Agent* agent) {
    qDebug() << "Looking for agent item with pointer with getAgentItem:" << agent;
    qDebug() << "Agent position with getAgentItem: (" << agent->getX() << ", " << agent->getY() << ")";
    qDebug() << "Agent side with getAgentItem:" << QString::fromStdString(agent->getSide());

    for (QGraphicsItem* item : scene->items()) {
        qDebug() << "Item data with getAgentItem:" << item->data(0).toString();
        if (item->data(0).value<quintptr>() == reinterpret_cast<quintptr>(agent)) {
            qDebug() << "Found matching item for agent with getAgentItem:" << agent;
            return item;
        }
    }

    qDebug() << "No matching item found for agent with getAgentItem:" << agent;
    return nullptr;
}

void GameField::updateSceneItems() {
    for (QGraphicsItem* item : scene->items()) {
        updateAgentItem(item, blueAgents, Qt::blue);
        updateAgentItem(item, redAgents, Qt::red);
    }
}

void GameField::updateAgentItem(QGraphicsItem* item, const std::vector<std::shared_ptr<Agent>>& agents, QColor color) {
    if (QGraphicsEllipseItem* agentItem = qgraphicsitem_cast<QGraphicsEllipseItem*>(item)) {
        for (const auto& agent : agents) {
            if (agentItem->brush().color() == color) {
                int x = agent->getX() * cellSize;
                int y = agent->getY() * cellSize;
                agentItem->setRect(x - 10, y - 10, 20, 20);
            }
        }
    }
}

void GameField::resetEnemyFlag(const QString& team) {
    if (team == "red") {
        // Reset the red flag position
        QGraphicsPolygonItem* redFlag = findFlagItem("red");
        if (redFlag) {
            QPolygon redTriangle;
            redTriangle << QPoint(710, 280) << QPoint(720, 300) << QPoint(700, 300);
            redFlag->setPolygon(redTriangle);
        }
        else {
            // Handle the case when the red flag item is not found in the scene
            qDebug() << "Red flag item not found in the scene";
        }

        // Reset the carrying flag state of blue agents
        for (const auto& agent : blueAgents) {
            agent->setCarryingFlag(false);
        }
    }
    else if (team == "blue") {
        // Reset the blue flag position
        QGraphicsPolygonItem* blueFlag = findFlagItem("blue");
        if (blueFlag) {
            QPolygon blueTriangle;
            blueTriangle << QPoint(70, 280) << QPoint(80, 300) << QPoint(60, 300);
            blueFlag->setPolygon(blueTriangle);
        }
        else {
            // Handle the case when the blue flag item is not found in the scene
            qDebug() << "Blue flag item not found in the scene";
        }

        // Reset the carrying flag state of red agents
        for (const auto& agent : redAgents) {
            agent->setCarryingFlag(false);
        }
    }
}

QGraphicsPolygonItem* GameField::findFlagItem(const QString& team) {
    for (QGraphicsItem* item : scene->items()) {
        if (QGraphicsPolygonItem* polygonItem = qgraphicsitem_cast<QGraphicsPolygonItem*>(item)) {
            if ((team == "red" && polygonItem->brush().color() == Qt::red) ||
                (team == "blue" && polygonItem->brush().color() == Qt::blue)) {
                return polygonItem;
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

std::pair<int, int> GameField::pixelToGrid(int pixelX, int pixelY) {
    int gridX = pixelX / cellSize;
    int gridY = pixelY / cellSize;
    return { gridX, gridY };
}

void GameField::setupScene() {
    scene = new QGraphicsScene(this);
    setScene(scene);
    // Calculate the game field width and height based on the grid dimensions
    gameFieldWidth = cols * cellSize;
    gameFieldHeight = rows * cellSize;

    // Set the scene rect to match the game field size
    setSceneRect(0, 0, gameFieldWidth, gameFieldHeight);

    // Create the grid based on the calculated rows and columns
    grid.resize(rows, std::vector<int>(cols, 0));
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Add the combined game field of blue area and red area
    this->gameField = new QGraphicsRectItem(5, 10, 790, 580);
    this->gameField->setPen(QPen(Qt::black, 2));
    scene->addItem(this->gameField);

    // Calculate the number of rows and columns based on the combined game field dimensions
    int gameFieldWidth = this->gameField->rect().width();
    int gameFieldHeight = this->gameField->rect().height();
    rows = std::ceil(gameFieldHeight / static_cast<double>(cellSize));
    cols = std::ceil(gameFieldWidth / static_cast<double>(cellSize));

    // Create the grid based on the calculated rows and columns
    grid.resize(rows, std::vector<int>(cols, 0));

    // Draw grid lines
    QPen gridPen(Qt::black, 1, Qt::DotLine);
    for (int i = 0; i <= rows; ++i) {
        QGraphicsLineItem* horizontalLine = new QGraphicsLineItem(0, i * cellSize, gameFieldWidth, i * cellSize);
        horizontalLine->setPen(gridPen);
        horizontalLine->setParentItem(this->gameField);

        // Add row numbers
        QGraphicsTextItem* rowNumber = new QGraphicsTextItem(QString::number(i));
        rowNumber->setDefaultTextColor(Qt::black);
        rowNumber->setFont(QFont("Arial", 10));
        rowNumber->setPos(-20, i * cellSize);
        scene->addItem(rowNumber);

    }
    for (int j = 0; j <= cols; ++j) {
        QGraphicsLineItem* verticalLine = new QGraphicsLineItem(j * cellSize, 0, j * cellSize, gameFieldHeight);
        verticalLine->setPen(gridPen);
        verticalLine->setParentItem(this->gameField);

        // Add column numbers
        QGraphicsTextItem* columnNumber = new QGraphicsTextItem(QString::number(j));
        columnNumber->setDefaultTextColor(Qt::black);
        columnNumber->setFont(QFont("Arial", 10));
        columnNumber->setPos(j * cellSize, -20);
        scene->addItem(columnNumber);
    }

    // Add team areas fields
    QGraphicsRectItem* blueArea = new QGraphicsRectItem(5, 10, 400, 580);
    blueArea->setPen(QPen(Qt::blue, 2));
    scene->addItem(blueArea);

    QGraphicsRectItem* redArea = new QGraphicsRectItem(410, 10, 390, 580);
    redArea->setPen(QPen(Qt::red, 2));
    scene->addItem(redArea);

    // Add the combined game field of blue area and red area
    QGraphicsRectItem* gameField = new QGraphicsRectItem(5, 10, 790, 580);
    gameField->setPen(QPen(Qt::black, 2));
    scene->addItem(gameField);

    // Add team zones (circular areas around flags)
    QGraphicsEllipseItem* blueZone = new QGraphicsEllipseItem(50, 260, 80, 80);
    QPen bluePen(Qt::blue);
    bluePen.setWidth(3);
    blueZone->setPen(bluePen);
    blueZone->setBrush(Qt::NoBrush);
    scene->addItem(blueZone);

    QGraphicsEllipseItem* redZone = new QGraphicsEllipseItem(690, 260, 80, 80);
    QPen redPen(Qt::red);
    redPen.setWidth(3);
    redZone->setPen(redPen);
    redZone->setBrush(Qt::NoBrush);
    scene->addItem(redZone);

    // Add flags
    QGraphicsPolygonItem* blueFlag = new QGraphicsPolygonItem();
    QPolygon blueTriangle;
    qreal blueFlagCenter = blueZone->rect().center().y();
    blueTriangle << QPoint(70, blueFlagCenter - 20) << QPoint(80, blueFlagCenter) << QPoint(60, blueFlagCenter);
    blueFlag->setPolygon(blueTriangle);
    blueFlag->setBrush(Qt::blue);
    scene->addItem(blueFlag);

    QGraphicsPolygonItem* redFlag = new QGraphicsPolygonItem();
    QPolygon redTriangle;
    qreal redFlagCenter = redZone->rect().center().y();
    redTriangle << QPoint(710, redFlagCenter - 20) << QPoint(720, redFlagCenter) << QPoint(700, redFlagCenter);
    redFlag->setPolygon(redTriangle);
    redFlag->setBrush(Qt::red);
    scene->addItem(redFlag);

    // Update the GameManager with the new flag positions
    QPointF blueFlagPosition = blueFlag->boundingRect().center();
    QPointF redFlagPosition = redFlag->boundingRect().center();

    std::pair<int, int> blueFlagGridPosition = pixelToGrid(blueFlagPosition.x(), blueFlagPosition.y());
    std::pair<int, int> redFlagGridPosition = pixelToGrid(redFlagPosition.x(), redFlagPosition.y());

    gameManager->setFlagPosition("blue", blueFlagGridPosition.first, blueFlagGridPosition.second);
    gameManager->setFlagPosition("red", redFlagGridPosition.first, redFlagGridPosition.second);

    qDebug() << "Blue flag grid position: (" << blueFlagGridPosition.first << ", " << blueFlagGridPosition.second << ")";
    qDebug() << "Red flag grid position: (" << redFlagGridPosition.first << ", " << redFlagGridPosition.second << ")";

    // Update the GameManager with the team zone positions
    QPointF blueZoneCenter = blueZone->rect().center();
    QPointF redZoneCenter = redZone->rect().center();

    std::pair<int, int> blueZoneGridPosition = pixelToGrid(blueZoneCenter.x(), blueZoneCenter.y());
    std::pair<int, int> redZoneGridPosition = pixelToGrid(redZoneCenter.x(), redZoneCenter.y());

    gameManager->setTeamZonePosition("blue", blueZoneGridPosition.first, blueZoneGridPosition.second);
    gameManager->setTeamZonePosition("red", redZoneGridPosition.first, redZoneGridPosition.second);

    // Spawn blue agents in a square formation on the top left of the blue side
    int blueStartX = 50;
    int blueStartY = 50;
    int blueSpacing = 30;

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            int x = blueStartX + j * blueSpacing;
            int y = blueStartY + i * blueSpacing;

            std::shared_ptr<Brain> blueBrain = std::make_shared<Brain>();
            std::shared_ptr<Memory> blueMemory = std::make_shared<Memory>();
            std::shared_ptr<Agent> blueAgent = std::make_shared<Agent>(x / cellSize, y / cellSize, "blue", cols, rows, grid, pathfinder, taggingDistance, blueBrain, blueMemory, gameManager, blueAgents, redAgents);
            blueAgents.push_back(blueAgent);

            grid[y / cellSize][x / cellSize] = 1;

            // Create the visual representation of the blue agent
            QGraphicsEllipseItem* blueAgentItem = new QGraphicsEllipseItem(x - 10, y - 10, 20, 20);
            blueAgentItem->setBrush(Qt::blue);
            blueAgentItem->setData(0, QVariant::fromValue(reinterpret_cast<quintptr>(blueAgent.get())));
            scene->addItem(blueAgentItem);

            // Connect signals for the blue agent
            connect(blueAgent.get(), &Agent::blueFlagCaptured, this, [this]() { handleFlagCapture("blue"); });
            connect(blueAgent.get(), &Agent::redFlagReset, this, [this]() { resetEnemyFlag("red"); });
        }
    }

    // Spawn red agents in a square formation on the bottom right of the red side
    int redStartX = 650;
    int redStartY = 450;
    int redSpacing = 30;

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            int x = redStartX + j * redSpacing;
            int y = redStartY + i * redSpacing;

            std::shared_ptr<Brain> redBrain = std::make_shared<Brain>();
            std::shared_ptr<Memory> redMemory = std::make_shared<Memory>();
            std::shared_ptr<Agent> redAgent = std::make_shared<Agent>((x - 600) / cellSize, (y - 400) / cellSize, "red", cols, rows, grid, pathfinder, taggingDistance, redBrain, redMemory, gameManager, blueAgents, redAgents);
            redAgents.push_back(redAgent);

            grid[(y - 400) / cellSize][(x - 600) / cellSize] = 1;

            // Create the visual representation of the red agent
            QGraphicsEllipseItem* redAgentItem = new QGraphicsEllipseItem(x - 10, y - 10, 20, 20);
            redAgentItem->setBrush(Qt::red);
            redAgentItem->setData(0, QVariant::fromValue(reinterpret_cast<quintptr>(redAgent.get())));
            scene->addItem(redAgentItem);

            // Connect signals for the red agent
            connect(redAgent.get(), &Agent::redFlagCaptured, this, [this]() { handleFlagCapture("red"); });
            connect(redAgent.get(), &Agent::blueFlagReset, this, [this]() { resetEnemyFlag("blue"); });
        }
    }
}

GameField::~GameField() {
    if (pathfinder) {
        delete pathfinder;
    }
    pathfinder = nullptr;

    // Assuming agents are now managed via shared_ptr, no need to delete them
    blueAgents.clear();
    redAgents.clear();

    if (gameManager) {
        delete gameManager;
    }
    gameManager = nullptr;
}
