#ifndef _SYSTEM_HPP
#define _SYSTEM_HPP

class System{
    public:
    System(int system_number);
    int connect(int switch_number, int port_number);
    int get_number(){return system_number_;}

    private:
    unsigned int system_number_;
};


#endif