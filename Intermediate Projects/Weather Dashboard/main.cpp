#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>

// Task 1: Replace with your actual OpenWeatherMap API key
const std::string API_KEY = "ff678986574daff67cce6f0c0a35e20c";  // Insert API key here

// Extracts the value for a given key from a JSON-like string
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

// Connects to a host over TCP on the specified port
int connect_to_host(const std::string& host, int port) {
    struct hostent* server = gethostbyname(host.c_str());
    if (!server) {
        std::cerr << "Error: No such host.\n";
        return -1;
    }

    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("Socket error");
        return -1;
    }

    struct sockaddr_in server_addr {};
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    std::memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    server_addr.sin_port = htons(port);

    if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection error");
        close(sock_fd);
        return -1;
    }

    return sock_fd;
}

// Sends an HTTP request to get weather data for a city
std::string fetch_weather(const std::string& city) {
    const std::string host = "api.openweathermap.org";
    int port = 80;

    int sock = connect_to_host(host, port);
    if (sock == -1) return "ERROR";

    std::ostringstream request;
    request << "GET /data/2.5/weather?q=" << city
            << "&appid=" << API_KEY << "&units=metric HTTP/1.1\r\n"
            << "Host: " << host << "\r\n"
            << "Connection: close\r\n\r\n";

    std::string req_str = request.str();
    send(sock, req_str.c_str(), req_str.length(), 0);

    char buffer[8192];
    std::string response;
    ssize_t bytes_read;

    while ((bytes_read = read(sock, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0';
        response += buffer;
    }

    close(sock);

    // Task 4: Remove the HTTP headers (everything before the JSON body)
    std::size_t header_end = response.find("\r\n\r\n");
    if (header_end != std::string::npos) {
        response = response.substr(header_end + 4);
    }

    return response;
}

// Extract city name from posted JSON body
std::string extract_city(const std::string& body) {
    return extract_value(body, "city");   
}

// Task 5: Build JSON response string to send back to client
std::string build_response_json(const std::string& city, const std::string& weather) {
    std::string temp = extract_value(weather, "temp");
    std::string desc = extract_value(weather, "description");
    std::string hum  = extract_value(weather, "humidity");

    std::ostringstream response;
    response << "{"
             << "\"City\":\"" << city << "\","
             << "\"Temperature\":" << temp << ","
             << "\"Description\":\"" << desc << "\","
             << "\"Humidity\":" << hum
             << "}";
    return response.str();
}

// Handles a single client connection
void handle_client(int client_fd) {
    char buffer[4096] = {0};
    read(client_fd, buffer, sizeof(buffer));
    
    std::string request(buffer);

    // Task 6: Check HTTP request method and path
    if (request.find("POST /weather") != 0) {  
        std::string not_found = "HTTP/1.1 404 Not Found\r\n\r\n";
        send(client_fd, not_found.c_str(), not_found.size(), 0);
        close(client_fd);
        return;
    }

    // Task 7: Locate and extract the JSON body
    std::size_t json_start = request.find("\r\n\r\n");
    if (json_start == std::string::npos) {
        std::string bad = "HTTP/1.1 400 Bad Request\r\n\r\n";
        send(client_fd, bad.c_str(), bad.size(), 0);
        close(client_fd);
        return;
    }

    std::string body = request.substr(json_start + 4);

    std::string city = extract_city(body);

    std::string weather_data = fetch_weather(city);

    // Task 8: Check validity of weather response (look for a key like "main")
    if (weather_data.find("\"main\"") == std::string::npos) {
        std::string error = "HTTP/1.1 400 Bad Request\r\n\r\n{\"error\":\"Invalid city\"}";
        send(client_fd, error.c_str(), error.length(), 0);
        close(client_fd);
        return;
    }

    std::string response_body = build_response_json(city, weather_data);

    std::ostringstream http_response;
    http_response << "HTTP/1.1 200 OK\r\n"
                  << "Content-Type: application/json\r\n"
                  << "Access-Control-Allow-Origin: *\r\n" // <-- Add this line
                  << "Content-Length: " << response_body.length() << "\r\n\r\n"
                  << response_body;

    std::string full_response = http_response.str();
    send(client_fd, full_response.c_str(), full_response.length(), 0);

    close(client_fd);
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket error");
        return 1;
    }

    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Allow immediate reuse of the port after program exits
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    std::memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);    

    // Bind socket to that address and port
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        return 1;
    }

    if (listen(server_fd, 5) < 0) {   // Max backlog of pending connections, e.g., 3
        perror("Listen failed");
        return 1;
    }

    std::cout << "Server running at http://localhost:8080\n";  // Replace PORT with actual number

    while (true) {
        int client_fd = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        if (client_fd < 0) {
            perror("Accept failed");
            continue;
        }

        handle_client(client_fd);
    }

    close(server_fd);
    return 0;
}