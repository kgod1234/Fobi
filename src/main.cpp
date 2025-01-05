#if defined(__linux__) || defined(__APPLE__)
#include <fcntl.h>
#include <termios.h>
#define STDIN_FILENO 0
#elif defined(_WIN32) || defined(_WIN64)
#include <conio.h>
#endif

#include <stdlib.h>
#include <stdio.h>

#include "dynamixel_sdk.h"
#include "Mylibs/SimplifyDynamixel.h"                                  // Uses Dynamixel SDK library

// Default setting
#define BAUDRATE                        57600
#define DEVICENAME                      "/dev/ttyUSB0"      // Check which port is being used on your controller
                                                            // ex) Windows: "COM1"   Linux: "/dev/ttyUSB0" Mac: "/dev/tty.usbserial-*"
#define ESC_ASCII_VALUE                 0x1b

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

int main() {
    int index = 0;
    int id[2] = {1,2};
    int target[2][2] = {{0,0},{4095,4095}};
    
    // Create a Dynamixel object and specify the device name and baud rate
    Dynamixel dynamixel("/dev/ttyUSB0",57600);

    // Open the port and set the baud rate
    dynamixel.OpenPort();

    int motorid = 1;
    dynamixel.TorqueEnable("XL430_W250_T", 1);
    dynamixel.TorqueEnable("XL430_W250_T", 2);

    while(1) {
        printf("Press any key to continue! (or press ESC to quit!)\n");
        if (getch() == ESC_ASCII_VALUE) {  // Wait for a key press
            break;  // Exit the loop if ESC is pressed
        }
        dynamixel.SyncDriveTo("XL430_W250_T", id, target[index]);
        if(index == 1){
          index = 0;
        }
        else{
          index = 1;
        }
    }

    dynamixel.TorqueDisable("XL430_W250_T", 1);
    dynamixel.TorqueDisable("XL430_W250_T", 2);

    return 0;
}
