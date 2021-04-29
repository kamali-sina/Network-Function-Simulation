#include "System.hpp"

using namespace std;

System::System(int system_number, int command_fd){
    system_number_ = system_number;
    command_fd_ = command_fd;
}

int System::connect(int switch_number, int port_number){
    // TODO: do something here!
    return 1;
}

int System::getCommandFd() {
    return command_fd_;
}
