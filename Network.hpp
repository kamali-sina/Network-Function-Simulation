#ifndef _NETWORK_HPP
#define _NETWORK_HPP

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include "System.hpp"
#include "Switch.hpp"

class Network{
    public:
    Network();
    int handleCommand(std::string input);
    std::vector<std::string> splitCommand(std::string input);
    int run();
    int mySwitch(std::vector<std::string> &splitted_command);
    int mySystem(std::vector<std::string> &splitted_command);
    int connect(std::vector<std::string> &splitted_command);
    int send(std::vector<std::string> &splitted_command);
    int recieve(std::vector<std::string> &splitted_command);
    int findSwitch(int switch_number);
    int findSystem(int system_number);

    private:
    std::vector<System> systems_;
    std::vector<Switch> switches_;
};


#endif