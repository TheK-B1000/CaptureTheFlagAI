#include "Driver.h"
#include "GameField.h"
#include <QMenuBar>
#include <QAction>
#include <QInputDialog>

Driver::Driver(QWidget* parent) : QMainWindow(parent), gameField(nullptr) {
    int rows = 30;
    int cols = 40;
    std::vector<std::vector<int>> grid(rows, std::vector<int>(cols, 0));

    gameField = new GameField(this, grid);
    gameManager = gameField->getGameManager();

    setCentralWidget(gameField);

    // Create the menu bar
    QMenuBar* menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    // Create the "Test Cases" menu
    testCaseMenu = menuBar->addMenu("Test Cases");

    // Add actions for each test case
    QAction* testCase1Action = new QAction("Test Case 1: Default Setup", this);
    connect(testCase1Action, &QAction::triggered, this, &Driver::runTestCase1);
    testCaseMenu->addAction(testCase1Action);

    QAction* testCase2Action = new QAction("Test Case 2: Change Agent Count", this);
    connect(testCase2Action, &QAction::triggered, this, &Driver::runTestCase2);
    testCaseMenu->addAction(testCase2Action);

    QAction* testCase3Action = new QAction("Test Case 3: Change Team Zone Positions", this);
    connect(testCase3Action, &QAction::triggered, this, &Driver::runTestCase3);
    testCaseMenu->addAction(testCase3Action);
}

void Driver::runTestCase1() {
    gameManager->resetGame();
    gameField->runTestCase1();
}

void Driver::runTestCase2() {
    gameManager->resetGame();
    bool ok;
    int agentCount = QInputDialog::getInt(this, "Test Case 2", "Enter the number of agents:", 8, 1, 100, 1, &ok);
    if (ok) {
        gameField->runTestCase2(agentCount, gameField->getGameManager());
    }
}

void Driver::runTestCase3() {
    gameManager->resetGame();
    gameField->runTestCase3();
}