#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ============================================================================
// HARDWARE CONFIGURATION
// ============================================================================

// Button Configuration (AIN1 - Analog Input)
#define BUTTON_PIN WB_A1              // AIN1 on RAK4631
#define BUTTON_THRESHOLD_LOW 512      // <0.5V = not pressed (ADC value ~0-512)
#define BUTTON_THRESHOLD_HIGH 2900    // >2.9V = pressed (ADC value ~2900-4095)
#define BUTTON_DEBOUNCE_MS 1000       // 1 second debounce as specified
#define BUTTON_READ_INTERVAL_MS 50    // How often to check button

// Relay Configuration (RAK13007 on IO Slot)
// Note: RAK13007 uses WB_IO2 (IO Slot)
#define RELAY_PIN WB_IO2              // IO Slot pin for RAK13007
#define RELAY_ON HIGH                 // HIGH = relay engaged
#define RELAY_OFF LOW                 // LOW = relay disengaged

// OLED Display Configuration
#define OLED_I2C_ADDRESS 0x3C         // Standard I2C address for SSD1306
#define SCREEN_WIDTH 128              // OLED display width, in pixels
#define SCREEN_HEIGHT 64              // OLED display height, in pixels
#define DISPLAY_TIMEOUT_MS 30000      // Turn off display 30s after relay OFF

// I2C Pins (built-in on RAK4631)
#define I2C_SDA WB_I2C1_SDA
#define I2C_SCL WB_I2C1_SCL

// ============================================================================
// SYSTEM CONFIGURATION
// ============================================================================

// Power Management
#define CPU_SPEED_MEDIUM 64000000     // 64 MHz (medium speed)
#define CPU_SPEED_LOW 16000000        // 16 MHz (low speed for power saving)
#define SLEEP_MODE_ENABLED true       // Enable deep sleep when idle

// Transmit Power Settings
typedef enum {
    TX_POWER_LOW = 0,      // Low power
    TX_POWER_MEDIUM = 4,   // Medium power (default)
    TX_POWER_HIGH = 8      // High power
} TransmitPower;

#define DEFAULT_TX_POWER TX_POWER_MEDIUM

// ============================================================================
// MESHTASTIC CONFIGURATION
// ============================================================================

// Message Protocol
#define MSG_EMERGENCY_ON "Emergency_Shutdown ON"
#define MSG_EMERGENCY_OFF "Emergency_Shutdown OFF"
#define MSG_ACK "ACK"

// Message Handling
#define MESSAGE_RETRY_TIMEOUT_MS 60000  // 1 minute before retry
#define MAX_MESSAGE_RETRIES 3           // Maximum number of retry attempts

// Whitelist Configuration
// Starting with one public key, more can be added via web interface
#define MAX_WHITELIST_NODES 20          // Maximum nodes in whitelist
#define NODE_NAME_MAX_LENGTH 20         // Max ASCII characters for node name

// Default whitelist entry
const char* DEFAULT_WHITELIST_KEY = "0mmK16HVgbaV+2Pb+RLa4ggStG1RCkmx-WwkRh12d1hY=";

// ============================================================================
// DISPLAY CONFIGURATION
// ============================================================================

// Font Configuration (8 pixel height as specified)
// Using u8g2_font_t0_11_mf as specified in original request
// Alternative: u8g2_font_scrum_tr (also 8px height)
#define DISPLAY_FONT u8g2_font_scrum_tr
#define FONT_HEIGHT 8
#define LINE_SPACING 2
#define MAX_DISPLAY_LINES 6            // Number of text lines on 64px display

// Display Messages
#define DISPLAY_MSG_EMERGENCY_ON "Emergency Shutdown"
#define DISPLAY_MSG_EMERGENCY_INITIATED "Initiated"
#define DISPLAY_MSG_EMERGENCY_OFF "Emergency Shutdown"
#define DISPLAY_MSG_EMERGENCY_CANCELLED "Cancelled"
#define DISPLAY_MSG_BOOT_TITLE "Meshtastic-Solar"
#define DISPLAY_MSG_COPYRIGHT "© SilverTime 2025"

// Boot splash duration
#define BOOT_SPLASH_DURATION_MS 30000  // 30 seconds

// ============================================================================
// WEB SERVER CONFIGURATION
// ============================================================================

#define WEB_SERVER_PORT 80
#define WEB_UPDATE_INTERVAL_MS 1000    // Update web data every second
#define MAX_LOG_ENTRIES 100            // Maximum message log entries

// ============================================================================
// PREFERENCES/NVRAM KEYS
// ============================================================================

#define PREFS_NAMESPACE "meshtastic_solar"
#define PREFS_KEY_WHITELIST "whitelist"
#define PREFS_KEY_NODE_NAMES "node_names"
#define PREFS_KEY_RELAY_STATE "relay_state"
#define PREFS_KEY_CPU_SPEED "cpu_speed"
#define PREFS_KEY_TX_POWER "tx_power"
#define PREFS_KEY_DATETIME "datetime"

// ============================================================================
// TIMING CONFIGURATION
// ============================================================================

#define LOOP_DELAY_MS 10              // Main loop delay
#define WATCHDOG_TIMEOUT_MS 30000     // Watchdog timeout

// ============================================================================
// DEBUG CONFIGURATION
// ============================================================================

#define DEBUG_SERIAL_ENABLED true
#define DEBUG_SERIAL_BAUD 115200

#if DEBUG_SERIAL_ENABLED
    #define DEBUG_PRINT(x) Serial.print(x)
    #define DEBUG_PRINTLN(x) Serial.println(x)
    #define DEBUG_PRINTF(fmt, ...) Serial.printf(fmt, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(x)
    #define DEBUG_PRINTLN(x)
    #define DEBUG_PRINTF(fmt, ...)
#endif

// ============================================================================
// COMPILE-TIME INFORMATION
// ============================================================================

// These are defined in platformio.ini build_flags
#ifndef COMPILE_DATE
#define COMPILE_DATE __DATE__
#endif

#ifndef COMPILE_TIME
#define COMPILE_TIME __TIME__
#endif

#endif // CONFIG_H
