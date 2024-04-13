#include "CaptureTheFlagV001.h"
#include "GameField.h"

CaptureTheFlagV001::CaptureTheFlagV001(QWidget* parent)
    : QMainWindow(parent), gameField(nullptr)
{
    ui.setupUi(this);

    int rows = 10;
    int cols = 10;
    std::vector<std::vector<int>> grid(rows, std::vector<int>(cols, 0));

    gameField = new GameField(this, grid);
    setCentralWidget(gameField);
}

CaptureTheFlagV001::~CaptureTheFlagV001()
{
    delete gameField;
}