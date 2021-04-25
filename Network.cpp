#include "Network.hpp"

using namespace std;

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
    }

}

int Network::mySwitch(std::vector<std::string> &splitted_command){
    // TODO: This part has to be completed with threads
    switches_.push_back(Switch(stoi(splitted_command[1]),stoi(splitted_command[2])));
}

int Network::mySystem(std::vector<std::string> &splitted_command){
    // TODO: This part has to be completed with threads
    systems_.push_back(System(stoi(splitted_command[1])));
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
}

int Network::send(std::vector<std::string> &splitted_command){
    //TODO: No Idea!
}

int Network::recieve(std::vector<std::string> &splitted_command){
    //TODO: No Idea!
}



int Network::run(){
    string input;
    while (getline(cin, input)){
        if (handleCommand(input) == 0)
            cout<<"ERROR: bad command"<<endl;
    }
}