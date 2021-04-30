#include "Frame.hpp"
#include <string>

using namespace std;

Frame::Frame(std::string frame_string){
    frame_string_ = frame_string;
    extractDataFromFrame();
}

Frame::Frame(int sender_system_id, int reciever_system_id, std::string message){
    sender_id_ = sender_system_id;
    reciever_id_ = reciever_system_id;
    message_ = message;
    makeFrameString();
}

void Frame::makeFrameString(){
    string header = to_string(sender_id_);
    if (header.size() > HEADER_SIZE) cout<<"Error: header is longer than the possible size!"<<endl;
    header.resize(HEADER_SIZE, '\0');
    string message = message_;
    if (message.size() > DATA_SIZE) cout<<"Error: message is longer than the possible size!"<<endl;
    message.resize(DATA_SIZE, '\0');
    string address = to_string(reciever_id_);
    if (message.size() > DATA_SIZE) cout<<"Error: address is longer than the possible size!"<<endl;
    address.resize(ADDRESS_SIZE, '\0');
    frame_string_ = header + message + address;
}

void Frame::extractDataFromFrame(){
    sender_id_ = stoi(frame_string_.substr(0, HEADER_SIZE));
    message_ = frame_string_.substr(HEADER_SIZE, DATA_SIZE);
    reciever_id_ = stoi(frame_string_.substr(HEADER_SIZE + DATA_SIZE, ADDRESS_SIZE));
}