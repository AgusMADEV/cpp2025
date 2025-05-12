#include <iostream>
#include <string>
#include <winsock2.h>
#include <conio.h> // For kbhit() and getch()
#include <fstream>
#include <sstream>

#pragma comment(lib, "ws2_32.lib")

std::string readFile(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        return "";
    }

    std::ostringstream content;
    content << file.rdbuf();
    return content.str();
}

int main() {
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Error initializing Winsock" << std::endl;
        return -1;
    }

    // Create a socket
    SOCKET server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_fd == INVALID_SOCKET) {
        std::cerr << "Error creating socket: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return -1;
    }

    // Configure the server address
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080); // Port on which the server listens

    // Bind the socket to the address
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "Error binding socket: " << WSAGetLastError() << std::endl;
        closesocket(server_fd);
        WSACleanup();
        return -1;
    }

    // Listen for incoming connections
    if (listen(server_fd, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Error listening for connections: " << WSAGetLastError() << std::endl;
        closesocket(server_fd);
        WSACleanup();
        return -1;
    }

    std::cout << "Server listening on port 8080... Press 'q' to quit." << std::endl;

    bool running = true;
    while (running) {
        // Check if the 'q' key is pressed
        if (_kbhit()) {
            char key = _getch();
            if (key == 'q') {
                running = false;
                break;
            }
        }

        // Accept an incoming connection
        SOCKET client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd == INVALID_SOCKET) {
            std::cerr << "Error accepting connection: " << WSAGetLastError() << std::endl;
            continue;
        }

        // Read the client's request
        char buffer[1024] = {0};
        recv(client_fd, buffer, 1024, 0);
        std::cout << "Request received: " << buffer << std::endl;

        // Parse the request to get the file path
        std::string request(buffer);
        size_t spacePos = request.find(' ');
        if (spacePos != std::string::npos) {
            std::string filePath = request.substr(spacePos + 1);
            size_t pathEnd = filePath.find(' ');
            if (pathEnd != std::string::npos) {
                filePath = filePath.substr(0, pathEnd);
                if (filePath.empty() || filePath == "/") {
                    filePath = "index.html"; // Default file
                }

                // Read the file from the public directory
                std::string fullPath = "public/" + filePath;
                std::string fileContent = readFile(fullPath);

                if (!fileContent.empty()) {
                    // Simple HTTP response with the file content
                    std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + fileContent;
                    send(client_fd, response.c_str(), response.size(), 0);
                } else {
                    // File not found response
                    std::string response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nFile not found";
                    send(client_fd, response.c_str(), response.size(), 0);
                }
            }
        }

        // Close the connection with the client
        closesocket(client_fd);
    }

    // Close the server socket
    closesocket(server_fd);
    WSACleanup();

    // Keep the console window open until a key is pressed
    std::cout << "Press any key to exit..." << std::endl;
    _getch();

    return 0;
}
