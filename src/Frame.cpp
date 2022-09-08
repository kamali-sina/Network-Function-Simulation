#include "Frame.hpp"
#include <string>

using namespace std;

Frame::Frame(std::string frame_string){
    frame_strings_.push_back(frame_string);
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
    string address = to_string(reciever_id_);
    header.resize(HEADER_SIZE, '#');
    address.resize(ADDRESS_SIZE, '#');

    string message = message_;
    int curr_index = 0;
    while (curr_index + DATA_SIZE < message.size()){
        string sub_message = message.substr(curr_index, DATA_SIZE);
        sub_message.resize(DATA_SIZE, '#');
        string frame_string = header + sub_message + address;
        frame_strings_.push_back(frame_string);
        curr_index += DATA_SIZE;
    }
    string sub_message = message.substr(curr_index);
    sub_message.resize(DATA_SIZE, '#');
    string frame_string = header + sub_message + address;
    frame_strings_.push_back(frame_string);
}

void Frame::extractDataFromFrame(){
    string sender_string = frame_strings_[0].substr(0, HEADER_SIZE);
    int index = sender_string.find('#');
    sender_id_ = stoi(sender_string.substr(0, index));
    string message_string = frame_strings_[0].substr(HEADER_SIZE, DATA_SIZE);
    index = message_string.find('#');
    message_ = message_string.substr(0, index);
    string reciever_string = frame_strings_[0].substr(HEADER_SIZE + DATA_SIZE, ADDRESS_SIZE);
    index = reciever_string.find('#');
    reciever_id_ = stoi(reciever_string.substr(0, index));
}