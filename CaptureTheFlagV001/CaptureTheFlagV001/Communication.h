#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <string>
#include <vector>

class Communication {
public:
    void sendMessage(const std::string& message);
    void broadcastIntention(const std::string& intention);
    std::vector<std::string> receiveIntentions(); 
    void broadcastStrategy(const std::string& strategy); 
};

#endif 
