CC := g++ -std=c++17 -pthread
BUILD_DIR=build
SRC_DIR=src

all : mk Network

Network : main.o Network.o Switch.o System.o Frame.o
		$(CC) ${BUILD_DIR}/main.o ${BUILD_DIR}/Network.o ${BUILD_DIR}/Switch.o ${BUILD_DIR}/System.o ${BUILD_DIR}/Frame.o -o Network

main.o : ${SRC_DIR}/main.cpp ${SRC_DIR}/Network.hpp
	$(CC) -c ${SRC_DIR}/main.cpp -o ${BUILD_DIR}/main.o

Network.o : ${SRC_DIR}/Network.cpp ${SRC_DIR}/Network.hpp
	$(CC) -c ${SRC_DIR}/Network.cpp -o ${BUILD_DIR}/Network.o

Switch.o : ${SRC_DIR}/Switch.cpp ${SRC_DIR}/Switch.hpp
	$(CC) -c ${SRC_DIR}/Switch.cpp -o ${BUILD_DIR}/Switch.o

System.o : ${SRC_DIR}/System.cpp ${SRC_DIR}/System.hpp
	$(CC) -c ${SRC_DIR}/System.cpp -o ${BUILD_DIR}/System.o

Frame.o : ${SRC_DIR}/Frame.cpp ${SRC_DIR}/Frame.hpp
	$(CC) -c ${SRC_DIR}/Frame.cpp -o ${BUILD_DIR}/Frame.o 

mk:
	mkdir ./${BUILD_DIR}

clean:
	rm -rf ${BUILD_DIR}
	rm Network
