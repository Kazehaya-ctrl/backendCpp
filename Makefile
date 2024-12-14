all: compile

compile: 
	g++ -std=c++11 -o server server.cpp -lboost_system -pthread

clear: 
	rm -f ./server