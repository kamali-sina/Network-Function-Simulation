all : Network

Network : main.o Network.o Switch.o System.o
		g++ main.o Network.o Switch.o System.o -o Network

main.o : main.cpp Network.hpp
	g++ -c main.cpp

Network.o : Network.cpp Network.hpp
	g++ -c Network.cpp

Switch.o : Switch.cpp Switch.hpp
	g++ -c Switch.cpp

System.o : System.cpp System.hpp
	g++ -c System.cpp

clean:
	rm *.o Network
