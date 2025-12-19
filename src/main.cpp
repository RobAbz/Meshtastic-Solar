/**
 * Meshtastic-Solar Emergency Shutdown System
 * © SilverTime 2025
 * 
 * Main application file
 * 
 * This system provides emergency shutdown control via:
 * - Physical button press (AIN1)
 * - Meshtastic messages from whitelisted nodes
 * - Web configuration interface
 * 
 * Hardware: RAK4631 on RAK19007 base board with RAK13007 relay module
 */

#include <Arduino.h>
#include "config.h"
#include "hardware.h"
#include "display.h"
#include "whitelist.h"
#include "messaging.h"
#include "timekeeper.h"
#include "webserver.h"

// System state
bool emergencyShutdownActive = false;
uint32_t lastLoopTime = 0;

/**
 * Arduino setup function
 */
void setup() {
    // Initialize serial for debugging
    #if DEBUG_SERIAL_ENABLED
    Serial.begin(DEBUG_SERIAL_BAUD);
    delay(1000);  // Wait for serial to stabilize
    DEBUG_PRINTLN("===========================================");
    DEBUG_PRINTLN("Meshtastic-Solar Emergency Shutdown System");
    DEBUG_PRINTLN("© SilverTime 2025");
    DEBUG_PRINTF("Compiled: %s %s\n", COMPILE_DATE, COMPILE_TIME);
    DEBUG_PRINTLN("===========================================");
    #endif
    
    // Initialize hardware
    DEBUG_PRINTLN("Initializing hardware...");
    Hardware::init();
    DEBUG_PRINTLN("Hardware initialized");
    
    // Initialize display
    DEBUG_PRINTLN("Initializing display...");
    Display::init();
    DEBUG_PRINTLN("Display initialized");
    
    // Show boot splash screen
    DEBUG_PRINTLN("Showing boot splash...");
    Display::showBootSplash();
    
    // Initialize time keeper
    DEBUG_PRINTLN("Initializing time keeper...");
    TimeKeeper::init();
    TimeKeeper::load();
    DEBUG_PRINTLN("Time keeper initialized");
    
    // Initialize whitelist
    DEBUG_PRINTLN("Initializing whitelist...");
    Whitelist::init();
    DEBUG_PRINTLN("Whitelist initialized");
    
    // Initialize messaging
    DEBUG_PRINTLN("Initializing messaging...");
    Messaging::init();
    DEBUG_PRINTLN("Messaging initialized");
    
    // Initialize web server
    DEBUG_PRINTLN("Initializing web server...");
    WebServer::init();
    DEBUG_PRINTLN("Web server initialized");
    
    // Wait for boot splash to complete
    uint32_t bootStart = millis();
    while (millis() - bootStart < BOOT_SPLASH_DURATION_MS) {
        delay(100);
    }
    
    // Clear display and show initial status
    DEBUG_PRINTLN("Boot complete, entering normal operation");
    emergencyShutdownActive = Hardware::getRelayState();
    
    char timeStr[20];
    TimeKeeper::getDateTimeString(timeStr);
    
    if (emergencyShutdownActive) {
        Display::updateStatus(true, DISPLAY_MSG_EMERGENCY_INITIATED, timeStr);
    } else {
        Display::updateStatus(false, "System Ready", timeStr);
        // Display will auto-off after timeout
    }
    
    DEBUG_PRINTLN("Setup complete - System ready");
    lastLoopTime = millis();
}

/**
 * Arduino main loop
 */
void loop() {
    uint32_t currentTime = millis();
    
    // Update time keeper
    TimeKeeper::update();
    
    // Check for button press
    if (Hardware::checkButton()) {
        DEBUG_PRINTLN("BUTTON PRESSED!");
        handleEmergencyActivation(true, nullptr);
    }
    
    // Check for pending message retries
    Messaging::checkRetries();
    
    // Handle web server requests
    WebServer::handle();
    
    // Check display auto-off
    Display::checkAutoOff();
    
    // Periodic status update (every 10 seconds)
    if (currentTime - lastLoopTime > 10000) {
        lastLoopTime = currentTime;
        DEBUG_PRINTF("Status: Relay=%s, Display=%s, Battery=%dmV\n",
                    Hardware::getRelayState() ? "ON" : "OFF",
                    Display::isOn() ? "ON" : "OFF",
                    Hardware::getBatteryVoltage());
    }
    
    // Small delay to prevent CPU hogging
    delay(LOOP_DELAY_MS);
    
    // Enter deep sleep if relay is off and no activity
    if (!Hardware::getRelayState() && !Display::isOn()) {
        // Could enter deep sleep here if configured
        // Hardware::enterDeepSleep();
    }
}

/**
 * Handle emergency activation
 * @param fromButton true if triggered by button, false if from message
 * @param senderKey public key of sender (if from message), null if from button
 */
void handleEmergencyActivation(bool fromButton, const char* senderKey) {
    DEBUG_PRINTLN("=== Emergency Activation ===");
    DEBUG_PRINTF("Source: %s\n", fromButton ? "Button" : "Meshtastic Message");
    
    if (!fromButton && senderKey) {
        DEBUG_PRINTF("Sender: %s\n", senderKey);
        
        // Verify sender is whitelisted
        if (!Whitelist::isAuthorized(senderKey)) {
            DEBUG_PRINTLN("REJECTED: Sender not in whitelist");
            return;
        }
        DEBUG_PRINTLN("Sender authorized");
    }
    
    // Activate relay
    Hardware::setRelay(true);
    emergencyShutdownActive = true;
    
    // Get timestamp
    char timeStr[20];
    TimeKeeper::getDateTimeString(timeStr);
    
    // Update display
    Display::turnOn();
    Display::updateStatus(true, DISPLAY_MSG_EMERGENCY_INITIATED, timeStr);
    
    // Broadcast to all whitelisted nodes
    DEBUG_PRINTLN("Broadcasting emergency state to whitelist...");
    Messaging::broadcastToWhitelist(MSG_EMERGENCY_ON);
    
    // Send acknowledgment if from message
    if (!fromButton && senderKey) {
        DEBUG_PRINTLN("Sending acknowledgment...");
        Messaging::sendAck(senderKey);
    }
    
    // Log the event
    Messaging::logMessage("System", "LOCAL", 
                         fromButton ? "Button activated" : "Message activated", 
                         true);
    
    DEBUG_PRINTLN("Emergency activation complete");
}

/**
 * Handle emergency deactivation
 * @param fromMessage true if triggered by message
 * @param senderKey public key of sender (if from message), null otherwise
 */
void handleEmergencyDeactivation(bool fromMessage, const char* senderKey) {
    DEBUG_PRINTLN("=== Emergency Deactivation ===");
    DEBUG_PRINTF("Source: %s\n", fromMessage ? "Meshtastic Message" : "Reset/Web");
    
    if (fromMessage && senderKey) {
        DEBUG_PRINTF("Sender: %s\n", senderKey);
        
        // Verify sender is whitelisted
        if (!Whitelist::isAuthorized(senderKey)) {
            DEBUG_PRINTLN("REJECTED: Sender not in whitelist");
            return;
        }
        DEBUG_PRINTLN("Sender authorized");
    }
    
    // Deactivate relay
    Hardware::setRelay(false);
    emergencyShutdownActive = false;
    
    // Get timestamp
    char timeStr[20];
    TimeKeeper::getDateTimeString(timeStr);
    
    // Update display
    Display::turnOn();
    Display::updateStatus(false, DISPLAY_MSG_EMERGENCY_CANCELLED, timeStr);
    
    // Broadcast to all whitelisted nodes
    DEBUG_PRINTLN("Broadcasting emergency state to whitelist...");
    Messaging::broadcastToWhitelist(MSG_EMERGENCY_OFF);
    
    // Send acknowledgment if from message
    if (fromMessage && senderKey) {
        DEBUG_PRINTLN("Sending acknowledgment...");
        Messaging::sendAck(senderKey);
    }
    
    // Log the event
    Messaging::logMessage("System", "LOCAL", 
                         fromMessage ? "Message deactivated" : "Manual deactivated", 
                         true);
    
    DEBUG_PRINTLN("Emergency deactivation complete");
}

/**
 * Callback for incoming Meshtastic messages
 * This function should be called by the Meshtastic integration layer
 * @param senderKey Sender's public key
 * @param message Message content
 * @param batteryLevel Sender's battery level (0-100)
 */
void onMeshtasticMessage(const char* senderKey, const char* message, uint16_t batteryLevel) {
    DEBUG_PRINTLN("=== Incoming Meshtastic Message ===");
    DEBUG_PRINTF("From: %s\n", Whitelist::getNodeName(senderKey));
    DEBUG_PRINTF("Message: %s\n", message);
    DEBUG_PRINTF("Battery: %d%%\n", batteryLevel);
    
    // Update whitelist with battery info
    Whitelist::updateBatteryLevel(senderKey, batteryLevel);
    Whitelist::updateLastSeen(senderKey);
    
    // Process the message
    Messaging::processIncomingMessage(senderKey, message, batteryLevel);
    
    // Handle emergency commands
    if (strcmp(message, MSG_EMERGENCY_ON) == 0) {
        handleEmergencyActivation(false, senderKey);
    } else if (strcmp(message, MSG_EMERGENCY_OFF) == 0) {
        handleEmergencyDeactivation(true, senderKey);
    } else if (strcmp(message, MSG_ACK) == 0) {
        Messaging::handleAck(senderKey);
    } else {
        DEBUG_PRINTLN("Unknown message type");
    }
}
