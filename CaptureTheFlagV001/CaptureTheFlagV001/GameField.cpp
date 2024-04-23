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
    setupAgents(4, 4, cols, gameManager);

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
    for (Agent* agent : blueAgents) {
        if (agent) {
            if (agent->getBrain()) {
                delete agent->getBrain();
            }
            if (agent->getMemory()) {
                delete agent->getMemory();
            }
            delete agent;
            agent = nullptr; 
        }
    }

    for (Agent* agent : redAgents) {
        if (agent) {
            if (agent->getBrain()) {
                delete agent->getBrain();
            }
            if (agent->getMemory()) {
                delete agent->getMemory();
            }
            delete agent;
            agent = nullptr;
        }
    }
    redAgents.clear();
}

void GameField::setupAgents(int blueCount, int redCount, int cols, GameManager* gameManager) {
    qDebug() << "setupAgents called with blueCount:" << blueCount << "and redCount:" << redCount;
    // Initialize blue agents
    for (int i = 0; i < blueCount; i++) {
        int x, y;
        bool validPosition = false;
        while (!validPosition) {
            x = QRandomGenerator::global()->bounded(1, cols / 2 - 1); 
            y = QRandomGenerator::global()->bounded(1, rows - 2); 
            if (x > 0 && x < cols - 1 && y > 0 && y < rows - 1 && grid[y][x] != 1) {
                validPosition = true;
            }
        }
        Brain* blueBrain = new Brain();
        if (!blueBrain) {
            qDebug() << "Failed to allocate memory for Brain object";
        }

        Memory* blueMemory = new Memory();
        if (!blueMemory) {
            qDebug() << "Failed to allocate memory for Memory object";
            delete blueBrain;
        }

        Agent* agent = new Agent(x, y, "blue", cols, grid, rows, pathfinder, taggingDistance, blueBrain, blueMemory, gameManager, blueAgents, redAgents);
        blueAgents.push_back(agent);
        grid[y][x] = 1;

        connect(agent, &Agent::redFlagCaptured, this, [this]() { handleFlagCapture("red"); });
        connect(agent, &Agent::blueFlagReset, this, [this]() { resetEnemyFlag("blue"); });
    }

    // Initialize red agents
    for (int i = 0; i < redCount; i++) {
        int x, y;
        bool validPosition = false;
        while (!validPosition) {
            x = QRandomGenerator::global()->bounded(cols / 2, cols - 2);
            y = QRandomGenerator::global()->bounded(1, rows - 2);
            if (x > 0 && x < cols - 1 && y > 0 && y < rows - 1 && grid[y][x] != 1) {
                validPosition = true;
            }
        }
        Brain* redBrain = new Brain();
        if (!redBrain) {
            qDebug() << "Failed to allocate memory for Brain object";
        }

        Memory* redMemory = new Memory();
        if (!redMemory) {
            qDebug() << "Failed to allocate memory for Memory object";
            delete redBrain;
        }

        Agent* agent = new Agent(x, y, "red", cols, grid, rows, pathfinder, taggingDistance, redBrain, redMemory, gameManager, blueAgents, redAgents);
        redAgents.push_back(agent);
        grid[y][x] = 1;

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
    int gameDuration = 300; // 5 minutes in seconds
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

        updateAgentItemsPositions();
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

void GameField::updateAgents() {
    qDebug() << "Updating agent positions"; 
    updateAgentPositions();

    qDebug() << "Checking tagging"; 
    checkTagging();

    qDebug() << "Updating agent item positions"; 
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
        int prevX = agent->getX();
        int prevY = agent->getY();

        // Update the agent's position
        agent->update(redPositions, redAgents);

        int newX = agent->getX();
        int newY = agent->getY();

        // Check if the agent's position is within the game field boundaries
        if (newX >= 0 && newX < cols && newY >= 0 && newY < rows) {
            // Clear the previous position in the grid
            grid[prevY][prevX] = 0;

            // Mark the new position as occupied
            grid[newY][newX] = 1;

            bluePositions.emplace_back(newX, newY);
        }
        else {
            // If the new position is outside the boundaries, revert to the previous position
            agent->setX(prevX);
            agent->setY(prevY);
            bluePositions.emplace_back(prevX, prevY);
        }
    }

    // Print blue agent positions for debugging
    qDebug() << "Blue agent positions:";
    for (const auto& pos : bluePositions) {
        qDebug() << "(" << pos.first << ", " << pos.second << ")";
    }

    for (Agent* agent : redAgents) {
        int prevX = agent->getX();
        int prevY = agent->getY();

        // Update the agent's position
        agent->update(bluePositions, blueAgents);

        int newX = agent->getX();
        int newY = agent->getY();

        // Check if the agent's position is within the game field boundaries
        if (newX >= 0 && newX < cols && newY >= 0 && newY < rows) {
            // Update the grid representation only if the new position is valid
            grid[prevY][prevX] = 0; // Mark the previous position as empty
            grid[newY][newX] = 1; // Mark the new position as occupied

            redPositions.emplace_back(newX, newY);
        }
        else {
            // If the new position is outside the boundaries, revert to the previous position
            agent->setX(prevX);
            agent->setY(prevY);
            redPositions.emplace_back(prevX, prevY);
        }
    }

    // Print red agent positions for debugging
    qDebug() << "Red agent positions:";
    for (const auto& pos : redPositions) {
        qDebug() << "(" << pos.first << ", " << pos.second << ")";
    }
}

void GameField::updateAgentItemsPositions() {
    for (Agent* agent : blueAgents) {
        QGraphicsItem* item = getAgentItem(agent);
        if (item) {
            qDebug() << "Blue agent at (" << agent->getX() << ", " << agent->getY() << ") - Item found with updateAgentItemsPositions";
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
            qDebug() << "Agent item not found in the scene for blue agent at position (" << agent->getX() << ", " << agent->getY() << ") with updateAgentItemsPositions";
        }
    }

    for (Agent* agent : redAgents) {
        QGraphicsItem* item = getAgentItem(agent);
        if (item) {
            qDebug() << "Red agent at (" << agent->getX() << ", " << agent->getY() << ") - Item found with updateAgentItemsPositions";
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
            qDebug() << "Agent item not found in the scene for red agent at position (" << agent->getX() << ", " << agent->getY() << ") with updateAgentItemsPositions";
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
        for (Agent* agent : blueAgents) {
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
        for (Agent* agent : redAgents) {
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

    // Update the GameManager with the team zone positions
    QPointF blueZoneCenter = blueZone->rect().center();
    QPointF redZoneCenter = redZone->rect().center();

    std::pair<int, int> blueZoneGridPosition = pixelToGrid(blueZoneCenter.x(), blueZoneCenter.y());
    std::pair<int, int> redZoneGridPosition = pixelToGrid(redZoneCenter.x(), redZoneCenter.y());

    gameManager->setTeamZonePosition("blue", blueZoneGridPosition.first, blueZoneGridPosition.second);
    gameManager->setTeamZonePosition("red", redZoneGridPosition.first, redZoneGridPosition.second);

    // Add agents
    for (Agent* agent : blueAgents) {
        int col = agent->getX();
        int row = agent->getY();
        int x = col * cellSize;
        int y = row * cellSize;

        // Check if the agent's position is within the valid range
        if (col >= 0 && col < cols / 2 && row >= 0 && row < rows) {
            QGraphicsEllipseItem* blueAgent = new QGraphicsEllipseItem(x - 10, y - 10, 20, 20);
            blueAgent->setBrush(Qt::blue);
            blueAgent->setData(0, QVariant::fromValue(reinterpret_cast<quintptr>(agent)));
            scene->addItem(blueAgent);
            qDebug() << "Blue agent (SetupScene) added at position (" << x << ", " << y << ")";
            qDebug() << "Agent pointer from SetupScene:" << agent;
            qDebug() << "Item data from SetupScene:" << blueAgent->data(0).value<quintptr>();
        }
        else {
            qDebug() << "Blue agent position (" << col << ", " << row << ") is outside the valid range";
        }
    }

    for (Agent* agent : redAgents) {
        int col = agent->getX();
        int row = agent->getY();
        int x = col * cellSize;
        int y = row * cellSize;

        // Check if the agent's position is within the valid range
        if (col >= cols / 2 && col < cols && row >= 0 && row < rows) {
            QGraphicsEllipseItem* redAgent = new QGraphicsEllipseItem(x - 10, y - 10, 20, 20);
            redAgent->setBrush(Qt::red);
            redAgent->setData(0, QVariant::fromValue(reinterpret_cast<quintptr>(agent)));
            scene->addItem(redAgent);
            qDebug() << "Red agent (SetupScene) added at position (" << x << ", " << y << ")";
            qDebug() << "Agent pointer from SetupScene:" << agent;
            qDebug() << "Item data from SetupScene:" << redAgent->data(0).value<quintptr>();
        }
        else {
            qDebug() << "Red agent position (" << col << ", " << row << ") is outside the valid range";
        }
    }

    qDebug() << "Number of blue agent items added:" << blueAgents.size();
    qDebug() << "Number of red agent items added:" << redAgents.size();

    // Adjust agent positions if they are outside the game field boundaries
    for (Agent* agent : blueAgents) {
        int x = agent->getX();
        int y = agent->getY();
        if (x < 0 || x >= cols || y < 0 || y >= rows) {
            do {
                x = QRandomGenerator::global()->bounded(1, cols / 2 - 2);
                y = QRandomGenerator::global()->bounded(1, rows - 1);
            } while (grid[y][x] != 0);
            agent->setX(x);
            agent->setY(y);
        }
    }

    for (Agent* agent : redAgents) {
        int x = agent->getX();
        int y = agent->getY();
        if (x < 0 || x >= cols || y < 0 || y >= rows) {
            do {
                x = QRandomGenerator::global()->bounded(cols / 2 + 1, cols - 2);
                y = QRandomGenerator::global()->bounded(1, rows - 1);
            } while (grid[y][x] != 0);
            agent->setX(x);
            agent->setY(y);
        }
    }
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

    blueAgents.clear();
    redAgents.clear();

    delete gameManager;
}