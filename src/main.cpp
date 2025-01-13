#if defined(__linux__) || defined(__APPLE__)
#include <fcntl.h>
#include <termios.h>
#define STDIN_FILENO 0
#elif defined(_WIN32) || defined(_WIN64)
#include <conio.h>
#endif

#include "udp_common.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "dynamixel_sdk.h"
#include "Mylibs\SimplifyDynamixel.h"                                  // Uses Dynamixel SDK library

// Default setting
#define BAUDRATE                        57600
#define DEVICENAME                      "COM7"      // Check which port is being used on your controller
                                                            // ex) Windows: "COM1"   Linux: "/dev/ttyUSB0" Mac: "/dev/tty.usbserial-*"
#define ESC_ASCII_VALUE                 0x1b
#define BAUDRATE 57600         // Dynamixel Baud Rate (example)


int getch()
{
#if defined(__linux__) || defined(__APPLE__)
  struct termios oldt, newt;
  int ch;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  ch = getchar();
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  return ch;
#elif defined(_WIN32) || defined(_WIN64)
  return _getch();
#endif
}

int kbhit(void)
{
#if defined(__linux__) || defined(__APPLE__)
  struct termios oldt, newt;
  int ch;
  int oldf;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if (ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }

  return 0;
#elif defined(_WIN32) || defined(_WIN64)
  return _kbhit();
#endif
}

void runClient() {
    // Create a Dynamixel object and specify the device name and baud rate
    Dynamixel dynamixel("COM7");

    // Open the port and set the baud rate
    dynamixel.SetBAUDRATE(BAUDRATE);
    dynamixel.OpenPort();

    dynamixel.TorqueEnable("XL430_W250_T", 1);

    dynamixel.ChangeMode("XL430_W250_T", "position", 1); // For PID Test using Position Control
    dynamixel.SetPIDGains("XL430_W250_T", 1, 50, 0, 10);

    initializeSockets();

    SOCKET clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed!" << std::endl;
        cleanupSockets();
        return;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(9999);

    sockaddr_in recvAddr{};
    socklen_t recvLen = sizeof(recvAddr);

    char buffer[1024];
    std::cout << "Client is ready to receive commands." << std::endl;

    // Send initial connection message to server
    std::string initMsg = "HELLO";
    sendto(clientSocket, initMsg.c_str(), initMsg.size(), 0, (sockaddr*)&serverAddr, sizeof(serverAddr));

    while (true) {
        // Receive command
        int recvResult = recvfrom(clientSocket, buffer, sizeof(buffer) - 1, 0, 
                                  (sockaddr*)&recvAddr, &recvLen);

        if (recvResult == SOCKET_ERROR) {
            int error_code = WSAGetLastError();
            std::cerr << "Error receiving data: " << error_code << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1)); // Avoid tight loop
            continue;
        }

        if (recvResult > 0) {
            buffer[recvResult] = '\0';
            std::string command(buffer);
            std::cout << "Received: " << command << std::endl;

            // Simulate command execution
            // std::this_thread::sleep_for(std::chrono::seconds(2));
            dynamixel.DriveTo("XL430_W250_T", 1, stoi(command));
            std::cout << "Executed: " << command << std::endl;

            // Send acknowledgment
            std::string ack = "ACK";
            int ackResult = sendto(clientSocket, ack.c_str(), ack.size(), 0, (sockaddr*)&serverAddr, sizeof(serverAddr));
            if (ackResult == SOCKET_ERROR) {
                std::cerr << "Failed to send acknowledgment!" << std::endl;
            } else {
                std::cout << "Acknowledgment sent successfully." << std::endl;
            }
        }
    }

    std::cout << "Client is shutting down." << std::endl;
    closesocket(clientSocket);
    cleanupSockets();
}

int main() {
    runClient();
    return 0;
}