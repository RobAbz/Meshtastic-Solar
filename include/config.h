#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// Hardware Configuration
#define RELAY_PIN WB_IO4              // IO4 for relay control
#define BUTTON_PIN WB_IO6             // IO6 for button input
#define BUTTON_PRESSED LOW            // Button pressed state (active low with pull-up)

// I2C Configuration for RAK1921 OLED (on Slot A of RAK19007)
#define OLED_I2C_ADDRESS 0x3C         // Standard I2C address for SSD1306
#define SCREEN_WIDTH 128              // OLED display width, in pixels
#define SCREEN_HEIGHT 64              // OLED display height, in pixels

// Display Configuration
#define MAX_DISPLAY_LINES 7           // Maximum lines on display (64px / 8px font = 8, reserve 1 for header)
#define LINE_HEIGHT 9                 // 8px font + 1px spacing
#define HEADER_HEIGHT 10              // Height reserved for header
#define MAX_DISPLAY_CHARS 21          // Maximum characters per line on display
#define TRUNCATE_CHARS 18             // Characters to keep when truncating (leaving room for "...")

// Emergency Signal Messages
#define EMERGENCY_ON_MSG "Emergency_signal ON"
#define EMERGENCY_OFF_MSG "Emergency_signal OFF"
#define ACK_MSG "Emergency_signal ACK"

// Timing Configuration
#define BUTTON_DEBOUNCE_MS 50         // Button debounce time in milliseconds
#define DISPLAY_UPDATE_MS 100         // Display update interval
#define MESSAGE_DISPLAY_TIME_MS 30000 // How long to show a message (30 seconds)

// Whitelist Configuration
// Public keys of authorized nodes
// Format: Base64 encoded public keys
const char* WHITELIST[] = {
    "0mmK16HVkgbaV+2Pb+RLa4gStG1RCkmx-WWKRh12dlhY=",
    // Add more public keys here as needed
};
const int WHITELIST_SIZE = sizeof(WHITELIST) / sizeof(WHITELIST[0]);

#endif // CONFIG_H
