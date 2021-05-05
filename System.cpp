#include "System.hpp"

using namespace std;

System::System(int system_number, int command_fd){
    system_number_ = system_number;
    command_fd_ = command_fd;
}

int System::connect(int switch_number, int port_number){
    string link = "link_" + to_string(system_number_) + "_" + to_string(switch_number) + "_" + to_string(port_number);

    std::string link_r = "r_" + link;
    std::string link_w = "w_" + link;

    std::string send_message = "Hello from System " + to_string(system_number_) + ".";
    
    size_t message_size = 129;
    char message[message_size];

    int write_flag, read_flag = 0;

    while (!(read_flag && write_flag))
    {
        std::cout << "System " << system_number_ << ": Trying to open link to write." << std::endl;
        int fd = open(link_w.c_str(), O_WRONLY);
  
        int write_bytes = write(fd, send_message.c_str(), strlen(send_message.c_str()) + 1);
        if (write_bytes < 1) {
            std::cout << "System " << system_number_ << ": Couldn't write a message to Switch " << switch_number << ": " << message << std::endl;
        } else {
            write_flag = 1;
        }

        close(fd);
  
        std::cout << "System " << system_number_ << ": Trying to open link to read." << std::endl;
        fd = open(link_r.c_str(), O_RDONLY);
  
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
    this->connected_switch_ = switch_number;
    this->used_port_ = port_number;

    return 1;
}

bool System::isConnected() {
    return is_connected;
}

int System::send(int receiver_system_number) {
    int switch_number = this->connected_switch_;
    int port_number = this->used_port_;
    string link = "link_" + to_string(system_number_) + "_" + to_string(switch_number) + "_" + to_string(port_number);

    std::string link_w = "w_" + link;
    
    string message = "Sending a Hello from System " + to_string(system_number_) + " to " + to_string(receiver_system_number) + ".";
    Frame frame(this->system_number_, receiver_system_number, message);
    
    vector<string> frame_string = frame.getFrameString();

    for (int f = 0; f < frame_string.size(); f++) {
        cout << "System " << system_number_ << ": Frame-send: " << frame_string[0] << endl;

        cout << "System " << system_number_ << ": Trying to open link to write." << endl;
        int fd = open(link_w.c_str(), O_WRONLY);

        while (true) {
            int write_bytes = write(fd, frame_string[f].c_str(), strlen(frame_string[0].c_str()) + 1);
            if (write_bytes < 1) {
                cout << "System " << system_number_ << ": Couldn't write a message to Switch " << switch_number << "." << endl;
            } else {
                cout << "System " << system_number_ << ": write a message to Switch " << switch_number << "." << endl;
                close(fd);
                break;
            }
        }
    }

    return 1;
}

int System::receive() {    

    fd_set readfds;
    FD_ZERO(&readfds);

    if (this->used_port_ > -1) {
        // cout << "System " << system_number_ << ": Receiving ..." << endl;

        string link = "link_" + to_string(system_number_) + "_" + to_string(this->connected_switch_) + "_" + to_string(this->used_port_);

        std::string link_r = "r_" + link;
        
        // cout << "System " << system_number_ << ": " << link << endl;

        size_t message_size = 129;
        char message[message_size];

        // cout << "System " << system_number_ << ": Trying to open link to read." << endl;
        int fd = open(link_r.c_str(), O_RDONLY|O_NONBLOCK);

        FD_SET(fd, &readfds);

        if (FD_ISSET(fd, &readfds)) {
            
            int read_bytes = read(fd, message, message_size);

            // cout << "System " << system_number_ << ": Message from Switch " << this->connected_switch_ << ": " << read_bytes << " bytes." << endl;

            if (read_bytes > 0) {
                // cout << "System " << system_number_ << ": Message from Switch " << this->connected_switch_ << ": " << message << endl;

                Frame frame(message);

                if (system_number_ == frame.getRecieverId_()) {
                    cout << "System " << system_number_ << ": Message from System " << frame.getSenderId() << " to System " << frame.getRecieverId_() << ": " << frame.getMessage() << endl;
                }

                memset(message, 0, message_size);
            }
        } else {
            close(fd);
        }

    }

    return 1;
}
