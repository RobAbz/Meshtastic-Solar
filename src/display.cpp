/**
 * Display Module Implementation
 * Handles OLED display output and power management
 */

#include "display.h"

// Static member initialization
U8G2_SSD1306_128X64_NONAME_F_HW_I2C Display::u8g2(U8G2_R0, U8X8_PIN_NONE);
bool Display::displayOn = false;
uint32_t Display::lastActivityTime = 0;
bool Display::relayWasOn = false;

void Display::init() {
    // Initialize I2C for display
    Wire.begin();
    
    // Initialize U8g2
    u8g2.begin();
    u8g2.setFont(DISPLAY_FONT);
    
    displayOn = true;
    lastActivityTime = millis();
    
    DEBUG_PRINTLN("Display initialized");
}

void Display::showBootSplash() {
    turnOn();
    u8g2.clearBuffer();
    
    // Draw Meshtastic logo (simplified)
    drawLogo();
    
    // Draw title
    drawCentered(30, DISPLAY_MSG_BOOT_TITLE);
    
    // Draw copyright
    drawCentered(42, DISPLAY_MSG_COPYRIGHT);
    
    // Draw compile date/time
    char compileInfo[32];
    snprintf(compileInfo, sizeof(compileInfo), "%s %s", COMPILE_DATE, COMPILE_TIME);
    drawCentered(54, compileInfo);
    
    u8g2.sendBuffer();
    
    DEBUG_PRINTLN("Boot splash displayed");
}

void Display::updateStatus(bool relayState, const char* message, const char* timestamp) {
    if (!displayOn) {
        turnOn();
    }
    
    lastActivityTime = millis();
    relayWasOn = relayState;
    
    u8g2.clearBuffer();
    
    // Draw header with relay status
    u8g2.setFont(u8g2_font_6x10_tr);  // Smaller font for header
    if (relayState) {
        u8g2.drawStr(0, 8, "RELAY: ON");
    } else {
        u8g2.drawStr(0, 8, "RELAY: OFF");
    }
    
    // Draw separator line
    u8g2.drawLine(0, 10, 128, 10);
    
    // Draw main message
    u8g2.setFont(DISPLAY_FONT);
    
    // Word wrap message if needed
    uint8_t y = 22;
    const char* line = message;
    char buffer[22];  // Max ~20 chars per line
    
    while (*line && y < 64) {
        // Find line break or max length
        int len = 0;
        const char* nextSpace = line;
        
        while (*nextSpace && *nextSpace != '\n' && len < 20) {
            if (*nextSpace == ' ') nextSpace++;
            len++;
            nextSpace++;
        }
        
        strncpy(buffer, line, len);
        buffer[len] = '\0';
        
        u8g2.drawStr(2, y, buffer);
        y += (FONT_HEIGHT + LINE_SPACING);
        
        line += len;
        if (*line == '\n') line++;
    }
    
    // Draw timestamp if provided
    if (timestamp) {
        u8g2.setFont(u8g2_font_5x7_tr);  // Small font for timestamp
        u8g2.drawStr(2, 62, timestamp);
    }
    
    u8g2.sendBuffer();
    
    DEBUG_PRINTF("Display updated: Relay=%s, Msg=%s\n", 
                relayState ? "ON" : "OFF", message);
}

void Display::turnOn() {
    if (!displayOn) {
        u8g2.setPowerSave(0);  // Wake up display
        displayOn = true;
        DEBUG_PRINTLN("Display turned ON");
    }
    lastActivityTime = millis();
}

void Display::turnOff() {
    if (displayOn) {
        u8g2.setPowerSave(1);  // Sleep display
        displayOn = false;
        DEBUG_PRINTLN("Display turned OFF (power save)");
    }
}

void Display::checkAutoOff() {
    // Only auto-off if relay is OFF and timeout has passed
    if (!relayWasOn && displayOn) {
        if (millis() - lastActivityTime > DISPLAY_TIMEOUT_MS) {
            turnOff();
        }
    }
}

bool Display::isOn() {
    return displayOn;
}

void Display::clear() {
    u8g2.clearBuffer();
    u8g2.sendBuffer();
}

void Display::drawText(uint8_t x, uint8_t y, const char* text) {
    u8g2.setFont(DISPLAY_FONT);
    u8g2.drawStr(x, y, text);
}

void Display::update() {
    u8g2.sendBuffer();
}

void Display::drawLogo() {
    // Simple Meshtastic-inspired logo
    // Draw a mesh pattern (simplified)
    
    // Draw connecting nodes
    u8g2.drawCircle(64, 12, 3);  // Center top
    u8g2.drawCircle(50, 12, 2);  // Left
    u8g2.drawCircle(78, 12, 2);  // Right
    
    // Draw connection lines
    u8g2.drawLine(54, 12, 60, 12);  // Left to center
    u8g2.drawLine(68, 12, 74, 12);  // Center to right
    
    DEBUG_PRINTLN("Logo drawn");
}

void Display::drawCentered(uint8_t y, const char* text) {
    // Calculate text width and center it
    uint8_t textWidth = u8g2.getStrWidth(text);
    uint8_t x = (SCREEN_WIDTH - textWidth) / 2;
    u8g2.drawStr(x, y, text);
}
