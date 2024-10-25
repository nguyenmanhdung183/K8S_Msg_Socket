#include <iostream>
#include <cstring>
#include <thread>
#include<string>
#include <winsock2.h> // Sử dụng winsock2.h cho Windows
#include <sstream> // Thư viện để tuần tự hóa và giải mã dữ liệu
#include <chrono> // Thư viện để lấy thời gian
#include <iomanip> // Thư viện để định dạng thời gian
#pragma comment(lib, "Ws2_32.lib") // Liên kết thư viện Winsock2
//   g++ -o client client.cpp -lws2_32
#define PORT 57336
#define BUFFER_SIZE 1024

typedef struct {
    int id;
    std::string msg;
    std::string timestamp; // Thêm trường thời gian
} Data;

std::string getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string serializeData(const Data& data) {
    std::ostringstream oss;
    oss << data.id << "|" << data.msg << "|" << data.timestamp; // Thay đổi để bao gồm timestamp
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

void receiveMessages(SOCKET sock) {
    char buffer[BUFFER_SIZE];
    while (true) {
        int valread = recv(sock, buffer, BUFFER_SIZE, 0);
        if (valread > 0) {
            std::string serializedData(buffer, valread);
            Data data = deserializeData(serializedData);
            std::cout << "[" << data.timestamp << "] Client " << data.id << ": " <<"\t"<< data.msg <<"\n"<< std::endl;
        }
        memset(buffer, 0, BUFFER_SIZE);
    }
}

void clearInputLine() {
    // Di chuyển con trỏ về đầu dòng và xóa
    std::cout << "\r"; // Di chuyển con trỏ về đầu dòng
    std::cout << std::string(50, ' '); // Xóa nội dung dòng
    std::cout << "\r"; // Di chuyển con trỏ về đầu dòng một lần nữa
}

int main(int argc, char** argv) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Winsock initialization failed" << std::endl;
        return -1;
    }

    if (argc < 2) {
        std::cerr << "Usage: ./client <ID>" << std::endl;
        return -1;
    }

    int id = std::stoi(argv[1]);
    SOCKET sock = 0;
    struct sockaddr_in serv_addr;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        std::cerr << "Socket creation error" << std::endl;
        WSACleanup();
        return -1;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        closesocket(sock);
        WSACleanup();
        return -1;
    }

    std::cout << "Connected to the server as Client ID: " << id << " - with PORT :" <<PORT<<std::endl;

    std::thread(receiveMessages, sock).detach();

    while (true) {
        std::string message;
        //std::cout << "ENTER MSG: ";
        std::getline(std::cin, message);

        // Lấy thời gian hiện tại khi gửi tin nhắn
        std::string currentTime = getCurrentTime();
        Data data{id, message, currentTime}; // Thêm thời gian vào dữ liệu
        std::string serializedData = serializeData(data);

        send(sock, serializedData.c_str(), serializedData.size(), 0);

        // Hiển thị thời gian gửi tin nhắn trên terminal
        clearInputLine(); // Xóa dòng nhập
        std::cout << "[" << currentTime << "] You sent: \t" << message <<"\n"<< std::endl;
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
