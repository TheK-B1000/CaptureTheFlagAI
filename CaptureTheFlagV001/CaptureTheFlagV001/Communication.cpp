#include "Communication.h"
#include <queue>
#include <mutex>
#include <condition_variable>

std::queue<std::string> messageQueue;
std::mutex queueMutex;
std::condition_variable queueCondition;

void Communication::sendMessage(const std::string& message) {
    std::lock_guard<std::mutex> lock(queueMutex);
    messageQueue.push(message);
    queueCondition.notify_one();
}

void Communication::broadcastIntention(const std::string& intention) {
    std::lock_guard<std::mutex> lock(queueMutex);
    messageQueue.push("INTENTION:" + intention);
    queueCondition.notify_all();
}

std::vector<std::string> Communication::receiveIntentions() {
    std::vector<std::string> intentions;
    std::unique_lock<std::mutex> lock(queueMutex);
    while (!messageQueue.empty()) {
        std::string message = messageQueue.front();
        messageQueue.pop();
        if (message.substr(0, 10) == "INTENTION:") {
            intentions.push_back(message.substr(10));
        }
    }
    return intentions;
}

void Communication::broadcastStrategy(const std::string& strategy) {
    std::lock_guard<std::mutex> lock(queueMutex);
    messageQueue.push("STRATEGY:" + strategy);
    queueCondition.notify_all();
}