#ifndef _SWITCH_HPP
#define _SWITCH_HPP

#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <iostream>
#include <vector>

struct SystemInfo{
    int system_number;
    int port_number;
};

class Switch{
    public:
    Switch(int number_of_ports, int switch_number, int command_fd);
    int connect(int system_number, int port_number);
    int get_number(){return switch_number_;}
    int getCommandFd();
    int updateLookupTable(int system_number, int port_number);

    private:
    unsigned int number_of_ports_;
    unsigned int switch_number_;
    std::vector<SystemInfo> lookup_table_;
    int command_fd_;
};

#endif