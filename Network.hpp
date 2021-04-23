#ifndef _NETWORK_HPP
#define _NETWORK_HPP

#include <vector>
#include "System.hpp"
#include "Switch.hpp"

class Network{
    public:
    Network();
    int run();

    private:
    std::vector<System> systems_;
    std::vector<Switch> swithces_;
};


#endif