#ifndef _NETWORK_HPP
#define _NETWORK_HPP

#include <vector>
#include <string>
#include <pthread.h>
#include <iostream>
#include <sstream>
#include "System.hpp"
#include "Switch.hpp"

#define MAX_NUMBER_OF_THREADS 100

void* switchThread(void* switch_class);
void* systemThread(void* system_class);

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
    std::vector<pthread_t> threads;
    pthread_mutex_t mutex_lock;
};


#endif