# Multi-Threaded, Multi-Process TCP Server
### Server:
- Main thread listens for incoming connections
- New fork created to handle each accepted connection
- Management thread reaps terminated forks
