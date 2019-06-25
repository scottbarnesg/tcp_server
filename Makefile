server_exec: server.cpp
		gcc server.cpp -std c++17 -o tcp_server -pthread -lstdc++
