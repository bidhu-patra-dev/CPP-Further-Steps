// main.hpp
#ifndef MAIN_HPP
#define MAIN_HPP

#include <string>

std::string fetch_weather(const std::string& city);
std::string extract_city(const std::string& json);
void handle_client(int client_fd);  // For testing, you will use mock file descriptors

#endif // MAIN_HPP