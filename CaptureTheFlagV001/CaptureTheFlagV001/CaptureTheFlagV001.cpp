#include "CaptureTheFlagV001.h"
#include "GameField.h"

CaptureTheFlagV001::CaptureTheFlagV001(QWidget* parent)
    : QMainWindow(parent), gameField(nullptr)
{
    ui.setupUi(this);

    int gameFieldWidth = 790;
    int gameFieldHeight = 580;
    int cellSize = 20;

    gameField = new GameField(this, gameFieldWidth, gameFieldHeight, cellSize);
    setCentralWidget(gameField);
}

CaptureTheFlagV001::~CaptureTheFlagV001()
{
    delete gameField;
}