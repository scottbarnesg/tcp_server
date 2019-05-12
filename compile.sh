#!/bin/bash
gcc server.cpp -o tcp_server -pthread
gcc client.cpp -o tcp_client
