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

GameField::GameField(QWidget* parent, int width, int height)
    : QGraphicsView(parent), gameFieldWidth(0), gameFieldHeight(0) {
    setRenderHint(QPainter::Antialiasing);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Set the game field size
    gameFieldWidth = 800;
    gameFieldHeight = 600;

    // Initialize the GameManager
    gameManager = new GameManager(gameFieldWidth, gameFieldHeight);

    // Create the Pathfinder object
    pathfinder = new Pathfinder(gameFieldWidth, gameFieldHeight);

    // Set up the agents before setting up the scene
    //setupAgents(4, 4, gameFieldWidth, gameFieldHeight, gameManager);

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

void GameField::setupAgents(int blueCount, int redCount, int gameFieldWidth, int gameFieldHeight, GameManager* gameManager) {
    // Initialize blue agents
    for (int i = 0; i < blueCount; i++) {
        int x, y;
        bool validPosition = false;

        while (!validPosition) {
            x = QRandomGenerator::global()->bounded(0, gameFieldWidth / 2);
            y = QRandomGenerator::global()->bounded(0, gameFieldHeight);

            // Check if the position is valid (not occupied by other agents)
            validPosition = true; // Assuming all positions are valid for now
        }

        // Construct the blue agent and add to the list
        auto blueBrain = std::make_shared<Brain>();
        auto blueMemory = std::make_shared<Memory>();
        auto agent = std::make_shared<Agent>(x, y, "blue", gameFieldWidth, gameFieldHeight, pathfinder, taggingDistance, blueBrain, blueMemory, gameManager, blueAgents, redAgents);
        blueAgents.push_back(agent);

        // Connect signals
        connect(agent.get(), &Agent::redFlagCaptured, this, [this]() { handleFlagCapture("red"); });
        connect(agent.get(), &Agent::blueFlagReset, this, [this]() { resetEnemyFlag("blue"); });
    }

    // Initialize red agents
    for (int i = 0; i < redCount; i++) {
        int x, y;
        bool validPosition = false;

        while (!validPosition) {
            x = QRandomGenerator::global()->bounded(gameFieldWidth / 2, gameFieldWidth);
            y = QRandomGenerator::global()->bounded(0, gameFieldHeight);

            // Check if the position is valid (not occupied by other agents)
            validPosition = true; // Assuming all positions are valid for now
        }

        // Construct the red agent and add to the list
        auto redBrain = std::make_shared<Brain>();
        auto redMemory = std::make_shared<Memory>();
        auto agent = std::make_shared<Agent>(x, y, "red", gameFieldWidth, gameFieldHeight, pathfinder, taggingDistance, redBrain, redMemory, gameManager, blueAgents, redAgents);
        redAgents.push_back(agent);

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
    setupAgents(blueCount, redCount, gameFieldWidth, gameFieldHeight, gameManager);
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

    updateAgentPositions();
    checkTagging();

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

    if (timeRemaining <= 0) {
        stopGame();
        declareWinner();
    }
    else {
        // Update agents every N iterations to reduce excessive updating
        const int updateFrequency = 5; // Adjust this value based on your game's requirements
        static int updateCounter = 0;

        if (updateCounter % updateFrequency == 0) {
            updateAgents();
        }

        updateCounter++;
    }
}

void GameField::stopGame() {
    gameTimer->stop();

    for (const auto& agent : blueAgents) {
        agent->setEnabled(false);
    }

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
        bluePositions.emplace_back(agent->getX(), agent->getY());
    }

    for (const auto& agent : redAgents) {
        QGraphicsItem* item = getAgentItem(agent.get());
        updateAgentItemPositions(item, agent, agent->getX(), agent->getY());
        redPositions.emplace_back(agent->getX(), agent->getY());
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
        item->setPos(x, y);

        QGraphicsEllipseItem* agentItem = qgraphicsitem_cast<QGraphicsEllipseItem*>(item);
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
                int x = agent->getX();
                int y = agent->getY();
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

void GameField::setupScene() {
    scene = new QGraphicsScene(this);
    setScene(scene);

    // Set the scene rect to match the game field size
    setSceneRect(0, 0, 800, 600);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Add the combined game field of blue area and red area
    this->gameField = new QGraphicsRectItem(5, 10, 790, 580);
    this->gameField->setPen(QPen(Qt::black, 2));
    scene->addItem(this->gameField);

    // Add team areas fields
    QGraphicsRectItem* blueArea = new QGraphicsRectItem(5, 10, 400, 580);
    blueArea->setPen(QPen(Qt::blue, 2));
    scene->addItem(blueArea);

    QGraphicsRectItem* redArea = new QGraphicsRectItem(410, 10, 390, 580);
    redArea->setPen(QPen(Qt::red, 2));
    scene->addItem(redArea);

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

    // Update the GameManager with the flag positions
    QPointF blueFlagPosition = blueFlag->boundingRect().center();
    QPointF redFlagPosition = redFlag->boundingRect().center();

    gameManager->setFlagPosition("blue", blueFlagPosition.x(), blueFlagPosition.y());
    gameManager->setFlagPosition("red", redFlagPosition.x(), redFlagPosition.y());

    // Update the GameManager with the team zone positions
    QPointF blueZoneCenter = blueZone->rect().center();
    QPointF redZoneCenter = redZone->rect().center();

    gameManager->setTeamZonePosition("blue", blueZoneCenter.x(), blueZoneCenter.y());
    gameManager->setTeamZonePosition("red", redZoneCenter.x(), redZoneCenter.y());

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
            auto blueAgent = std::make_shared<Agent>(x, y, "blue", gameFieldWidth, gameFieldHeight, pathfinder, taggingDistance, blueBrain, blueMemory, gameManager, blueAgents, redAgents);
            blueAgents.push_back(blueAgent);

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
            auto redAgent = std::make_shared<Agent>(x, y, "red", gameFieldWidth, gameFieldHeight, pathfinder, taggingDistance, redBrain, redMemory, gameManager, blueAgents, redAgents);
            redAgents.push_back(redAgent);

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
