server_exec: server.cpp
		gcc server.cpp -std=c++11 -o tcp_server -pthread -lstdc++
client_exec: client.cpp
		gcc client.cpp -std=c++11 -o tcp_client -lstdc++
