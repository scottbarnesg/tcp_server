#!/bin/bash
gcc server.cpp -o tcp_server -pthread -lstdc++
gcc client.cpp -o tcp_client -lstdc++
