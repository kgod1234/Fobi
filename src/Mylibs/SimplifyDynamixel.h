#ifndef SIMPLIFY_DYNAMIXEL_H
#define SIMPLIFY_DYNAMIXEL_H

#include "dynamixel_sdk.h" // include dynamixel SDK libs
#include "DynamixelProperties.h"  
#include <string>

using namespace std;

class Dynamixel {
public :
     // Constructor to initialize the port and packet handlers
    Dynamixel(std::string deviceName, int baudRate = 115200);

    // Destructor to clean up resources
    ~Dynamixel();
    // Setup
    // Set the PortHandler and PacketHandler
    void OpenPort();
    // Control Function
    void TorqueEnable(string name, int id[]);
    void TorqueEnable(string name, int id);
    void TorqueDisable(string name, int id[]);
    void TorqueDisable(string name, int id);

    void DriveTo(string name ,int id, int pos[]); // to write single dynamixel to goal position
    void SyncDriveTo(string name ,int id[], int pos[]); // to write multi dynamixel to goal position

    // Value Setting 
    int GetBAUDRATE() const { return BAUDRATE; }
    void SetBAUDRATE(int baudrate) { BAUDRATE = baudrate; }
private :
    dynamixel::PortHandler *portHandler;
    dynamixel::PacketHandler *packetHandler;
    int BAUDRATE = 115200;
};

#endif // MY_LIBRARY_H