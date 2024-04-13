#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_CaptureTheFlagV001.h"

class GameField;

class CaptureTheFlagV001 : public QMainWindow
{
    Q_OBJECT

public:
    CaptureTheFlagV001(QWidget* parent = nullptr);
    ~CaptureTheFlagV001();

private:
    Ui::CaptureTheFlagV001Class ui;
    GameField* gameField;
};
