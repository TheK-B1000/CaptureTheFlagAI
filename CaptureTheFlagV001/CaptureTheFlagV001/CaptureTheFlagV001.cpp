#include "CaptureTheFlagV001.h"
#include "GameField.h"  

CaptureTheFlagV001::CaptureTheFlagV001(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    GameField* gameField = new GameField(this);  
    setCentralWidget(gameField);  
}

CaptureTheFlagV001::~CaptureTheFlagV001()
{

}
