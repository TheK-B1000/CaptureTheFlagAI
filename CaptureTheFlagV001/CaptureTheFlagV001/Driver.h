#ifndef DRIVER_H
#define DRIVER_H

#include <QMainWindow>
#include <QMenu>

class GameField;

class Driver : public QMainWindow {
    Q_OBJECT

public:
    explicit Driver(QWidget* parent = nullptr);

private slots:
    void runTestCase1();
    void runTestCase2();
    void runTestCase3();

private:
    GameField* gameField = nullptr;
    QMenu* testCaseMenu;
};

#endif