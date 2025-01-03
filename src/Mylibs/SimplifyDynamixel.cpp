#include "SimplifyDynamixel.h"
#include "DynamixelProperties.h"
#include <iostream>

// Constructor to initialize PortHandler, PacketHandler, and set the baud rate
Dynamixel::Dynamixel(std::string deviceName, int baudRate) {
    // Set the baud rate (default is 115200)
    BAUDRATE = baudRate;

    // Initialize PortHandler with the device name (e.g., "COM1" or "/dev/ttyUSB0")
    portHandler = dynamixel::PortHandler::getPortHandler(deviceName.c_str());

    // Initialize PacketHandler (use protocol version 1.0 or 2.0 depending on the motor)
    packetHandler = dynamixel::PacketHandler::getPacketHandler(2); // Example using Protocol 2.0
}

// Destructor to close the port and clean up
Dynamixel::~Dynamixel() {
    if (portHandler) {
        portHandler->closePort();  // Close the communication port when done
    }
}

// Open the communication port and set the baud rate
void Dynamixel::OpenPort() {
    if (portHandler->openPort()) {
        std::cout << "Succeeded to open the port!" << std::endl;
        
        // Set the baud rate
        if (portHandler->setBaudRate(BAUDRATE)) {
            std::cout << "Succeeded to set the baud rate to " << BAUDRATE << std::endl;
        } else {
            std::cerr << "Failed to set the baud rate!" << std::endl;
        }
    } else {
        std::cerr << "Failed to open the port!" << std::endl;
    }
}

// Torque Enable for both single and multiple motors based on name
void Dynamixel::TorqueEnable(std::string name, int id) {
    uint8_t dxl_error = 0;
    int result = 0;

    // Check which motor type is passed (name) and apply corresponding addresses
    if (name == "XL430_W250_T") {
        result = packetHandler->write1ByteTxRx(portHandler, id, XL430_W250_T::ADDR_TORQUE_ENABLE, 1, &dxl_error);
    } else if (name == "AX_12") {
        result = packetHandler->write1ByteTxRx(portHandler, id, AX_12::ADDR_TORQUE_ENABLE, 1, &dxl_error);
    }

    if (result != COMM_SUCCESS) {
        std::cerr << "Failed to enable torque on motor " << id << ": " << packetHandler->getTxRxResult(result) << std::endl;
    } else if (dxl_error != 0) {
        std::cerr << "Error in enabling torque on motor " << id << ": " << packetHandler->getRxPacketError(dxl_error) << std::endl;
    } else {
        std::cout << "Torque enabled for motor " << id << " (" << name << ")" << std::endl;
    }
}

// Torque Enable for multiple motors based on name (passing an array of IDs)
void Dynamixel::TorqueEnable(std::string name, int id[]) {
    for (int i = 0; id[i] != -1; i++) {  // Assuming -1 signals the end of the array
        TorqueEnable(name, id[i]);  // Call the single motor function for each motor in the array
    }
}

// Torque Disable for both single and multiple motors based on name
void Dynamixel::TorqueDisable(std::string name, int id) {
    uint8_t dxl_error = 0;
    int result = 0;

    // Check which motor type is passed (name) and apply corresponding addresses
    if (name == "XL430_W250_T") {
        result = packetHandler->write1ByteTxRx(portHandler, id, XL430_W250_T::ADDR_TORQUE_ENABLE, 0, &dxl_error);
    } else if (name == "AX_12") {
        result = packetHandler->write1ByteTxRx(portHandler, id, AX_12::ADDR_TORQUE_ENABLE, 0, &dxl_error);
    }

    if (result != COMM_SUCCESS) {
        std::cerr << "Failed to disable torque on motor " << id << ": " << packetHandler->getTxRxResult(result) << std::endl;
    } else if (dxl_error != 0) {
        std::cerr << "Error in disabling torque on motor " << id << ": " << packetHandler->getRxPacketError(dxl_error) << std::endl;
    } else {
        std::cout << "Torque disabled for motor " << id << " (" << name << ")" << std::endl;
    }
}

// Torque Disable for multiple motors based on name (passing an array of IDs)
void Dynamixel::TorqueDisable(std::string name, int id[]) {
    for (int i = 0; id[i] != -1; i++) {  // Assuming -1 signals the end of the array
        TorqueDisable(name, id[i]);  // Call the single motor function for each motor in the array
    }
}

void Dynamixel::SyncDriveTo(std::string name, int id[], int pos[]) {
    // Define control table parameters based on the motor model
    uint16_t goal_position_address;
    uint8_t goal_position_length;

    if (name == "XL430_W250_T") {
        goal_position_address = XL430_W250_T::ADDR_GOAL_POSITION; // Address for XL430
        goal_position_length = 4; // 4 bytes for XL430 goal position
    } 
    else if (name == "AX_12") {
        goal_position_address = AX_12::ADDR_GOAL_POSITION; // Address for AX-12
        goal_position_length = 2; // 2 bytes for AX-12 goal position
    } 
    else {
        std::cerr << "Unknown motor model: " << name << std::endl;
        return;
    }

    // Initialize GroupSyncWrite with the determined address and length
    dynamixel::GroupSyncWrite groupSyncWrite(portHandler, packetHandler, goal_position_address, goal_position_length);

    // Iterate over the provided IDs and positions, adding each to GroupSyncWrite
    for (int i = 0; i < sizeof(id)/sizeof(id[0]); i++) {
        uint8_t param_goal_position[4]; // Use a 4-byte array to handle both cases
        int32_t goal_position = pos[i];

        if (goal_position_length == 4) {
            // 4-byte position for models like XL430
            param_goal_position[0] = DXL_LOBYTE(DXL_LOWORD(goal_position));
            param_goal_position[1] = DXL_HIBYTE(DXL_LOWORD(goal_position));
            param_goal_position[2] = DXL_LOBYTE(DXL_HIWORD(goal_position));
            param_goal_position[3] = DXL_HIBYTE(DXL_HIWORD(goal_position));
        } else if (goal_position_length == 2) {
            // 2-byte position for models like AX-12
            param_goal_position[0] = DXL_LOBYTE(goal_position);
            param_goal_position[1] = DXL_HIBYTE(goal_position);
        }

        // Add the motor ID and its goal position to GroupSyncWrite
        if (!groupSyncWrite.addParam(id[i], param_goal_position)) {
            std::cerr << "Failed to add param for ID: " << id[i] << std::endl;
        }
    }

    // Transmit the packet to all motors
    if (groupSyncWrite.txPacket() != COMM_SUCCESS) {
        std::cerr << "Failed to transmit SyncWrite packet." << std::endl;
    }

    // Clear the parameter storage in GroupSyncWrite
    groupSyncWrite.clearParam();
}
