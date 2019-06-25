tcp_server: server.cpp
		gcc server.cpp -std=c++11 -pthread -lstdc++
		
tcp_client: client.cpp
		gcc client.cpp -std=c++11 -lstdc++
