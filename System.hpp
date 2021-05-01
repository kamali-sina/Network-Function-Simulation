#ifndef _SYSTEM_HPP
#define _SYSTEM_HPP

#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <iostream>
#include <vector>

#include "Frame.hpp"

class System{
    public:
    System(int system_number, int command_fd);
    int connect(int switch_number, int port_number);
    int get_number(){return system_number_;}
    int getCommandFd();
    int addConnection(int switch_number, int port_number);
    bool isConnected();
    void setConnected(){is_connected = true;}
    int send(int receiver_system_number);

    private:
    unsigned int system_number_;
    int command_fd_;
    int connected_switch_;
    int used_port_ = -1;
    bool is_connected=false;
};


#endif