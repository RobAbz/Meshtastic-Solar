/**
 * Hardware Module Implementation
 * Handles button input, relay control, and power management
 */

#include "hardware.h"

// Static member initialization
bool Hardware::relayState = false;
uint32_t Hardware::lastButtonCheck = 0;
uint32_t Hardware::buttonPressStart = 0;
bool Hardware::buttonWasPressed = false;
int Hardware::lastButtonReading = 0;

void Hardware::init() {
    // Initialize button pin (analog input)
    pinMode(BUTTON_PIN, INPUT);
    DEBUG_PRINTLN("Button pin (AIN1) initialized");
    
    // Initialize relay pin (digital output)
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, RELAY_OFF);
    relayState = false;
    DEBUG_PRINTLN("Relay pin (IO2) initialized");
    
    // Set CPU speed to medium
    setCPUSpeed(CPU_SPEED_MEDIUM);
    DEBUG_PRINTLN("CPU speed set to medium (64MHz)");
    
    // Read initial button state
    lastButtonReading = analogRead(BUTTON_PIN);
    DEBUG_PRINTF("Initial button reading: %d\n", lastButtonReading);
}

bool Hardware::checkButton() {
    uint32_t currentTime = millis();
    
    // Check at specified interval
    if (currentTime - lastButtonCheck < BUTTON_READ_INTERVAL_MS) {
        return false;
    }
    lastButtonCheck = currentTime;
    
    // Read analog value from button pin
    int reading = analogRead(BUTTON_PIN);
    
    // Debug output (commented out to reduce spam)
    // DEBUG_PRINTF("Button reading: %d\n", reading);
    
    // Check if button is pressed (>2.9V = high reading)
    if (reading >= BUTTON_THRESHOLD_HIGH) {
        // Button is pressed
        if (!buttonWasPressed) {
            // Button just pressed
            buttonPressStart = currentTime;
            buttonWasPressed = true;
            DEBUG_PRINTLN("Button pressed (high voltage detected)");
        } else {
            // Button still pressed - check if held long enough
            if (currentTime - buttonPressStart >= BUTTON_DEBOUNCE_MS) {
                // Valid press detected (held for full debounce period)
                DEBUG_PRINTLN("Button press validated (>1 second)");
                buttonWasPressed = false;  // Reset for next press
                return true;
            }
        }
    } else if (reading <= BUTTON_THRESHOLD_LOW) {
        // Button is not pressed
        if (buttonWasPressed) {
            // Button was released before debounce period
            DEBUG_PRINTLN("Button released early (debounce not met)");
            buttonWasPressed = false;
        }
    }
    
    lastButtonReading = reading;
    return false;
}

void Hardware::setRelay(bool state) {
    relayState = state;
    digitalWrite(RELAY_PIN, state ? RELAY_ON : RELAY_OFF);
    DEBUG_PRINTF("Relay set to: %s\n", state ? "ON (HIGH)" : "OFF (LOW)");
}

bool Hardware::getRelayState() {
    return relayState;
}

void Hardware::setCPUSpeed(uint32_t hz) {
    // Note: nRF52 CPU speed changes may require specific SDK calls
    // This is a placeholder - actual implementation depends on platform
    DEBUG_PRINTF("CPU speed requested: %lu Hz\n", hz);
    
    // On nRF52, you might use:
    // sd_power_mode_set(NRF_POWER_MODE_LOWPWR); // for low power
    // Or configure clock settings via Nordic SDK
    
    // For now, just log the request
    if (hz == CPU_SPEED_MEDIUM) {
        DEBUG_PRINTLN("CPU: Medium speed mode");
    } else if (hz == CPU_SPEED_LOW) {
        DEBUG_PRINTLN("CPU: Low power mode");
    }
}

void Hardware::enterDeepSleep() {
    DEBUG_PRINTLN("Entering deep sleep mode...");
    DEBUG_PRINTLN("Wake sources: Button press (AIN1), Meshtastic message");
    
    // Turn off display
    // Display::turnOff(); // Would be called from main
    
    // Configure wake sources
    // On nRF52, configure GPIO interrupt for button
    // nrf_gpio_cfg_sense_input(BUTTON_PIN, NRF_GPIO_PIN_PULLDOWN, NRF_GPIO_PIN_SENSE_HIGH);
    
    // Enter system OFF mode (lowest power)
    // sd_power_system_off();
    
    // Note: This is a placeholder. Actual implementation requires:
    // 1. Configure button as wake source
    // 2. Configure radio wake for Meshtastic
    // 3. Enter appropriate sleep mode
    
    DEBUG_PRINTLN("Deep sleep not fully implemented - staying awake");
}

uint16_t Hardware::getBatteryVoltage() {
    // RAK4631 has built-in battery voltage measurement
    // Typically on a specific ADC channel
    
    // Placeholder implementation
    // Real implementation would read from VBAT pin via ADC
    
    #ifdef WB_A0  // Some RAK boards have battery on A0
    int reading = analogRead(WB_A0);
    // Convert ADC reading to millivolts
    // Assuming 3.3V reference and voltage divider
    uint16_t mv = (reading * 3300 * 2) / 4096;  // *2 for voltage divider
    return mv;
    #else
    // Default return for testing
    return 3700;  // 3.7V typical for LiPo
    #endif
}

uint8_t Hardware::getBatteryPercent() {
    uint16_t voltage = getBatteryVoltage();
    
    // LiPo battery voltage to percentage (approximate)
    // 4.2V = 100%, 3.0V = 0%
    if (voltage >= 4200) return 100;
    if (voltage <= 3000) return 0;
    
    // Linear approximation
    return (uint8_t)((voltage - 3000) * 100 / 1200);
}
