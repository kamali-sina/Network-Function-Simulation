#include "Switch.hpp"
#include <fcntl.h>

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
        std::cout << "Switch " << switch_number_ << ": Trying to open link to read." << std::endl;
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

        std::cout << "Switch " << switch_number_ << ": Trying to open link to write." << std::endl;
        fd = open(link.c_str(), O_WRONLY);
  
        int write_bytes = write(fd, send_message.c_str(), strlen(send_message.c_str()) + 1);
        if (write_bytes < 1) {
            std::cout << "Switch " << switch_number_ << ": Couldn't write a message to System " << system_number << ": " << message << std::endl;
        } else {
            write_flag = 1;
        }

        close(fd);
    }
    cout<<"Switch " << switch_number_ << ": Connect Complete"<<endl;

    if (this->updateLookupTable(system_number, port_number) < 1) {
        std::cout << "Switch " << switch_number_ << ": Port number " << port_number << "is already taken." << endl;
        return 0;
    }

    return 1;
}

int Switch::getCommandFd() {
    return command_fd_;
}

int Switch::updateLookupTable(int system_number, int port_number) {
    for (int system_index = 0; system_index < this->lookup_table_.size(); system_index++) {
        if (port_number == this->lookup_table_[system_index].port_number) {
            return 0;
        }
    }

    SystemInfo system;
    system.system_number = system_number;
    system.port_number = port_number;

    this->lookup_table_.push_back(system);

    return 1;
}

int Switch::receive() {
    for (int system_index = 0; system_index < this->lookup_table_.size(); system_index++) {
        int system_number = this->lookup_table_[system_index].system_number;
        int port_number = this->lookup_table_[system_index].port_number;

        string link = "link_" + to_string(system_number) + "_" + to_string(switch_number_) + "_" + to_string(port_number);

        // cout << "Switch " << switch_number_ << ": " << link << endl;
        
        size_t message_size = 128;
        char message[message_size];

        // cout << "Switch " << switch_number_ << ": Trying to opne link to read." << endl;
        int fd = open(link.c_str(), O_RDONLY | O_NONBLOCK);

        int read_bytes = read(fd, message, message_size);
        if (read_bytes > 0) {
            cout << "Switch " << switch_number_ << ": Message from System " << system_number << ": " << message << endl;
            memset(message, 0, message_size);
        }
        
        close(fd);
    }

    return 1;
}
