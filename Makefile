tcp_server: server.cpp tcp_client
		gcc server.cpp -std=c++11 -o tcp_server -pthread -lstdc++
		
tcp_client: client.cpp
		gcc client.cpp -std=c++11 -lstdc++
