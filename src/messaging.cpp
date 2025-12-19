/**
 * Messaging Module Implementation
 * Handles message sending, receiving, acknowledgments, and retries
 */

#include "messaging.h"
#include "whitelist.h"

// Static member initialization
PendingMessage Messaging::pendingMessages[MAX_WHITELIST_NODES];
MessageLogEntry Messaging::messageLog[MAX_LOG_ENTRIES];
uint16_t Messaging::logCount = 0;
uint16_t Messaging::logIndex = 0;
TransmitPower Messaging::txPower = DEFAULT_TX_POWER;

void Messaging::init() {
    // Initialize pending messages array
    for (int i = 0; i < MAX_WHITELIST_NODES; i++) {
        pendingMessages[i].active = false;
        pendingMessages[i].waitingForAck = false;
        pendingMessages[i].retryCount = 0;
    }
    
    // Initialize message log
    logCount = 0;
    logIndex = 0;
    
    DEBUG_PRINTLN("Messaging system initialized");
}

void Messaging::broadcastToWhitelist(const char* message) {
    DEBUG_PRINTF("Broadcasting to whitelist: %s\n", message);
    
    uint8_t nodeCount = Whitelist::getNodeCount();
    for (uint8_t i = 0; i < nodeCount; i++) {
        WhitelistNode* node = Whitelist::getNodeAt(i);
        if (node && node->active) {
            sendTo(node->publicKey, message);
        }
    }
}

void Messaging::sendTo(const char* publicKey, const char* message) {
    if (!publicKey || !message) {
        DEBUG_PRINTLN("Cannot send: invalid parameters");
        return;
    }
    
    DEBUG_PRINTF("Sending to %s: %s\n", Whitelist::getNodeName(publicKey), message);
    
    // Attempt to transmit
    bool success = transmitMessage(publicKey, message);
    
    if (success) {
        // Add to pending messages for ACK tracking (if not an ACK itself)
        if (strcmp(message, MSG_ACK) != 0) {
            int index = findPendingMessage(publicKey);
            if (index < 0) {
                // Find empty slot
                for (int i = 0; i < MAX_WHITELIST_NODES; i++) {
                    if (!pendingMessages[i].active) {
                        index = i;
                        break;
                    }
                }
            }
            
            if (index >= 0) {
                strncpy(pendingMessages[index].publicKey, publicKey, sizeof(pendingMessages[index].publicKey) - 1);
                strncpy(pendingMessages[index].message, message, sizeof(pendingMessages[index].message) - 1);
                pendingMessages[index].sentTime = millis();
                pendingMessages[index].retryCount = 0;
                pendingMessages[index].waitingForAck = true;
                pendingMessages[index].active = true;
            }
        }
        
        // Log outgoing message
        logMessage(Whitelist::getNodeName(publicKey), publicKey, message, true);
    } else {
        DEBUG_PRINTLN("Transmission failed");
    }
}

void Messaging::sendAck(const char* publicKey) {
    sendTo(publicKey, MSG_ACK);
}

void Messaging::processIncomingMessage(const char* senderKey, const char* message, uint16_t batteryLevel) {
    // Verify sender is whitelisted
    if (!Whitelist::isAuthorized(senderKey)) {
        DEBUG_PRINTLN("Message from unauthorized sender - ignored");
        return;
    }
    
    const char* senderName = Whitelist::getNodeName(senderKey);
    DEBUG_PRINTF("Processing message from %s: %s\n", senderName, message);
    
    // Update whitelist info
    if (batteryLevel > 0) {
        Whitelist::updateBatteryLevel(senderKey, batteryLevel);
    }
    Whitelist::updateLastSeen(senderKey);
    
    // Log incoming message
    logMessage(senderName, senderKey, message, false);
    
    // The actual emergency command handling is done in main.cpp
    // via the onMeshtasticMessage callback
}

void Messaging::checkRetries() {
    uint32_t currentTime = millis();
    
    for (int i = 0; i < MAX_WHITELIST_NODES; i++) {
        if (pendingMessages[i].active && pendingMessages[i].waitingForAck) {
            uint32_t elapsed = currentTime - pendingMessages[i].sentTime;
            
            // Check if retry timeout reached
            if (elapsed > MESSAGE_RETRY_TIMEOUT_MS) {
                if (pendingMessages[i].retryCount < MAX_MESSAGE_RETRIES) {
                    // Retry sending
                    DEBUG_PRINTF("Retrying message to %s (attempt %d/%d)\n",
                               Whitelist::getNodeName(pendingMessages[i].publicKey),
                               pendingMessages[i].retryCount + 1,
                               MAX_MESSAGE_RETRIES);
                    
                    transmitMessage(pendingMessages[i].publicKey, pendingMessages[i].message);
                    pendingMessages[i].sentTime = currentTime;
                    pendingMessages[i].retryCount++;
                    
                    // Log retry
                    char logMsg[128];
                    snprintf(logMsg, sizeof(logMsg), "RETRY: %s", pendingMessages[i].message);
                    logMessage(Whitelist::getNodeName(pendingMessages[i].publicKey),
                             pendingMessages[i].publicKey, logMsg, true);
                } else {
                    // Max retries reached
                    DEBUG_PRINTF("Max retries reached for %s - giving up\n",
                               Whitelist::getNodeName(pendingMessages[i].publicKey));
                    
                    // Log failure
                    char logMsg[128];
                    snprintf(logMsg, sizeof(logMsg), "FAILED: %s (no ACK after %d retries)",
                           pendingMessages[i].message, MAX_MESSAGE_RETRIES);
                    logMessage(Whitelist::getNodeName(pendingMessages[i].publicKey),
                             pendingMessages[i].publicKey, logMsg, true);
                    
                    pendingMessages[i].active = false;
                }
            }
        }
    }
}

void Messaging::handleAck(const char* senderKey) {
    int index = findPendingMessage(senderKey);
    if (index >= 0) {
        DEBUG_PRINTF("ACK received from %s\n", Whitelist::getNodeName(senderKey));
        pendingMessages[index].waitingForAck = false;
        pendingMessages[index].active = false;
    }
}

void Messaging::logMessage(const char* nodeName, const char* publicKey, 
                          const char* message, bool isOutgoing) {
    // Add to circular log buffer
    MessageLogEntry* entry = &messageLog[logIndex];
    
    strncpy(entry->nodeName, nodeName, NODE_NAME_MAX_LENGTH);
    entry->nodeName[NODE_NAME_MAX_LENGTH] = '\0';
    
    strncpy(entry->publicKey, publicKey, sizeof(entry->publicKey) - 1);
    entry->publicKey[sizeof(entry->publicKey) - 1] = '\0';
    
    strncpy(entry->message, message, sizeof(entry->message) - 1);
    entry->message[sizeof(entry->message) - 1] = '\0';
    
    entry->timestamp = millis();
    entry->isOutgoing = isOutgoing;
    
    logIndex = (logIndex + 1) % MAX_LOG_ENTRIES;
    if (logCount < MAX_LOG_ENTRIES) {
        logCount++;
    }
    
    DEBUG_PRINTF("Message logged: [%s] %s -> %s\n",
               isOutgoing ? "OUT" : "IN", nodeName, message);
}

uint16_t Messaging::getLogCount() {
    return logCount;
}

MessageLogEntry* Messaging::getLogEntry(uint16_t index) {
    if (index >= logCount) {
        return nullptr;
    }
    
    // Calculate actual index in circular buffer
    uint16_t actualIndex;
    if (logCount < MAX_LOG_ENTRIES) {
        actualIndex = index;
    } else {
        actualIndex = (logIndex + index) % MAX_LOG_ENTRIES;
    }
    
    return &messageLog[actualIndex];
}

void Messaging::clearLog() {
    logCount = 0;
    logIndex = 0;
    DEBUG_PRINTLN("Message log cleared");
}

TransmitPower Messaging::getTxPower() {
    return txPower;
}

void Messaging::setTxPower(TransmitPower power) {
    txPower = power;
    DEBUG_PRINTF("TX power set to: %d\n", (int)power);
    // Actual power setting would happen in Meshtastic radio config
}

int Messaging::findPendingMessage(const char* publicKey) {
    for (int i = 0; i < MAX_WHITELIST_NODES; i++) {
        if (pendingMessages[i].active && 
            strcmp(pendingMessages[i].publicKey, publicKey) == 0) {
            return i;
        }
    }
    return -1;
}

bool Messaging::transmitMessage(const char* publicKey, const char* message) {
    // PLACEHOLDER: Actual Meshtastic transmission
    // This needs to be implemented with Meshtastic firmware integration
    
    DEBUG_PRINTLN("----------------------------------------");
    DEBUG_PRINTLN("MESHTASTIC TRANSMISSION (PLACEHOLDER)");
    DEBUG_PRINTF("To: %s\n", publicKey);
    DEBUG_PRINTF("Message: %s\n", message);
    DEBUG_PRINTF("TX Power: %d\n", (int)txPower);
    DEBUG_PRINTLN("----------------------------------------");
    
    // In real implementation, this would:
    // 1. Create a Meshtastic packet
    // 2. Set destination to publicKey
    // 3. Set payload to message
    // 4. Configure TX power
    // 5. Transmit via LoRa radio
    // 6. Return success/failure
    
    // For now, always return success for testing
    return true;
}
