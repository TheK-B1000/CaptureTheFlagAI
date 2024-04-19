#include "CaptureTheFlagV001.h"
#include "GameField.h"

CaptureTheFlagV001::CaptureTheFlagV001(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    setCentralWidget(gameField);
}

CaptureTheFlagV001::~CaptureTheFlagV001()
{
    delete gameField;
}