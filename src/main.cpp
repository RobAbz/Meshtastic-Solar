/**
 * Meshtastic-Solar Emergency Signal System
 * 
 * This program uses Meshtastic to send emergency signals between nodes.
 * Features:
 * - Button input triggers emergency signal
 * - Relay control via IO4
 * - RAK1921 OLED display for message visualization
 * - Whitelist-based node authentication
 * - Acknowledgment messages
 */

#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include "config.h"
#include "meshtastic_handler.h"

// Initialize U8g2 for RAK1921 OLED (SSD1306, 128x64, I2C)
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// Message buffer for display
struct DisplayMessage {
    String text;
    unsigned long timestamp;
    bool active;
};

DisplayMessage messageBuffer[MAX_DISPLAY_LINES];
int messageBufferIndex = 0;

// Button state tracking
bool lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
bool buttonPressed = false;

// Relay state
bool relayState = false;

// Forward declarations
void initHardware();
void initDisplay();
void handleButtonPress();
void checkButton();
void setRelay(bool state);
void sendEmergencySignal(const char* message);
void sendAcknowledgment();
void updateDisplay();
void addMessageToDisplay(const String& message);
bool isNodeWhitelisted(const char* publicKey);
void processIncomingMessage(const char* message, const char* sender);

/**
 * Initialize hardware components
 */
void initHardware() {
    // Initialize serial for debugging
    Serial.begin(115200);
    delay(1000);
    Serial.println("Meshtastic-Solar Emergency System starting...");
    
    // Configure relay pin as output
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, LOW);
    Serial.println("Relay pin configured (IO4)");
    
    // Configure button pin with internal pull-up
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    Serial.println("Button pin configured (IO6)");
    
    // Initialize I2C for OLED
    Wire.begin();
    Serial.println("I2C initialized");
}

/**
 * Initialize OLED display
 */
void initDisplay() {
    u8g2.begin();
    u8g2.setFont(u8g2_font_t0_11_mf); // 8px high font as specified
    u8g2.clearBuffer();
    
    // Display startup message
    u8g2.drawStr(0, 10, "Meshtastic-Solar");
    u8g2.drawStr(0, 22, "Emergency System");
    u8g2.drawStr(0, 34, "Initializing...");
    u8g2.sendBuffer();
    
    Serial.println("Display initialized");
    
    // Initialize message buffer
    for (int i = 0; i < MAX_DISPLAY_LINES; i++) {
        messageBuffer[i].active = false;
        messageBuffer[i].text = "";
        messageBuffer[i].timestamp = 0;
    }
    
    delay(2000); // Show startup message for 2 seconds
}

/**
 * Check button state with debouncing
 */
void checkButton() {
    int reading = digitalRead(BUTTON_PIN);
    
    // Check if button state has changed
    if (reading != lastButtonState) {
        lastDebounceTime = millis();
    }
    
    // If enough time has passed since last change
    if ((millis() - lastDebounceTime) > BUTTON_DEBOUNCE_MS) {
        // If button is pressed (and wasn't pressed before)
        if (reading == BUTTON_PRESSED && !buttonPressed) {
            buttonPressed = true;
            handleButtonPress();
        } else if (reading != BUTTON_PRESSED) {
            buttonPressed = false;
        }
    }
    
    lastButtonState = reading;
}

/**
 * Handle button press event
 */
void handleButtonPress() {
    Serial.println("Emergency button pressed!");
    
    // Activate relay (button always triggers ON, OFF comes via message)
    setRelay(true);
    
    // Send emergency ON signal
    sendEmergencySignal(EMERGENCY_ON_MSG);
    
    // Add message to display
    addMessageToDisplay("EMERGENCY ACTIVATED");
    
    // Send acknowledgment
    sendAcknowledgment();
}

/**
 * Set relay state
 */
void setRelay(bool state) {
    relayState = state;
    digitalWrite(RELAY_PIN, state ? HIGH : LOW);
    
    Serial.print("Relay set to: ");
    Serial.println(state ? "ON" : "OFF");
    
    // Update display status
    String statusMsg = state ? "Relay: ON" : "Relay: OFF";
    addMessageToDisplay(statusMsg);
}

/**
 * Send emergency signal via Meshtastic
 */
void sendEmergencySignal(const char* message) {
    Serial.print("Sending message: ");
    Serial.println(message);
    
    // Use the Meshtastic handler to send the message
    MeshtasticHandler::sendMessage(message);
    
    // Add to display
    addMessageToDisplay(String("SENT: ") + message);
}

/**
 * Send acknowledgment message
 */
void sendAcknowledgment() {
    Serial.println("Sending acknowledgment");
    sendEmergencySignal(ACK_MSG);
}

/**
 * Check if a node's public key is in the whitelist
 */
bool isNodeWhitelisted(const char* publicKey) {
    for (int i = 0; i < WHITELIST_SIZE; i++) {
        if (strcmp(publicKey, WHITELIST[i]) == 0) {
            return true;
        }
    }
    return false;
}

/**
 * Process incoming Meshtastic message
 * This function should be called from the Meshtastic message handler
 */
void processIncomingMessage(const char* message, const char* senderPublicKey) {
    // Check if sender is whitelisted
    if (!isNodeWhitelisted(senderPublicKey)) {
        Serial.println("Message from non-whitelisted node ignored");
        return;
    }
    
    Serial.print("Processing message from whitelisted node: ");
    Serial.println(message);
    
    // Handle emergency ON signal
    if (strcmp(message, EMERGENCY_ON_MSG) == 0) {
        setRelay(true);
        addMessageToDisplay("RX: EMERGENCY ON");
        sendAcknowledgment();
    }
    // Handle emergency OFF signal
    else if (strcmp(message, EMERGENCY_OFF_MSG) == 0) {
        setRelay(false);
        addMessageToDisplay("RX: EMERGENCY OFF");
        sendAcknowledgment();
    }
    // Handle acknowledgment
    else if (strcmp(message, ACK_MSG) == 0) {
        addMessageToDisplay("RX: ACK");
    }
    // Display any other message from whitelisted nodes
    else {
        addMessageToDisplay(String("RX: ") + message);
    }
}

/**
 * Add a message to the display buffer
 */
void addMessageToDisplay(const String& message) {
    // Remove oldest messages if buffer is full
    messageBuffer[messageBufferIndex].text = message;
    messageBuffer[messageBufferIndex].timestamp = millis();
    messageBuffer[messageBufferIndex].active = true;
    
    messageBufferIndex = (messageBufferIndex + 1) % MAX_DISPLAY_LINES;
    
    Serial.print("Display message added: ");
    Serial.println(message);
}

/**
 * Update OLED display with current messages
 */
void updateDisplay() {
    static unsigned long lastUpdate = 0;
    unsigned long currentTime = millis();
    
    // Update display at specified interval
    if (currentTime - lastUpdate < DISPLAY_UPDATE_MS) {
        return;
    }
    lastUpdate = currentTime;
    
    u8g2.clearBuffer();
    
    // Draw header
    u8g2.setFont(u8g2_font_t0_11_mf);
    u8g2.drawStr(0, 9, "Meshtastic Solar");
    
    // Draw relay status
    String status = relayState ? "RELAY:ON" : "RELAY:OFF";
    u8g2.drawStr(95, 9, status.c_str());
    
    // Draw separator line
    u8g2.drawLine(0, 11, 128, 11);
    
    // Draw messages (newest at bottom)
    int yPos = HEADER_HEIGHT + 3;
    int displayedMessages = 0;
    
    // Remove expired messages (using time difference to handle overflow)
    for (int i = 0; i < MAX_DISPLAY_LINES; i++) {
        if (messageBuffer[i].active) {
            // Use subtraction for safe overflow handling
            unsigned long timeDiff = currentTime - messageBuffer[i].timestamp;
            if (timeDiff > MESSAGE_DISPLAY_TIME_MS) {
                messageBuffer[i].active = false;
            }
        }
    }
    
    // Display active messages
    for (int i = 0; i < MAX_DISPLAY_LINES; i++) {
        int idx = (messageBufferIndex + i) % MAX_DISPLAY_LINES;
        if (messageBuffer[idx].active && displayedMessages < MAX_DISPLAY_LINES) {
            yPos += LINE_HEIGHT;
            
            // Truncate message if too long
            String displayText = messageBuffer[idx].text;
            if (displayText.length() > MAX_DISPLAY_CHARS) {
                displayText = displayText.substring(0, TRUNCATE_CHARS) + "...";
            }
            
            u8g2.drawStr(2, yPos, displayText.c_str());
            displayedMessages++;
        }
    }
    
    u8g2.sendBuffer();
}

/**
 * Arduino setup function
 */
void setup() {
    // Initialize hardware
    initHardware();
    
    // Initialize display
    initDisplay();
    
    // Initialize Meshtastic handler
    MeshtasticHandler::init();
    
    Serial.println("Setup complete - System ready");
    addMessageToDisplay("System Ready");
}

/**
 * Arduino main loop
 */
void loop() {
    // Check button state
    checkButton();
    
    // Check for incoming Meshtastic messages
    if (MeshtasticHandler::hasNewMessage()) {
        const char* message = MeshtasticHandler::getLastMessage();
        const char* senderKey = MeshtasticHandler::getLastSenderPublicKey();
        
        processIncomingMessage(message, senderKey);
        MeshtasticHandler::markMessageProcessed();
    }
    
    // Update display
    updateDisplay();
    
    // Small delay to prevent CPU hogging
    delay(10);
}
