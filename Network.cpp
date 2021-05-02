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
                cout << "System " << switch_class.get_number() << ": Connecting ..." << endl;

                int sec_index = string(message).find('#', fst_index + 1);

                int system_number = stoi(string(message).substr(fst_index + 1, sec_index - fst_index - 1));
                int port_number = stoi(string(message).substr(sec_index + 1));
                switch_class.connect(system_number, port_number);
            }
            memset(message, 0, message_size); 
        } else {
            sleep(10);
            switch_class.receive();
        }
    }

    close(read_fd);
    cout << "Switch " << switch_class.get_number() << ": Shutting down." << endl;
    pthread_exit(NULL);
}

void systemProcess(System system_class){
    cout << "System " << system_class.get_number() << ": Hello from System!" << endl;
    
    int read_fd = system_class.getCommandFd();

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
                
            }
            memset(message, 0, message_size);
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
            if (splitted_command.size() < 2) { cout<< "fuck size"<<endl; return 0;}
            return mySystem(splitted_command);
        } else if (command == "MySwitch"){
            if (splitted_command.size() < 3) { cout<< "fuck size"<<endl; return 0;}
            return mySwitch(splitted_command);
        } else if (command == "Connect"){
            if (splitted_command.size() < 4){ cout<< "fuck size"<<endl; return 0;}
            return connect(splitted_command);
        } else if (command == "Send"){
            // TODO: what is the structure?
            if (splitted_command.size() < 2) { cout<< "fuck size"<<endl; return 0;}
            return send(splitted_command);
        } else if (command == "Recieve"){
            // TODO: what is the structure?
            if (splitted_command.size() < 2) return 0;
            return recieve(splitted_command);
        }else{
            return 0;
        }
    } catch(exception &error){
        cout<<error.what()<<endl;
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
    //swtch.connect(system_number, port_number);
    //systm.connect(switch_number, port_number);

    string link = "link_" + to_string(system_number) + "_" + to_string(switch_number) + "_" + to_string(port_number);

    struct stat stats;
    if (stat(link.c_str(), &stats) < 0) {
        if (errno != ENOENT) {
            std::cout << "Network " << ": Stat failed. Error: " << errno << std::endl;
        }
    } else {
        if (unlink(link.c_str()) < 0) {
            std::cout << "Network " << ": Unlink failed." << std::endl;
        }
    }


    if (mkfifo(link.c_str(), 0666) < 0) {
        std::cout << "Network " << ": Failed to create link." << std::endl;
    }
    string system_message = "connect#" + to_string(switch_number) + "#" + to_string(port_number);

    int system_index = findSystem(system_number);
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

    int system_index;
    if ((system_index = findSystem(system_number)) < 0) {
        cout << "Network: System " << system_number << " is not available." << endl;
        return 0;
    }

    int receiver_system_index;
    if((receiver_system_index = findSystem(receiver_system_number)) < 0) {
        cout << "Network: System " << receiver_system_number << " is not available." << endl;
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

int Network::recieve(std::vector<std::string> &splitted_command){
    //TODO: No Idea!

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