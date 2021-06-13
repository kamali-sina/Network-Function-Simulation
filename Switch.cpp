#include "Switch.hpp"
#include <fcntl.h>

using namespace std;

Switch::Switch(std::string ip, int switch_number, int command_fd){
    number_of_ports_ = 20;
    IP_ = ip;
    switch_number_ = switch_number;
    command_fd_ = command_fd;
}

int Switch::connect(int system_number, int port_number){
    string link = "link_" + to_string(system_number) + "_" + to_string(switch_number_) + "_" + to_string(port_number);

    std::string link_r = "r_" + link;
    std::string link_w = "w_" + link;
    
    std::string send_message = "Hello from Switch " + to_string(switch_number_) + ".";
    
    size_t message_size = 129;
    char message[message_size];

    int read_flag, write_flag = 0;

    while (!(read_flag && write_flag))
    {
        std::cout << "Switch " << switch_number_ << ": Trying to open link to read." << std::endl;
        int fd = open(link_w.c_str(), O_RDONLY);
  
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
        fd = open(link_r.c_str(), O_WRONLY);
  
        int write_bytes = write(fd, send_message.c_str(), strlen(send_message.c_str()) + 1);
        if (write_bytes < 1) {
            std::cout << "Switch " << switch_number_ << ": Couldn't write a message to System " << system_number << ": " << message << std::endl;
        } else {
            write_flag = 1;
        }

        if (this->addToConnectedTable(system_number, port_number) < 1) {
            std::cout << "Switch " << switch_number_ << ": Port number " << port_number << "is already taken." << endl;
            return 0;
        }

        close(fd);
    }
    
    cout<<"Switch " << switch_number_ << ": Connect Complete"<<endl;

    return 1;
}

int Switch::getCommandFd() {
    return command_fd_;
}

bool Switch::hasInLookupTable(DeviceInfo device_info) {
    for (int i = 0; i < this->lookup_table_.size(); i++) {
        if (lookup_table_[i].device_number == device_info.device_number && lookup_table_[i].type == device_info.type)
            return true;
    }
    
    return false;
}

int Switch::updateLookupTable(DeviceInfo device_info) {
    if (this->hasInLookupTable(device_info)) {
        return 0;
    }
    this->lookup_table_.push_back(device_info);
    return 1;
}

int Switch::updateLookupTable(std::vector<DeviceInfo> lookup_table, string IP) {
    for (int i = 0; i < lookup_table.size(); i++) {
        lookup_table[i].IP_address_ = IP;
        this->updateLookupTable(lookup_table[i]);
    }
    return 0;
}

void Switch::printLookupTable() {
    cout << "Router " << this->switch_number_ << " Lookup Table" << endl;
    cout << "=====================" << endl;
    for (int i = 0; i < this->lookup_table_.size(); i++) {
        if (lookup_table_[i].type == ROUTER)
            cout << "Router " << lookup_table_[i].device_number << " IP: " << lookup_table_[i].IP_address_ << " port: " << lookup_table_[i].port_number << endl;
        else if (lookup_table_[i].type == SYSTEM)
            cout << "Client IP: " << lookup_table_[i].IP_address_ << " port: " << lookup_table_[i].port_number << endl;
        else if (lookup_table_[i].type == SERVER)
            cout << "Server IP: " << lookup_table_[i].IP_address_ << endl;
        cout << "---------------------" << endl;
    }
    cout<<endl;
}


int Switch::receive() {
    // cout << "Switch " << switch_number_ << ": Receiving ..." << endl;

    fd_set readfds;
    FD_ZERO(&readfds);

    for (int system_index = 0; system_index < this->connected_systems_table_.size(); system_index++) {
        int system_number = this->connected_systems_table_[system_index].device_number;
        int port_number = this->connected_systems_table_[system_index].port_number;

        string link = "link_" + to_string(system_number) + "_" + to_string(switch_number_) + "_" + to_string(port_number);

        std::string link_w = "w_" + link;
        
        // cout << "Switch " << switch_number_ << ": " << link << endl;
        
        size_t message_size = 129;
        char message[message_size];

        // cout << "Switch " << switch_number_ << ": Trying to open link to read." << endl;
        int fd = open(link_w.c_str(), O_RDONLY|O_NONBLOCK);

        FD_SET(fd, &readfds);

        if (FD_ISSET(fd, &readfds)) {
            
            int read_bytes = read(fd, message, message_size);
            if (read_bytes > 0) {
                cout << "Switch " << switch_number_ << ": Message from System " << system_number << ": " << message << endl;

                Frame frame(message);
                int receiver_port = this->getPortNumberFromLookupTable(frame.getSenderId());

                // if (!(this->isSystemNumberInLookupTable(frame.getSenderId()))) {
                //     this->updateLookupTable(frame.getSenderId(), port_number);
                // }

                if (this->isSystemNumberInLookupTable(frame.getRecieverId_())) {
                    this->send(frame);
                } else {
                    this->broadcast(frame);
                }

                memset(message, 0, message_size);
                close(fd);
            }
        } else {
            close(fd);
        }
    }

    return 1;
}

int Switch::receiveSwitch() {
    // cout << "Switch " << switch_number_ << ": SwitchReceiving ..." << endl;

    fd_set readfds;
    FD_ZERO(&readfds);

    for (int swithc_index = 0; swithc_index < this->connected_switches_table_.size(); swithc_index++) {
        int switch_number = this->connected_switches_table_[swithc_index].device_number;
        int port_number = this->connected_switches_table_[swithc_index].port_number;

        std::string link = "";
        
        if (this->connected_switches_table_[swithc_index].connector) {
            link = "r_link_switch_" + to_string(switch_number_) + "_" + to_string(switch_number) + "_" + to_string(port_number);
        } else {
            link = "w_link_switch_" + to_string(switch_number) + "_" + to_string(switch_number_) + "_" + to_string(port_number);
        }
        
        // cout << "Switch " << switch_number_ << ": SwitchReceiving:" << link << endl;
        
        size_t message_size = 129;
        char message[message_size];

        // cout << "Switch " << switch_number_ << ": Trying to open link to read." << endl;
        int fd = open(link.c_str(), O_RDONLY|O_NONBLOCK);

        FD_SET(fd, &readfds);

        if (FD_ISSET(fd, &readfds)) {
            
            int read_bytes = read(fd, message, message_size);
            if (read_bytes > 0) {
                cout << "Switch " << switch_number_ << ": Message from Switch " << switch_number << ": " << message << endl;

                Frame frame(message);
                int receiver_port = this->getPortNumberFromLookupTable(frame.getSenderId());

                // if (!(this->isSystemNumberInLookupTable(frame.getSenderId()))) {
                //     this->updateLookupTable(frame.getSenderId(), port_number);
                // }

                if (this->isSystemNumberInLookupTable(frame.getRecieverId_())) {
                    this->send(frame);
                } else {
                    this->broadcast(frame);
                }

                memset(message, 0, message_size);
                close(fd);
            }
        } else {
            close(fd);
        }
    }

    return 1;
}

int Switch::addToConnectedTable(int system_number, int index_number) {
    for (int system_index = 0; system_index < this->connected_systems_table_.size(); system_index++) {
        if (index_number == this->connected_systems_table_[system_index].port_number) {
            return 0;
        }
    }

    DeviceInfo system;
    system.device_number = system_number;
    system.port_number = index_number;

    this->connected_systems_table_.push_back(system);

    return 1;
}

int Switch::connectSwitch(int switch_number, int port_number) {
    std::cout << "Switch " << switch_number_ << ": connectSwitch." << std::endl;

    string link = "link_switch_" + to_string(switch_number_) + "_" + to_string(switch_number) + "_" + to_string(port_number);

    std::string link_r = "r_" + link;
    std::string link_w = "w_" + link;
    
    std::string send_message = "Hello from Switch " + to_string(switch_number_) + ".";
    
    size_t message_size = 129;
    char message[message_size];

    int read_flag, write_flag = 0;

    while (!(read_flag && write_flag)) {
        std::cout << "Switch " << switch_number_ << ": Trying to open link to write." << std::endl;
        int fd = open(link_w.c_str(), O_WRONLY);
  
        int write_bytes = write(fd, send_message.c_str(), strlen(send_message.c_str()) + 1);
        if (write_bytes < 1) {
            std::cout << "Switch " << switch_number_ << ": Couldn't write a message to Switch " << switch_number << ": " << message << std::endl;
        } else {
            write_flag = 1;
        }

        close(fd);

        std::cout << "Switch " << switch_number_ << ": Trying to open link to read." << std::endl;
        fd = open(link_r.c_str(), O_RDONLY);
  
        int read_bytes = read(fd, message, message_size);
        if (read_bytes > 1) {
            std::cout << "Switch " << switch_number_ << ": Message from Switch " << switch_number << ": " << message << std::endl;
            memset(message, 0, message_size);
            read_flag = 1;
        } else {
            std::cout << "Switch " << switch_number_ << ": Dind't recieve message from Switch " << switch_number << "." << std::endl;
        }

        close(fd);
        
    }
    
    cout << "Switch " << switch_number_ << ": Connect Complete" << endl;
    this->addToSwitchesConnectedTable(switch_number, port_number, true);

    return 1;
}

int Switch::acceptConnectSwitch(int switch_number, int port_number) {
    std::cout << "Switch " << switch_number_ << ": acceptConnectSwitch." << std::endl;

    string link = "link_switch_" + to_string(switch_number) + "_" + to_string(switch_number_) + "_" + to_string(port_number);

    std::string link_r = "r_" + link;
    std::string link_w = "w_" + link;
    
    std::string send_message = "Hello from Switch " + to_string(switch_number_) + ".";
    
    size_t message_size = 129;
    char message[message_size];

    int read_flag, write_flag = 0;

    while (!(read_flag && write_flag)) {
        std::cout << "Switch " << switch_number_ << ": Trying to open link to read." << std::endl;
        int fd = open(link_w.c_str(), O_RDONLY);
  
        int read_bytes = read(fd, message, message_size);
        if (read_bytes > 1) {
            std::cout << "Switch " << switch_number_ << ": Message from Switch " << switch_number << ": " << message << std::endl;
            memset(message, 0, message_size);
            read_flag = 1;
        } else {
            std::cout << "Switch " << switch_number_ << ": Didn't recieve message from Switch " << switch_number << "." << std::endl;
        }
        
        close(fd);

        std::cout << "Switch " << switch_number_ << ": Trying to open link to write." << std::endl;
        fd = open(link_r.c_str(), O_WRONLY);
  
        int write_bytes = write(fd, send_message.c_str(), strlen(send_message.c_str()) + 1);
        if (write_bytes < 1) {
            std::cout << "Switch " << switch_number_ << ": Couldn't write a message to Switch " << switch_number << ": " << message << std::endl;
        } else {
            write_flag = 1;
        }

        close(fd);
    }
    
    cout << "Switch " << switch_number_ << ": Connect Complete" << endl;
    this->addToSwitchesConnectedTable(switch_number, port_number, false);

    return 1;
}

int Switch::unlinkSwitch(int switch_number){
    for (int switch_index = 0; switch_index < this->connected_switches_table_.size(); switch_index++) {
        if (switch_number == this->connected_switches_table_[switch_index].device_number){
            this->connected_switches_table_.erase(this->connected_switches_table_.begin() + switch_index);
            cout<<"Switch " << switch_number_ << ": Successfully unlinked from switch "<<switch_number<<endl;
            return 1;
        }
    }
    cout<<"Error: trying to unlink from switch that is not connected!"<<endl;
    return 0;
}

int Switch::addToSwitchesConnectedTable(int switch_number, int index_number, bool connector) {
    for (int switch_index = 0; switch_index < this->connected_switches_table_.size(); switch_index++) {
        if (index_number == this->connected_switches_table_[switch_index].port_number) {
            cout<<"ERROR: Port was occupied!"<<endl;
            return 0;
        }
    }

    DeviceInfo switch_device;
    switch_device.device_number = switch_number;
    switch_device.port_number = index_number;
    switch_device.connector = connector;

    this->connected_switches_table_.push_back(switch_device);

    return 1;
}

bool Switch::isSystemNumberInLookupTable(int system_number) {
    for (int i = 0; i < this->lookup_table_.size(); i++) {
        if (this->lookup_table_[i].device_number == system_number) {
            return true;
        }
    }
    
    return false;
}

int Switch::getPortNumberFromLookupTable(int system_number) {
    for (int i = 0; i < this->lookup_table_.size(); i++) {
        if (this->lookup_table_[i].device_number == system_number) {
            return this->lookup_table_[i].port_number;
        }
    }
    
    return -1;
}

int Switch::send(Frame frame) {
    string link = "r_link_";
    
    int receiver_port_number = this->getPortNumberFromLookupTable(frame.getRecieverId_());
    link += to_string(frame.getRecieverId_()) + "_" + to_string(switch_number_) + "_" + to_string(receiver_port_number);

    vector<string> frame_string = frame.getFrameString();

    for (int f = 0; f < frame_string.size(); f++) {
        cout << "Switch " << switch_number_ << ": Frame-send: " << frame_string[f] << endl;

        cout << "Switch " << switch_number_ << ": Trying to open link " << link << " to write." << endl;

        int fd = open(link.c_str(), O_WRONLY);

        while (true) {
            int write_bytes = write(fd, frame_string[f].c_str(), strlen(frame_string[f].c_str()) + 1);
            if (write_bytes < 1) {
                cout << "Switch " << switch_number_ << ": Couldn't write a message to System." << endl;
                sleep(5);
            } else {
                cout << "Switch " << switch_number_ << ": write a message to System." << endl;
                close(fd);
                break;
            }
        }
    }

    return 1;
    
}

int Switch::broadcast(Frame frame) {
    this->broadcastToSystems(frame);
    this->broadcastToSwitches(frame);
    return 1;
}

int Switch::broadcastToSystems(Frame frame) {
    string link = "";

    for (int i = 0; i < this->connected_systems_table_.size(); i++) {
        if (frame.getSenderId() == connected_systems_table_[i].device_number) {
            continue;
        }

        link = "r_link_" + to_string(this->connected_systems_table_[i].device_number) + "_" + to_string(switch_number_) + "_" + to_string(this->connected_systems_table_[i].port_number);

        vector<string> frame_string = frame.getFrameString();

        for (int f = 0; f < frame_string.size(); f++) {
            cout << "Switch " << switch_number_ << ": Frame-send: " << frame_string[f] << endl;

            cout << "Switch " << switch_number_ << ": Trying to open link " << link << " to write." << endl;

            int fd = open(link.c_str(), O_WRONLY);

            while (true) {
                int write_bytes = write(fd, frame_string[f].c_str(), strlen(frame_string[f].c_str()) + 1);
                if (write_bytes < 1) {
                    cout << "Switch " << switch_number_ << ": Couldn't write a message to System." << endl;
                    sleep(5);
                } else {
                    cout << "Switch " << switch_number_ << ": write a message to System." << endl;
                    close(fd);
                    break;
                }
            }
        }
        
    }

    return 1;
}

int Switch::broadcastToSwitches(Frame frame) {
    for (int i = 0; i < this->connected_switches_table_.size(); i++) {
        if (frame.getSenderId() == connected_switches_table_[i].device_number) {
            continue;
        }

        if (this->getPortNumberFromLookupTable(frame.getSenderId()) == this->connected_switches_table_[i].port_number) {
            continue;
        }

        std::string link = "";
        
        if (this->connected_switches_table_[i].connector) {
            link = "w_link_switch_" + to_string(switch_number_) + "_" + to_string(connected_switches_table_[i].device_number) + "_" + to_string(this->connected_switches_table_[i].port_number);
        } else {
            link = "r_link_switch_" + to_string(connected_switches_table_[i].device_number) + "_" + to_string(switch_number_) + "_" + to_string(this->connected_switches_table_[i].port_number);
        }

        vector<string> frame_string = frame.getFrameString();

        for (int f = 0; f < frame_string.size(); f++) {
            cout << "Switch " << switch_number_ << ": Frame-send: " << frame_string[f] << endl;

            cout << "Switch " << switch_number_ << ": Trying to open link " << link << " to write." << endl;

            int fd = open(link.c_str(), O_WRONLY);

            while (true) {
                int write_bytes = write(fd, frame_string[f].c_str(), strlen(frame_string[f].c_str()) + 1);
                if (write_bytes < 1) {
                    cout << "Switch " << switch_number_ << ": Couldn't write a message to Switch." << endl;
                    sleep(5);
                } else {
                    cout << "Switch " << switch_number_ << ": write a message to Switch." << endl;
                    close(fd);
                    break;
                }
            }
        }
        
    }

    return 1;
}