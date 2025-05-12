#include <iostream>
#include <string>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

int main() {
    // Inicializar Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Error al inicializar Winsock" << std::endl;
        return -1;
    }

    // Crear un socket
    SOCKET server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_fd == INVALID_SOCKET) {
        std::cerr << "Error al crear el socket: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return -1;
    }

    // Configurar la dirección del servidor
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080); // Puerto en el que escucha el servidor

    // Enlazar el socket a la dirección
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "Error al enlazar el socket: " << WSAGetLastError() << std::endl;
        closesocket(server_fd);
        WSACleanup();
        return -1;
    }

    // Escuchar conexiones entrantes
    if (listen(server_fd, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Error al escuchar conexiones: " << WSAGetLastError() << std::endl;
        closesocket(server_fd);
        WSACleanup();
        return -1;
    }

    std::cout << "Servidor escuchando en el puerto 8080..." << std::endl;

    while (true) {
        // Aceptar una conexión entrante
        SOCKET client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd == INVALID_SOCKET) {
            std::cerr << "Error al aceptar la conexión: " << WSAGetLastError() << std::endl;
            continue;
        }

        // Leer la solicitud del cliente
        char buffer[1024] = {0};
        recv(client_fd, buffer, 1024, 0);
        std::cout << "Solicitud recibida: " << buffer << std::endl;

        // Respuesta HTTP simple
        std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nHola, peñita desde C++!";
        send(client_fd, response.c_str(), response.size(), 0);

        // Cerrar la conexión con el cliente
        closesocket(client_fd);
    }

    // Cerrar el socket del servidor
    closesocket(server_fd);
    WSACleanup();
    return 0;
}
