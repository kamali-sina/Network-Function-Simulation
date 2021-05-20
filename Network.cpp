#include "Network.hpp"
#include <exception>
#include <unistd.h>

using namespace std;

void switchProcess(Switch switch_class){
    cout << "Switch " << switch_class.get_number() << ": Hello from Switch!" << endl;
    
    int read_fd = switch_class.getCommandFd();

    int flags = fcntl(read_fd, F_GETFL, 0);
    fcntl(read_fd, F_SETFL, flags | O_NONBLOCK);

    size_t message_size = 100;
    char message[message_size];

    while (true) {
        int read_bytes = read(read_fd, message, message_size);
        if (read_bytes > 0) {
            cout << "Switch " << switch_class.get_number() << ": Read " << read_bytes << " bytes. The message is: " << message << endl;
            
            int fst_index = string(message).find('#');
            string command = string(message).substr(0, fst_index) ;
            if (string(command).compare("connect") == 0) {
                cout << "Switch " << switch_class.get_number() << ": Connecting ..." << endl;

                int sec_index = string(message).find('#', fst_index + 1);

                int system_number = stoi(string(message).substr(fst_index + 1, sec_index - fst_index - 1));
                int port_number = stoi(string(message).substr(sec_index + 1));
                switch_class.connect(system_number, port_number);
            } else if (string(command).compare("connect_switch") == 0) {
                int sec_index = string(message).find('#', fst_index + 1);
                int trd_index = string(message).find('#', sec_index + 1);

                int switch_number = stoi(string(message).substr(fst_index + 1, sec_index - fst_index - 1));
                int port_number = stoi(string(message).substr(sec_index + 1, trd_index - sec_index - 1));

                string mode = string(message).substr(trd_index + 1);

                cout << "Switch " << switch_class.get_number() << ": Mode: " << mode << endl;

                if (mode.compare("request") == 0) {
                    switch_class.connectSwitch(switch_number, port_number);
                } else if (mode.compare("accept") == 0) {
                    switch_class.acceptConnectSwitch(switch_number, port_number);
                }
            } else if (string(command).compare("unlink_switch") == 0){
                int switch_to_unlink = stoi(string(message).substr(fst_index + 1));
                switch_class.unlinkSwitch(switch_to_unlink);
            }
            memset(message, 0, message_size); 
        } else {
            sleep(8);
            switch_class.receive();
            switch_class.receiveSwitch();
        }
    }

    close(read_fd);
    cout << "Switch " << switch_class.get_number() << ": Shutting down." << endl;
    pthread_exit(NULL);
}

void systemProcess(System system_class){
    cout << "System " << system_class.get_number() << ": Hello from System!" << endl;
    
    int read_fd = system_class.getCommandFd();

    int flags = fcntl(read_fd, F_GETFL, 0);
    fcntl(read_fd, F_SETFL, flags | O_NONBLOCK);

    size_t message_size = 100;
    char message [message_size];

    while (true) {
        int read_bytes = read(read_fd, message, message_size);
        if (read_bytes > 0) {
            cout << "System " << system_class.get_number() << ": Read " << read_bytes << " bytes. The message is: " << message << endl;

            int fst_index = string(message).find('#');
            string command = string(message).substr(0, fst_index) ;
            if (string(command).compare("connect") == 0) {
                cout << "System " << system_class.get_number() << ": Connecting ..." << endl;

                int sec_index = string(message).find('#', fst_index + 1);

                int switch_number = stoi(string(message).substr(fst_index + 1, sec_index - fst_index - 1));
                int port_number = stoi(string(message).substr(sec_index + 1));
                system_class.connect(switch_number, port_number);
            } else if (string(command).compare("send") == 0) {
                cout << "System " << system_class.get_number() << ": Sending ..." << endl;

                int sec_index = string(message).find('#', fst_index + 1);

                int receiver_system_number = stoi(string(message).substr(fst_index + 1, sec_index - fst_index - 1));
                system_class.send(receiver_system_number);
                
            } else if (string(command).compare("receive") == 0) {
                int sec_index = string(message).find('#', fst_index + 1);

                int receiver_system_number = stoi(string(message).substr(fst_index + 1, sec_index - fst_index - 1));
                
                cout << "System " << system_class.get_number() << ": Requesting receive from system " << receiver_system_number << "." << endl;
                system_class.send(receiver_system_number);
            }
            memset(message, 0, message_size);
        } else {
            sleep(4);
            system_class.receive();
        }
    }

    close (read_fd);
    cout << "System " << system_class.get_number() << ": Shutting down." << endl;
    pthread_exit(NULL);
}

Network::Network(){

}

std::vector<std::string> Network::splitCommand(std::string input){
    vector<string> duppedline;
    istringstream stream(input);
    while (stream){
        string word;
        stream>>word;
        duppedline.push_back (word);
    }
    if (duppedline[duppedline.size() - 1] == "")
        duppedline.pop_back();
    return duppedline;
}

int Network::findSwitch(int switch_number){
    for (int i = 0 ; i < switches_.size(); i++){
        Switch swtch = switches_[i];
        if (swtch.get_number() == switch_number) return i;
    }
    return -1;
}

int Network::findSystem(int system_number){
    for (int i = 0 ; i < systems_.size(); i++){
        System systm = systems_[i];
        if (systm.get_number() == system_number) return i;
    }
    return -1;
}


int Network::handleCommand(std::string input){
    vector<string> splitted_command = splitCommand(input);
    if (splitted_command.size() == 0) { cout<< "command size is 0"<<endl; return 0;}
    string command = splitted_command[0];
    try{
        if (command == "MySystem"){
            if (splitted_command.size() < 2) { cout<< "bad size"<<endl; return 0;}
            return mySystem(splitted_command);
        } else if (command == "MySwitch"){
            if (splitted_command.size() < 3) { cout<< "bad size"<<endl; return 0;}
            return mySwitch(splitted_command);
        } else if (command == "Connect"){
            if (splitted_command.size() < 4){ cout<< "bad size"<<endl; return 0;}
            return connect(splitted_command);
        } else if (command == "Send"){
            if (splitted_command.size() < 2) { cout<< "bad size"<<endl; return 0;}
            return send(splitted_command);
        } else if (command == "Receive"){
            if (splitted_command.size() < 2) return 0;
            return receive(splitted_command);
        } else if (command == "ConnectSwitch") {
            if (splitted_command.size() < 4){ cout<< "bad size"<<endl; return 0;}
            return connectSwitches(splitted_command);
        } else if (command == "SpanningTree") {
            if (splitted_command.size() < 1){ cout<< "bad size"<<endl; return 0;}
            return SpanningTree(splitted_command);
        } else {
            return 0;
        }
    } catch(exception &error){
        cout<<"Exception: "<<error.what()<<endl;
        return 0;
    }
    return 1;
}

int Network::mySwitch(std::vector<std::string> &splitted_command){
    int fds[2];
    if (pipe(fds) < 0) {
        cout << "Network: Failed to create pipe." << endl;
    }

    int read_fd = fds[READ];
    int write_fd = fds[WRITE];

    switches_.push_back(Switch(stoi(splitted_command[1]),stoi(splitted_command[2]), read_fd));
    this->switch_command_fd_.push_back(write_fd);
    
    int pid = fork();
    if (pid == -1){
        cout<<"error: something's wrong, fork failed"<<endl;
        exit(-1);
    }else if (pid == 0){
        usleep(10000);
        switchProcess(switches_[switches_.size() - 1]);
        exit(-1);
    }

    string message = "Hello from Network!";

    if (write(write_fd, message.c_str(), strlen(message.c_str()) + 1) < 0) {
       cout << "Network: Faile to write to system " << switches_[switches_.size() - 1].get_number() << " command file descriptor." << endl;
    }

    // close(write_fd);

    return 1;
}

int Network::mySystem(std::vector<std::string> &splitted_command){
    int fds[2];
    if (pipe(fds) < 0) {
        cout << "Network: Failed to create pipe." << endl;
    }

    int read_fd = fds[READ];
    int write_fd = fds[WRITE];

    systems_.push_back(System(stoi(splitted_command[1]), read_fd));
    this->systems_command_fd_.push_back(write_fd);
    
    int pid = fork();
    if (pid == -1){
        cout<<"error: something's wrong, fork failed"<<endl;
        exit(-1);
    }else if (pid == 0){
        usleep(10000);
        systemProcess(systems_[systems_.size() - 1]);
        exit(-1);
    }

    string message = "Hello from Network!";

    if (write(write_fd, message.c_str(), strlen(message.c_str()) + 1) < 0) {
       cout << "Network: Failed to write to system " << systems_[systems_.size() - 1].get_number() << " command file descriptor." << endl;
    }

    // close(write_fd);

    return 1;
}

int Network::connect(std::vector<std::string> &splitted_command){
    int system_number = stoi(splitted_command[1]);
    int switch_number = stoi(splitted_command[2]);
    int port_number = stoi(splitted_command[3]);

    string link = "link_" + to_string(system_number) + "_" + to_string(switch_number) + "_" + to_string(port_number);

    createNamePipe("w_"+link);
    createNamePipe("r_"+link);

    string system_message = "connect#" + to_string(switch_number) + "#" + to_string(port_number);

    int system_index;
    if ((system_index = this->isSystemAvailable(system_number)) < 0) {
        return 0;
    }
    systems_[system_index].setConnected();
    int system_write_fd = this->systems_command_fd_[system_index];
    if (write(system_write_fd, system_message.c_str(), strlen(system_message.c_str()) + 1) < 0) {
       cout << "Network: Failed to write to system " << system_number << " command file descriptor." << endl;
    }


    string switch_message = "connect#" + to_string(system_number) + "#" + to_string(port_number);
    int switch_index = findSwitch(switch_number);
    int switch_write_fd = this->switch_command_fd_[switch_index];

    if (write(switch_write_fd, switch_message.c_str(), strlen(switch_message.c_str()) + 1) < 0) {
       cout << "Network: Failed to write to switch " << switch_number << " command file descriptor." << endl;
    }

    return 1;
}

int Network::send(std::vector<std::string> &splitted_command){
    int system_number = stoi(splitted_command[1]);
    int receiver_system_number = stoi(splitted_command[2]);
    
    string system_message = "send#" + to_string(receiver_system_number) + "#";

    int system_index = this->isSystemAvailable(system_number);

    int receiver_system_index = this->isSystemAvailable(receiver_system_number);

    if ((system_index < 0) || (receiver_system_index < 0)) {
        return 0;
    }

    if (this->systems_[system_index].isConnected()) {
        int system_write_fd = this->systems_command_fd_[system_index];

        if (write(system_write_fd, system_message.c_str(), strlen(system_message.c_str()) + 1) < 0) {
            cout << "Network: Failed to write to system " << system_number << " command file descriptor." << endl;
            return 0;
        }

    } else {
        cout << "Network: System " << system_number << " is not connected." << endl;
        return 0;
    }

    return 1;
}

int Network::receive(std::vector<std::string> &splitted_command){
    int system_number = stoi(splitted_command[1]);
    int sender_system_number = stoi(splitted_command[2]);
    
    string system_message = "receive#" + to_string(sender_system_number) + "#";

    int system_index = this->isSystemAvailable(system_number);

    int sender_system_index = this->isSystemAvailable(sender_system_number);

    if ((system_index < 0) || (sender_system_index < 0)) {
        return 0;
    }

    if (this->systems_[system_index].isConnected()) {
        int system_write_fd = this->systems_command_fd_[system_index];

        if (write(system_write_fd, system_message.c_str(), strlen(system_message.c_str()) + 1) < 0) {
            cout << "Network: Failed to write to system " << system_number << " command file descriptor." << endl;
            return 0;
        }

    } else {
        cout << "Network: System " << system_number << " is not connected." << endl;
        return 0;
    }

    return 1;
}

bool Network::checkAddVisited(vector<vector<int>> &tree_holder, switch_link swtch_link){
    int path1 = findPathIndex(tree_holder, swtch_link.switch1);
    int path2 = findPathIndex(tree_holder, swtch_link.switch2);
    if (path1 == -1 && path2 == -1){
        vector<int> temp{swtch_link.switch1, swtch_link.switch2};
        tree_holder.push_back(temp);
    }else if (path1 == -1 && path2 != -1){
        tree_holder[path2].push_back(swtch_link.switch1);
    }else if (path2 == -1 && path1 != -1){
        tree_holder[path1].push_back(swtch_link.switch2);
    }else if (path1 == path2){
        //bad one
        return false;
    }else{
        for (auto n : tree_holder[path2]){
            tree_holder[path1].push_back(n);
        }
        tree_holder.erase(tree_holder.begin() + path2);
    }
    return true;
}

int Network::findPathIndex(std::vector<std::vector<int>> &tree_holder, int node_){
    for (int i = 0 ; i < tree_holder.size() ; i++){
        for (auto n : tree_holder[i]){
            if (node_ == n) return i;
        }
    }
    return -1;
}

void Network::removeLink(switch_link swtch_link){
    int fst_switch_number = swtch_link.switch1;
    int sec_switch_number = swtch_link.switch2;

    string fst_switch_message = "unlink_switch#" + to_string(sec_switch_number);
    int fst_switch_index = findSwitch(fst_switch_number);
    int fst_switch_write_fd = this->switch_command_fd_[fst_switch_index];

    if (write(fst_switch_write_fd, fst_switch_message.c_str(), strlen(fst_switch_message.c_str()) + 1) < 0) {
       cout << "Network: Failed to write to switch " << fst_switch_number << " command file descriptor." << endl;
    }

    string sec_switch_message = "unlink_switch#" + to_string(fst_switch_number);
    int sec_switch_index = findSwitch(sec_switch_number);
    int sec_switch_write_fd = this->switch_command_fd_[sec_switch_index];

    if (write(sec_switch_write_fd, sec_switch_message.c_str(), strlen(sec_switch_message.c_str()) + 1) < 0) {
       cout << "Network: Failed to write to switch " << sec_switch_number << " command file descriptor." << endl;
    }
}

int Network::SpanningTree(std::vector<std::string> &splitted_command){
    cout<<"=================================="<<endl;
    cout<<"Running Spanning Tree..."<<endl;
    cout<<"searching for loops..."<<endl;
    vector<switch_link> bad_links;
    if (links.size() < 1){
        cout<<"there are no switches connected, therefor no loops!"<<endl;
        return 1;
    }

    vector<vector<int>> visited_nodes;
    for (auto x : links){
        usleep(30000);
        bool status = checkAddVisited(visited_nodes, x);
        if (!status){
            cout<<"found a bad link: <"<<x.switch1<<", "<<x.switch2<<">"<<endl;
            bad_links.push_back(x);
        }
    }
    
    if (bad_links.size() > 0){
        cout<<"removing bad links..."<<endl;
        for (auto x : bad_links){
            removeLink(x);
        }
    }else{
        cout<<"did not find any loops, done."<<endl;
    }
    return 1;
}

int Network::run(){
    string input;
    while (getline(cin, input)){
        if (handleCommand(input) == 0)
            cout<<"ERROR: Bad Command"<<endl;
    }
    return 1;
}

int Network::isSystemAvailable(int system_number) {
    int system_index; 
    if ((system_index = findSystem(system_number)) < 0) {
        cout << "Network: System " << system_number << " is not available." << endl;
        return -1;
    }

    return system_index;
}

int Network::connectSwitches(std::vector<std::string> &splitted_command) {
    int fst_switch_number = stoi(splitted_command[1]);
    int sec_switch_number = stoi(splitted_command[2]);
    int port_number = stoi(splitted_command[3]);

    string link = "link_switch_" + to_string(fst_switch_number) + "_" + to_string(sec_switch_number) + "_" + to_string(port_number);

    createNamePipe("w_"+link);
    createNamePipe("r_"+link);

    string fst_switch_message = "connect_switch#" + to_string(sec_switch_number) + "#" + to_string(port_number) + "#request";
    int fst_switch_index = findSwitch(fst_switch_number);
    int fst_switch_write_fd = this->switch_command_fd_[fst_switch_index];

    if (write(fst_switch_write_fd, fst_switch_message.c_str(), strlen(fst_switch_message.c_str()) + 1) < 0) {
       cout << "Network: Failed to write to switch " << fst_switch_number << " command file descriptor." << endl;
    }

    string sec_switch_message = "connect_switch#" + to_string(fst_switch_number) + "#" + to_string(port_number) + "#accept";
    int sec_switch_index = findSwitch(sec_switch_number);
    int sec_switch_write_fd = this->switch_command_fd_[sec_switch_index];

    if (write(sec_switch_write_fd, sec_switch_message.c_str(), strlen(sec_switch_message.c_str()) + 1) < 0) {
       cout << "Network: Failed to write to switch " << sec_switch_number << " command file descriptor." << endl;
    }

    switch_link x;
    x.switch1 = fst_switch_number;
    x.switch2 = sec_switch_number;
    links.push_back(x);

    return 1;
}

int Network::createNamePipe(std::string link) {
    struct stat stats;
    if (stat(link.c_str(), &stats) < 0) {
        if (errno != ENOENT) {
            std::cout << "Network " << ": Stat failed. Error: " << errno << std::endl;
            return 0;
        }
    } else {
        if (unlink(link.c_str()) < 0) {
            std::cout << "Network " << ": Unlink failed." << std::endl;
            return 0;
        }
    }

    if (mkfifo(link.c_str(), 0666) < 0) {
        std::cout << "Network " << ": Failed to create link." << std::endl;
        return 1;
    }

    return 0;
}
