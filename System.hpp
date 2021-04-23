#ifndef _SYSTEM_HPP
#define _SYSTEM_HPP

class System{
    public:
    System(int system_number);
    int connect(int switch_number, int port_number);

    private:
    unsigned int system_number_;
};


#endif