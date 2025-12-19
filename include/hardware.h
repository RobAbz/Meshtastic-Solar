#ifndef HARDWARE_H
#define HARDWARE_H

#include <Arduino.h>
#include "config.h"

/**
 * Hardware Management Module
 * Handles button input, relay control, and power management
 */

class Hardware {
public:
    /**
     * Initialize hardware components
     */
    static void init();
    
    /**
     * Check button state (analog read with debouncing)
     * Returns true if button was pressed
     */
    static bool checkButton();
    
    /**
     * Set relay state
     * @param state true = ON (engaged), false = OFF (disengaged)
     */
    static void setRelay(bool state);
    
    /**
     * Get current relay state
     */
    static bool getRelayState();
    
    /**
     * Set CPU speed
     * @param mhz Frequency in Hz (e.g., 64000000 for 64MHz)
     */
    static void setCPUSpeed(uint32_t hz);
    
    /**
     * Enter deep sleep mode
     * Wakes on button press or external interrupt
     */
    static void enterDeepSleep();
    
    /**
     * Get battery voltage in millivolts
     */
    static uint16_t getBatteryVoltage();
    
    /**
     * Get battery percentage (0-100)
     */
    static uint8_t getBatteryPercent();

private:
    static bool relayState;
    static uint32_t lastButtonCheck;
    static uint32_t buttonPressStart;
    static bool buttonWasPressed;
    static int lastButtonReading;
};

#endif // HARDWARE_H
