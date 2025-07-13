#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <netinet/in.h>
#include <unistd.h>
#include "solver.h"

#define PORT 8080

// Extracts the value associated with a key from a naive JSON string
std::string extract_value(const std::string& json, const std::string& key) {
    std::size_t key_pos = json.find("\"" + key + "\"");
    if (key_pos == std::string::npos) return "";

    std::size_t colon_pos = json.find(":", key_pos);
    if (colon_pos == std::string::npos) return "";

    std::size_t start = json.find_first_not_of(" \"", colon_pos + 1);
    std::size_t end = json.find_first_of(",}", start);

    std::string value = json.substr(start, end - start);
    value.erase(value.find_last_not_of("\"") + 1);

    return value;
}

// Extracts the Sudoku board from the incoming JSON body
std::string extract_board(const std::string& body) {
    return extract_value(body, "board");
}

// Builds a minimal JSON response containing the solution (or null if failed)
std::string build_response_json(const std::string& solution) {
    std::ostringstream response;
    if (!solution.empty()) {
        response << "{\"solution\":\"" << solution << "\"}";
    } else {
        response << "{\"solution\":null}";
    }
    return response.str();
}

// Handles a single client connection: reads request, processes it, and sends back a response
void handle_client(int client_fd) {
    char buffer[8192] = {0};
    read(client_fd, buffer, sizeof(buffer));
    std::string request(buffer);

    // Check that the request is a POST to /solve
    if (request.find("POST /solve") != 0) {
        std::string not_found = "HTTP/1.1 404 Not Found\r\n\r\n";
        send(client_fd, not_found.c_str(), not_found.size(), 0);
        close(client_fd);
        return;
    }

    // Extract the body of the HTTP request (after the header delimiter)
    std::size_t json_start = request.find("\r\n\r\n");
    if (json_start == std::string::npos) {
        std::string bad = "HTTP/1.1 400 Bad Request\r\n\r\n";
        send(client_fd, bad.c_str(), bad.size(), 0);
        close(client_fd);
        return;
    }

    std::string body = request.substr(json_start + 4);
    std::string board_str = extract_board(body);

    // Use the SudokuSolver class to solve the puzzle
    SudokuSolver solver(board_str);
    std::string solution = solver.solve();

    // Prepare the JSON response
    std::string response_body = build_response_json(solution);

    // Build the full HTTP response with headers and body
    std::ostringstream http_response;
    if (!solution.empty()) {
        http_response << "HTTP/1.1 200 OK\r\n";
    } else {
        http_response << "HTTP/1.1 400 Bad Request\r\n";
    }

    http_response << "Content-Type: application/json\r\n"
                  << "Content-Length: " << response_body.length() << "\r\n"
                  << "Connection: close\r\n\r\n"
                  << response_body;

    // Send the response
    std::string response = http_response.str();
    send(client_fd, response.c_str(), response.length(), 0);

    // Close the client socket
    close(client_fd);
}

int start_server(){
    // Create a server socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket error");
        return 1;
    }

    // Define the server address structure
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Allow port reuse in case of quick restarts
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    std::memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;  // Accept connections on any local IP
    address.sin_port = htons(PORT);        // Port to listen on

    // Bind the socket to the address/port
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        return 1;
    }

    // Start listening for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        return 1;
    }

    std::cout << "Sudoku Solver HTTP Server running at http://localhost:" << PORT << "\n";

    // Main server loop: accept and handle incoming client connections
    while (true) {
        int client_fd = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        if (client_fd < 0) {
            perror("Accept failed");
            continue;
        }

        // Delegate the work to a handler function for modularity
        handle_client(client_fd);
    }

    // Clean up
    close(server_fd);
    return 0;
}

// Sets up the network server to listen for incoming HTTP connections
int main() {
    start_server();
}