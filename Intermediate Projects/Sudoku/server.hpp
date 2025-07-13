#ifndef SERVER_HPP
#define SERVER_HPP

// Declare functions from server.cpp to expose for testing
void handle_client(int client_fd);
int start_server();

#endif  // SERVER_HPP