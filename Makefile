all: main.cpp
	g++ -o motortest main.cpp $(pkg-config --cflags --libs libsocketcan)

