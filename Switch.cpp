#include "Switch.hpp"

using namespace std;

Switch::Switch(int number_of_ports, int switch_number, int command_fd){
    number_of_ports_ = number_of_ports;
    switch_number_ = switch_number;
    command_fd_ = command_fd;
}

int Switch::connect(int system_number, int port_number){
    string link = "link_" + to_string(system_number) + "_" + to_string(switch_number_) + "_" + to_string(port_number);

    std::string send_message = "Hello from Switch " + to_string(switch_number_) + ".";
    
    size_t message_size = 100;
    char message[message_size];

    int read_flag, write_flag = 0;

    while (!(read_flag && write_flag))
    {
        std::cout << "Switch " << switch_number_ << ": Trying to opne link to read." << std::endl;
        int fd = open(link.c_str(), O_RDONLY);
  
        int read_bytes = read(fd, message, message_size);
        if (read_bytes > 1) {
            std::cout << "Switch " << switch_number_ << ": Message from System " << system_number << ": " << message << std::endl;
            memset(message, 0, message_size);
            read_flag = 1;
        } else {
            std::cout << "Switch " << switch_number_ << ": Didn't recieve message from System " << system_number << "." << std::endl;
        }
        
        close(fd);

        std::cout << "Switch " << switch_number_ << ": Trying to opne link to write." << std::endl;
        fd = open(link.c_str(), O_WRONLY);
  
        int write_bytes = write(fd, send_message.c_str(), strlen(send_message.c_str()) + 1);
        if (write_bytes < 1) {
            std::cout << "Switch " << switch_number_ << ": Couldn't write a message to System " << system_number << ": " << message << std::endl;
        } else {
            write_flag = 1;
        }

        close(fd);
    }


    return 1;
}

int Switch::getCommandFd() {
    return command_fd_;
}
