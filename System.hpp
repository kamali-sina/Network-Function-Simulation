#ifndef _SYSTEM_HPP
#define _SYSTEM_HPP

#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <iostream>

class System{
    public:
    System(int system_number, int command_fd);
    int connect(int switch_number, int port_number);
    int get_number(){return system_number_;}
    int getCommandFd();

    private:
    unsigned int system_number_;
    int command_fd_;
};


#endif