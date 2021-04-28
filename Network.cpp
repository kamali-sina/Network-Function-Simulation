#include "Network.hpp"

using namespace std;

void* switchThread(void* switch_class){
    Switch *this_class = (Switch*) switch_class;
    cout<<"I am alive and running: "<<this_class->get_number()<<endl;
    return 0;
}

void* systemThread(void* system_class){
    System *this_class = (System*) system_class;
    cout<<"I am alive and running: "<<this_class->get_number()<<endl;
    return 0;
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
    if (splitted_command.size() == 0) return 0;
    string command = splitted_command[0];
    if (command == "MySystem"){
        if (splitted_command.size() < 2) return 0;
        return mySystem(splitted_command);
    } else if (command == "MySwitch"){
        if (splitted_command.size() < 3) return 0;
        return mySwitch(splitted_command);
    } else if (command == "Connect"){
        if (splitted_command.size() < 4) return 0;
        return connect(splitted_command);
    } else if (command == "Send"){
        // TODO: what is the structure?
        if (splitted_command.size() < 2) return 0;
        return send(splitted_command);
    } else if (command == "Recieve"){
        // TODO: what is the structure?
        if (splitted_command.size() < 2) return 0;
        return recieve(splitted_command);
    }else{
        return 0;
    }
    return 1;
}

int Network::mySwitch(std::vector<std::string> &splitted_command){
    // TODO: Establish some sort of pipe when needed
    pthread_t new_thread;
    switches_.push_back(Switch(stoi(splitted_command[1]),stoi(splitted_command[2])));
    pthread_create(&new_thread, NULL, switchThread, (void*)&switches_[switches_.size() - 1]);
    threads.push_back(new_thread);
    return 1;
}

int Network::mySystem(std::vector<std::string> &splitted_command){
    // TODO: Establish some sort of pipe when needed
    pthread_t new_thread;
    systems_.push_back(System(stoi(splitted_command[1])));
    pthread_create(&new_thread, NULL, systemThread, (void*)&systems_[systems_.size() - 1]);
    threads.push_back(new_thread);
    return 1;
}

int Network::connect(std::vector<std::string> &splitted_command){
    int system_number = stoi(splitted_command[1]);
    int switch_number = stoi(splitted_command[2]);
    int port_number = stoi(splitted_command[3]);
    Switch swtch = switches_[findSwitch(switch_number)];
    System systm = systems_[findSystem(system_number)];
    swtch.connect(system_number, port_number);
    systm.connect(switch_number, port_number);
    // TODO: complete if needed
    return 1;
}

int Network::send(std::vector<std::string> &splitted_command){
    //TODO: No Idea!

    return 1;
}

int Network::recieve(std::vector<std::string> &splitted_command){
    //TODO: No Idea!

    return 1;
}



int Network::run(){
    pthread_mutex_init(&mutex_lock, NULL);
    string input;
    while (getline(cin, input)){
        if (handleCommand(input) == 0)
            cout<<"ERROR: Unknown Command"<<endl;
    }
    return 1;
}