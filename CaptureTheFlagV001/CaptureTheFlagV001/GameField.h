#ifndef GAMEFIELD_H
#define GAMEFIELD_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QTimer>
#include <QObject>
#include "Agent.h"
#include "Pathfinder.h"
#include "GameManager.h"
#include "FlagManager.h"
#include "TagManager.h"

class GameManager;
class FlagManager;
class TagManager;

class GameField : public QGraphicsView, public QObject {
	Q_OBJECT

public:
	GameField(QWidget* parent = nullptr);
	~GameField();

	int blueScore;
	int redScore;

	const std::vector<Agent*>& getBlueAgents() const { return blueAgents; }
	const std::vector<Agent*>& getRedAgents() const { return redAgents; }
	QGraphicsScene* getScene() const { return scene; }
	TagManager* getTagManager() const { return tagManager; }

private:
	QGraphicsScene* scene;
	std::vector<Agent*> blueAgents;
	std::vector<Agent*> redAgents;
	Pathfinder* pathfinder;
	int cellSize;
	int timeRemaining;
	QTimer* gameTimer;
	float taggingDistance;
	QGraphicsTextItem* timeRemainingTextItem;
	QGraphicsTextItem* blueScoreTextItem;
	QGraphicsTextItem* redScoreTextItem;
	GameManager* gameManager;
	FlagManager* flagManager;
	TagManager* tagManager;
	std::vector<std::vector<int>> grid;
	int rows;
	int cols;

	void initializeBlueAgents();
	void initializeRedAgents();
	void setupScene();
	void setupScoreDisplays();
};

#endif