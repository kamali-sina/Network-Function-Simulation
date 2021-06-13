#ifndef _SWITCH_HPP
#define _SWITCH_HPP

#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <iostream>
#include <vector>

#include "Frame.hpp"

#define ROUTER 0
#define SYSTEM 1
#define SERVER 2

struct DeviceInfo
{
    int device_number = -1;
    int port_number = -1;
    std::string IP_address_;
    int type = ROUTER; // Router = 0
    bool connector;
};

class Switch{
    public:
    Switch(std::string ip, int switch_number, int command_fd);
    int connect(int system_number, int port_number);
    int get_number(){return switch_number_;}
    int getCommandFd();
    int receive();
    int addToConnectedTable(int system_number, int index_number);
    int connectSwitch(int switch_number, int port_number);
    int acceptConnectSwitch(int switch_number, int port_number);
    int addToSwitchesConnectedTable(int switch_number, int index_number, bool connector);
    bool isSystemNumberInLookupTable(int system_number);
    int getPortNumberFromLookupTable(int system_number);
    int send(Frame frame);
    int broadcast(Frame frame);
    int broadcastToSystems(Frame frame);
    int broadcastToSwitches(Frame frame);
    int receiveSwitch();
    int unlinkSwitch(int switch_number);
    std::string getIP() { return IP_; }
    bool hasInLookupTable(DeviceInfo device_info);
    int updateLookupTable(DeviceInfo device_info);
    void printLookupTable();    
    std::vector<DeviceInfo> getLookupTable() { return lookup_table_;}
    int updateLookupTable(std::vector<DeviceInfo> lookup_table, std::string IP);
    private:
    unsigned int number_of_ports_=10;
    unsigned int switch_number_;
    std::string IP_;
    std::vector<DeviceInfo> lookup_table_;
    int command_fd_;
    std::vector<DeviceInfo> connected_systems_table_;
    std::vector<DeviceInfo> connected_switches_table_;
};

#endif
