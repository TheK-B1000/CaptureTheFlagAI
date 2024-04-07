#include "Pathfinder.h"
#include "GameField.h"
#include "Agent.h"
#include <QQueue>
#include <QSet>
#include <cmath>

struct Node {
    QPoint position;
    Node* parent;
    int g;
    int h;
    int f;

    Node(const QPoint& pos, Node* parent = nullptr)
        : position(pos), parent(parent), g(0), h(0), f(0) {}
};

Pathfinder::Pathfinder(GameField& gameField, QObject* parent)
    : QObject(parent), gameField(gameField)
{
}

QList<QPoint> Pathfinder::findPath(const QPoint& start, const QPoint& end)
{
    QList<QPoint> path;
    QSet<QPoint> closedSet;
    QQueue<Node*> openSet;

    Node* startNode = new Node(start);
    openSet.enqueue(startNode);

    while (!openSet.isEmpty()) {
        Node* currentNode = openSet.dequeue();
        closedSet.insert(currentNode->position);

        if (currentNode->position == end) {
            // Reconstruct the path
            while (currentNode != nullptr) {
                path.prepend(currentNode->position);
                currentNode = currentNode->parent;
            }
            break;
        }

        QList<QPoint> neighbors = getNeighbors(currentNode->position);
        for (const QPoint& neighbor : neighbors) {
            if (closedSet.contains(neighbor))
                continue;

            int tentativeG = currentNode->g + 1;
            bool tentativeIsBetter = false;

            Node* neighborNode = nullptr;
            for (Node* node : openSet) {
                if (node->position == neighbor) {
                    neighborNode = node;
                    break;
                }
            }

            if (neighborNode == nullptr) {
                neighborNode = new Node(neighbor, currentNode);
                tentativeIsBetter = true;
            }
            else if (tentativeG < neighborNode->g) {
                tentativeIsBetter = true;
            }

            if (tentativeIsBetter) {
                neighborNode->parent = currentNode;
                neighborNode->g = tentativeG;
                neighborNode->h = heuristic(neighbor, end);
                neighborNode->f = neighborNode->g + neighborNode->h;

                if (!openSet.contains(neighborNode))
                    openSet.enqueue(neighborNode);
            }
        }
    }

    // Clean up allocated memory
    while (!openSet.isEmpty())
        delete openSet.dequeue();

    return path;
}

QList<QPoint> Pathfinder::getNeighbors(const QPoint& position)
{
    QList<QPoint> neighbors;

    QList<QPoint> directions = {
        QPoint(0, -1),  // Up
        QPoint(0, 1),   // Down
        QPoint(-1, 0),  // Left
        QPoint(1, 0)    // Right
    };

    for (const QPoint& direction : directions) {
        QPoint neighbor = position + direction;
        if (isValidPosition(neighbor))
            neighbors.append(neighbor);
    }

    return neighbors;
}

bool Pathfinder::isValidPosition(const QPoint& position)
{
    // Check if the position is within the game field bounds
    if (!gameField.isWithinBounds(position))
        return false;

    // Check if the position is occupied by an obstacle
    if (isObstacle(position))
        return false;

    // Check if the position is occupied by another agent
    for (const Agent* agent : gameField.getBlueAgents()) {
        if (agent->getPosition() == position)
            return false;
    }

    for (const Agent* agent : gameField.getRedAgents()) {
        if (agent->getPosition() == position)
            return false;
    }

    return true;
}

bool Pathfinder::isObstacle(const QPoint& position) const {
    return false;
}

int Pathfinder::heuristic(const QPoint& position, const QPoint& end)
{
    return std::abs(position.x() - end.x()) + std::abs(position.y() - end.y());
}