#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <thread>
#include <mutex>
#include <arpa/inet.h>
#include <sstream>
#include <unistd.h>
#include <algorithm>
#include <chrono>
#include <iomanip>

#define PORT 8081
#define BUFFER_SIZE 1024

typedef struct {
    int id;
    std::string msg;
    std::string timestamp; // Thêm trường thời gian
} Data;

std::vector<int> clients;
std::mutex clients_mutex;
int client_id_counter = 0;  // Biến đếm ID của client

std::string getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string serializeData(const Data& data) {
    std::ostringstream oss;
    oss << data.id << "|" << data.msg << "|" << data.timestamp; // Thêm timestamp vào chuỗi
    return oss.str();
}

Data deserializeData(const std::string& serializedData) {
    std::istringstream iss(serializedData);
    Data data;
    std::string id_str;
    
    std::getline(iss, id_str, '|'); // Đọc id
    data.id = std::stoi(id_str);
    std::getline(iss, data.msg, '|'); // Đọc message
    std::getline(iss, data.timestamp); // Đọc timestamp
    return data;
}

void broadcastMessage(const std::string& message, int sender_socket) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (int client_socket : clients) {
        if (client_socket != sender_socket) {
            send(client_socket, message.c_str(), message.size(), 0);
        }
    }
}

void handleClient(int client_socket, int client_id) {
    char buffer[BUFFER_SIZE];
    while (true) {
        int valread = read(client_socket, buffer, BUFFER_SIZE);
        if (valread > 0) {
            std::string serializedData(buffer, valread);
            Data data = deserializeData(serializedData);

            // Hiển thị tin nhắn từ client trên terminal cùng với thời gian
            std::cout << "[" << data.timestamp << "] Message from Client " << client_id << ": " << data.msg << std::endl;
            
            // Phát tán tin nhắn cho các client khác
            broadcastMessage(serializedData, client_socket);

            memset(buffer, 0, BUFFER_SIZE);
        } else {
            std::lock_guard<std::mutex> lock(clients_mutex);
            clients.erase(std::remove(clients.begin(), clients.end(), client_socket), clients.end());
            close(client_socket);
            break;
        }
    }
}

int main(int argc, char** argv) {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    std::cout << "Server is listening on port " << PORT << std::endl;

    while (true) {
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept failed");
            continue;
        }

        {
            std::lock_guard<std::mutex> lock(clients_mutex);
            clients.push_back(new_socket);
            client_id_counter++;  // Tăng biến đếm ID
        }

        std::cout << "New client connected: " << new_socket << " with ID: " << client_id_counter << std::endl;
        std::thread(handleClient, new_socket, client_id_counter).detach();  // Gửi client_id cho hàm handleClient
    }

    return 0;
}
