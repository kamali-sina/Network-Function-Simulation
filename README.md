# Network-Function-Simulation

A network function simulation project coded on cpp. It features a series of simulated network components including switches, systems, frame decoders, and etc.

It uses multi-threading to run different components in parallel and uses IPC to pass messages between them.

## Running The Project

To build the code, run:

    make

Then, you can run the code by using:

    ./Network

## How to use

You can define differant components in the simulation by using a vide variaty of commands including:

* Switch: Defines a switch
* System: Defines a system
* Connect: Connects A switch to a system.
* ConnectSwitch: Connect two switches to each other.
* Send: Sends packet from a system.
* Recieve: Commands a system to recieve incoming packages.
* SpanningTree: Runs the spanning tree algorithm on the network to eliminate redundant cycles.

Feel free to email me if you have any questions.
