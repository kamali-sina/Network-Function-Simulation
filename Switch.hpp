#ifndef _SWITCH_HPP
#define _SWITCH_HPP

class Switch{
    public:
    Switch(int number_of_ports, int switch_number);
    connect(int system_number, int port_number);

    private:
    unsigned int number_of_ports_;
    unsigned int switch_number_;
};

#endif