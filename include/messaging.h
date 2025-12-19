#ifndef MESSAGING_H
#define MESSAGING_H

#include <Arduino.h>
#include "config.h"

/**
 * Meshtastic Messaging Module
 * Handles message sending, receiving, acknowledgments, and retries
 */

struct PendingMessage {
    char publicKey[64];        // Recipient public key
    char message[128];         // Message content
    uint32_t sentTime;         // When message was sent
    uint8_t retryCount;        // Number of retries
    bool waitingForAck;        // Waiting for acknowledgment
    bool active;               // Is this slot active
};

struct MessageLogEntry {
    char nodeName[NODE_NAME_MAX_LENGTH + 1];  // Sender name
    char publicKey[64];        // Sender public key
    char message[128];         // Message content
    uint32_t timestamp;        // When received
    bool isOutgoing;           // true = sent, false = received
};

class Messaging {
public:
    /**
     * Initialize messaging system
     */
    static void init();
    
    /**
     * Send message to all whitelisted nodes
     * @param message Message text
     */
    static void broadcastToWhitelist(const char* message);
    
    /**
     * Send message to specific node
     * @param publicKey Recipient public key
     * @param message Message text
     */
    static void sendTo(const char* publicKey, const char* message);
    
    /**
     * Send acknowledgment
     * @param publicKey Recipient public key
     */
    static void sendAck(const char* publicKey);
    
    /**
     * Process incoming message
     * @param senderKey Sender's public key
     * @param message Message content
     * @param batteryLevel Sender's battery level (optional)
     */
    static void processIncomingMessage(const char* senderKey, const char* message, uint16_t batteryLevel = 0);
    
    /**
     * Check for pending messages that need retry
     * Called periodically from main loop
     */
    static void checkRetries();
    
    /**
     * Mark message as acknowledged
     * @param senderKey Who sent the ack
     */
    static void handleAck(const char* senderKey);
    
    /**
     * Add entry to message log
     * @param nodeName Sender/recipient name
     * @param publicKey Sender/recipient key
     * @param message Message content
     * @param isOutgoing true if sent by us
     */
    static void logMessage(const char* nodeName, const char* publicKey, const char* message, bool isOutgoing);
    
    /**
     * Get message log entry count
     */
    static uint16_t getLogCount();
    
    /**
     * Get log entry at index
     * @param index Entry index (0 to count-1)
     * @return Pointer to entry or null if invalid
     */
    static MessageLogEntry* getLogEntry(uint16_t index);
    
    /**
     * Clear message log
     */
    static void clearLog();
    
    /**
     * Get transmit power setting
     */
    static TransmitPower getTxPower();
    
    /**
     * Set transmit power
     * @param power New power level
     */
    static void setTxPower(TransmitPower power);

private:
    static PendingMessage pendingMessages[MAX_WHITELIST_NODES];
    static MessageLogEntry messageLog[MAX_LOG_ENTRIES];
    static uint16_t logCount;
    static uint16_t logIndex;
    static TransmitPower txPower;
    
    /**
     * Find pending message by public key
     * @return Index or -1 if not found
     */
    static int findPendingMessage(const char* publicKey);
    
    /**
     * Actually send the message via Meshtastic
     * This is a placeholder - needs Meshtastic firmware integration
     */
    static bool transmitMessage(const char* publicKey, const char* message);
};

#endif // MESSAGING_H
