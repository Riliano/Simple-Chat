server:
	g++ server.cpp -o server -std=c++11 -pthread -lSDL2 -lSDL2_net
client:
	g++ client.cpp -o client -std=c++11 -pthread -lSDL2 -lSDL2_net
