#include "System.hpp"

using namespace std;

System::System(int system_number, int command_fd){
    system_number_ = system_number;
    command_fd_ = command_fd;
}

int System::connect(int switch_number, int port_number){
    string link = "link_" + to_string(system_number_) + "_" + to_string(switch_number) + "_" + to_string(port_number);

    std::string send_message = "Hello from System " + to_string(system_number_) + ".";
    
    size_t message_size = 100;
    char message[message_size];

    int write_flag, read_flag = 0;

    while (!(read_flag && write_flag))
    {
        std::cout << "System " << system_number_ << ": Trying to opne link to write." << std::endl;
        int fd = open(link.c_str(), O_WRONLY);
  
        int write_bytes = write(fd, send_message.c_str(), strlen(send_message.c_str()) + 1);
        if (write_bytes < 1) {
            std::cout << "System " << system_number_ << ": Couldn't write a message to Switch " << switch_number << ": " << message << std::endl;
        } else {
            write_flag = 1;
        }

        close(fd);
  
        std::cout << "System " << system_number_ << ": Trying to opne link to read." << std::endl;
        fd = open(link.c_str(), O_RDONLY);
  
        int read_bytes = read(fd, message, message_size);
        if (read_bytes > 1) {
            std::cout << "System " << system_number_ << ": Message from Switch " << switch_number << ": " << message << std::endl;
            memset(message, 0, message_size);
            read_flag = 1;
        } else {
            std::cout << "System " << system_number_ << ": Dind't recieve message from Switch " << switch_number << "." << std::endl;
        }

        close(fd);
    }

    this->addConnection(switch_number, port_number);

    return 1;
}

int System::getCommandFd() {
    return command_fd_;
}

int System::addConnection(int switch_number, int port_number) {
    this->connected_siwtch_ = switch_number;
    this->used_port_ = port_number;

    return 1;
}

bool System::isConnected() {
    if (this->used_port_ < 0) {
        return false;
    }

    return true;
}

int System::send(int receiver_system_number) {
    int switch_number = this->connected_siwtch_;
    int port_number = this->used_port_;
    string link = "link_" + to_string(system_number_) + "_" + to_string(switch_number) + "_" + to_string(port_number);

    string message = "Sending a Hello from System " + to_string(system_number_) + " to " + to_string(receiver_system_number) + ".";
    Frame frame(this->system_number_, receiver_system_number, message);
    
    string frame_string = frame.getFrameString();

    cout << "System " << system_number_ << ": Frame: " << frame_string << endl;

    cout << "System " << system_number_ << ": Trying to opne link to write." << endl;
    int fd = open(link.c_str(), O_WRONLY);

    while (true) {
        int write_bytes = write(fd, frame_string.c_str(), strlen(frame_string.c_str()) + 1);
        if (write_bytes < 1) {
            cout << "System " << system_number_ << ": Couldn't write a message to Switch " << switch_number << "." << endl;
        } else {
            break;
        }
    }

    close(fd);

    return 1;
}
