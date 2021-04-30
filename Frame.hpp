#ifndef _FRAME_HPP
#define _FRAME_HPP

#include <string>
#include <iostream>

/*
frame is a 128 character string
            128
---------------------------
-   32      64      32    -
---------------------------
- Header - Data - Address -
---------------------------
*/

#define HEADER_SIZE 32
#define DATA_SIZE 64
#define ADDRESS_SIZE 32

class Frame{
    public:
    Frame(std::string frame_string);
    Frame(int sender_system_id, int reciever_system_id, std::string message);
    int getSenderId(){return sender_id_;}
    int getRecieverId_(){return reciever_id_;}
    std::string getMessage(){return message_;}
    std::string getFrameString(){return frame_string_;}

    private:
    int sender_id_;
    int reciever_id_;
    std::string message_;
    std::string frame_string_;
    void makeFrameString();
    void extractDataFromFrame();
    
};


#endif