CC := g++ -std=c++11 -pthread

all : Network

Network : main.o Network.o Switch.o System.o
		$(CC) main.o Network.o Switch.o System.o -o Network

main.o : main.cpp Network.hpp
	$(CC) -c main.cpp

Network.o : Network.cpp Network.hpp
	$(CC) -c Network.cpp

Switch.o : Switch.cpp Switch.hpp
	$(CC) -c Switch.cpp

System.o : System.cpp System.hpp
	$(CC) -c System.cpp

clean:
	rm *.o Network
