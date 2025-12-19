#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <U8g2lib.h>
#include "config.h"

/**
 * Display Management Module
 * Handles OLED display output and power management
 */

class Display {
public:
    /**
     * Initialize display
     */
    static void init();
    
    /**
     * Show boot splash screen
     * Displays Meshtastic logo, copyright, and compile time
     */
    static void showBootSplash();
    
    /**
     * Update display with current status
     * @param relayState Current relay state
     * @param message Status message to display
     * @param timestamp Optional timestamp string
     */
    static void updateStatus(bool relayState, const char* message, const char* timestamp = nullptr);
    
    /**
     * Turn display on
     */
    static void turnOn();
    
    /**
     * Turn display off (power saving)
     */
    static void turnOff();
    
    /**
     * Check if display should auto-off
     * Called periodically to manage display timeout
     */
    static void checkAutoOff();
    
    /**
     * Get display state
     */
    static bool isOn();
    
    /**
     * Clear display
     */
    static void clear();
    
    /**
     * Draw custom text at position
     */
    static void drawText(uint8_t x, uint8_t y, const char* text);
    
    /**
     * Force display update
     */
    static void update();

private:
    static U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;
    static bool displayOn;
    static uint32_t lastActivityTime;
    static bool relayWasOn;
    
    /**
     * Draw Meshtastic logo (simplified version)
     */
    static void drawLogo();
    
    /**
     * Draw centered text
     */
    static void drawCentered(uint8_t y, const char* text);
};

#endif // DISPLAY_H
