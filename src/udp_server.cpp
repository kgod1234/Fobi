#include "udp_common.h"

void runServer() {
    initializeSockets();

    SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed!" << std::endl;
        cleanupSockets();
        return;
    }

    sockaddr_in serverAddr{}, clientAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(9999);

    bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));

    char buffer[1024];
    socklen_t clientLen = sizeof(clientAddr);

    std::cout << "Server is ready to receive user commands..." << std::endl;

    // First, receive the "HELLO" message from the client
    int recvLen = recvfrom(serverSocket, buffer, sizeof(buffer), 0, (sockaddr*)&clientAddr, &clientLen);
    if (recvLen > 0) {
        buffer[recvLen] = '\0';
        std::string initMessage(buffer);
        if (initMessage == "HELLO") {
            std::cout << "Received initial connection message: " << initMessage << std::endl;
        } else {
            std::cerr << "Unexpected message received: " << initMessage << std::endl;
            closesocket(serverSocket);
            cleanupSockets();
            return;
        }
    } else {
        std::cerr << "Failed to receive the 'HELLO' message!" << std::endl;
        closesocket(serverSocket);
        cleanupSockets();
        return;
    }

    // Now, enter the loop to process further commands
    while (true) {
        std::string userCommand;
        std::cout << "Enter command to send to client (or type 'STOP' to end): ";
        std::getline(std::cin, userCommand);

        if (userCommand == "STOP") {
            std::cout << "Server is stopping..." << std::endl;
            break;
        }

        // Send user command to client
        sendto(serverSocket, userCommand.c_str(), userCommand.size(), 0, (sockaddr*)&clientAddr, sizeof(clientAddr));
        std::cout << "Sent: " << userCommand << std::endl;

        // Receive acknowledgment from client
        recvLen = recvfrom(serverSocket, buffer, sizeof(buffer), 0, (sockaddr*)&clientAddr, &clientLen);
        if (recvLen > 0) {
            buffer[recvLen] = '\0';
            std::string ackMessage(buffer);
            std::cout << "Acknowledgment from client: " << ackMessage << std::endl;
        } else {
            std::cerr << "Failed to receive acknowledgment!" << std::endl;
        }
    }

    closesocket(serverSocket);
    cleanupSockets();
}

int main() {
    runServer();
    return 0;
}
