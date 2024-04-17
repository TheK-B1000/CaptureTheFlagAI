#ifndef DRIVER_H
#define DRIVER_H

#include <QMainWindow>
#include <QMenu>

class GameField;
class GameManager; 

class Driver : public QMainWindow {
    Q_OBJECT

public:
    explicit Driver(QWidget* parent = nullptr);

private slots:
    void runTestCase1();
    void runTestCase2();
    void runTestCase3();

private:
    QMenu* testCaseMenu;
    GameField* gameField;
    GameManager* gameManager;  
};

#endif