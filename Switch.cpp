#include "Switch.hpp"

using namespace std;

Switch::Switch(int number_of_ports, int switch_number, int command_fd){
    number_of_ports_ = number_of_ports;
    switch_number_ = switch_number;
    command_fd_ = command_fd;
}

int Switch::connect(int system_number, int port_number){
    // TODO: do something here!
    return 1;
}

int Switch::getCommandFd() {
    return command_fd_;
}