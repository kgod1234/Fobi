#ifndef DYNAMIXEL_PROPERTIES_H
#define DYNAMIXEL_PROPERTIES_H

#include "dynamixel_sdk.h" // Include the Dynamixel SDK

// Struct for control table addresses for XL430-W250-T model
struct XL430_W250_T {
    static const uint8_t PROTOCOL_VERSION = 2;            // Protocol 2.0 for XL430-W250-T

    static const uint16_t ADDR_TORQUE_ENABLE = 64;        // Torque Enable
    static const uint16_t ADDR_GOAL_POSITION = 116;       // Goal Position
    static const uint16_t ADDR_PRESENT_POSITION = 132;    // Present Position
    static const uint16_t ADDR_OPERATING_MODE = 11;       // Operating Mode
    static const uint16_t ADDR_VELOCITY_LIMIT = 44;       // Velocity Limit

    // Added addresses for control limits and mode
    static const uint16_t ADDR_MIN_POSITION_LIMIT = 52;   // Min Position Limit
    static const uint16_t ADDR_MAX_POSITION_LIMIT = 48;   // Max Position Limit
    static const uint16_t ADDR_DRIVE_MODE = 10;           // Drive Mode

    // Len
    static const int LEN_POSITION = 4;

};

// Struct for control table addresses for AX-12 model
struct AX_12 {
    static const uint8_t PROTOCOL_VERSION = 1;            // Protocol 1.0 for AX-12
    
    static const uint16_t ADDR_TORQUE_ENABLE = 24;        // Torque Enable
    static const uint16_t ADDR_GOAL_POSITION = 30;        // Goal Position
    static const uint16_t ADDR_PRESENT_POSITION = 36;     // Present Position
    static const uint16_t ADDR_OPERATING_MODE = 11;       // Operating Mode
    static const uint16_t ADDR_VELOCITY_LIMIT = 32;       // Velocity Limit

    // Added addresses for control limits and mode
    static const uint16_t ADDR_MIN_POSITION_LIMIT = 14;   // Min Position Limit
    static const uint16_t ADDR_MAX_POSITION_LIMIT = 15;   // Max Position Limit
    static const uint16_t ADDR_DRIVE_MODE = 10;           // Drive Mode

};

#endif // DYNAMIXEL_PROPERTIES_H


