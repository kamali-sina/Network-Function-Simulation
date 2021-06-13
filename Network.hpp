#ifndef _NETWORK_HPP
#define _NETWORK_HPP

#include <string.h>

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include "System.hpp"
#include "Switch.hpp"
#include "Frame.hpp"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/wait.h>
#include <fcntl.h> 

#define MAX_NUMBER_OF_THREADS 100
#define READ 0
#define WRITE 1

void* switchThread(void* switch_class);
void* systemThread(void* system_class);
struct switch_link{
    int switch1;
    int switch2;
};


class Network{
    public:
    Network();
    int handleCommand(std::string input);
    std::vector<std::string> splitCommand(std::string input);
    int run();
    int mySwitch(std::vector<std::string> &splitted_command);
    int mySystem(std::vector<std::string> &splitted_command);
    int connect(int system_number, int switch_number, int port_number, DeviceInfo device_info);
    int send(int sender_system_number, int receiver_system_number);
    int sendToGroup(std::vector<std::string> &splitted_command);
    int receive(std::vector<std::string> &splitted_command);
    int SpanningTree(std::vector<std::string> &splitted_command);
    int findSwitch(int switch_number);
    int findSystem(int system_number);
    int isSystemAvailable(int system_number);
    int connectSwitches(std::vector<std::string> &splitted_command);
    int createNamePipe(std::string link_name);
    int getGroupList(std::vector<std::string> &splitted_command);
    int group(std::vector<std::string> &splitted_command);
    int joinGroup(std::vector<std::string> &splitted_command);
    int findGroup(std::string group_ip);
    int findRouterIDByIP(std::string ip);

    int updateLookupTable(int router_index, DeviceInfo device_info);
    void printLookupTable();

    private:
    bool checkAddVisited(std::vector<std::vector<int>> &tree_holder, switch_link swtch_link);
    int findPathIndex(std::vector<std::vector<int>> &tree_holder, int node_);
    void removeLink(switch_link swtch_link);
    std::vector<System> systems_;
    std::vector<int> systems_command_fd_;
    std::vector<Switch> switches_;
    std::vector<int> switch_command_fd_;
    std::vector<switch_link> links;
    std::vector<std::vector<int> > groups_;
    std::vector<std::string> group_ips_;
    std::vector<std::vector<int> > connected_routers_;
    std::vector<std::vector<int> > client_connected_routers_;
};


#endif