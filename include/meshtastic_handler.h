#ifndef MESHTASTIC_HANDLER_H
#define MESHTASTIC_HANDLER_H

#include <Arduino.h>

/**
 * Meshtastic Message Handler
 * 
 * This module provides placeholder functions for Meshtastic integration.
 * In a complete implementation, these would interface with the Meshtastic firmware
 * or use the Meshtastic device API.
 * 
 * For actual Meshtastic integration, you would either:
 * 1. Build as part of Meshtastic firmware (recommended)
 * 2. Use Meshtastic Serial Protocol to communicate with a Meshtastic device
 * 3. Integrate Meshtastic protobufs and radio handling
 */

class MeshtasticHandler {
public:
    /**
     * Initialize Meshtastic communication
     */
    static void init();
    
    /**
     * Check for incoming messages
     * Returns true if a message was received
     */
    static bool checkMessages();
    
    /**
     * Send a text message to all nodes
     */
    static void sendMessage(const char* message);
    
    /**
     * Get the last received message
     */
    static const char* getLastMessage();
    
    /**
     * Get the sender's public key from last message
     */
    static const char* getLastSenderPublicKey();
    
    /**
     * Check if we have a new message waiting
     */
    static bool hasNewMessage();
    
    /**
     * Mark the current message as processed
     */
    static void markMessageProcessed();

private:
    static bool initialized;
    static bool newMessage;
    static String lastMessage;
    static String lastSenderKey;
};

#endif // MESHTASTIC_HANDLER_H
