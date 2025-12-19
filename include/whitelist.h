#ifndef WHITELIST_H
#define WHITELIST_H

#include <Arduino.h>
#include "config.h"

/**
 * Whitelist Management Module
 * Handles authorized node list and node names
 */

struct WhitelistNode {
    char publicKey[64];        // Base64 encoded public key
    char name[NODE_NAME_MAX_LENGTH + 1];  // Node name (20 chars + null)
    bool active;               // Is this slot active?
    uint16_t batteryLevel;     // Last known battery level (0-100%)
    uint32_t lastSeen;         // Timestamp of last message
};

class Whitelist {
public:
    /**
     * Initialize whitelist from preferences
     */
    static void init();
    
    /**
     * Check if a public key is in the whitelist
     * @param publicKey Base64 encoded public key
     * @return true if authorized
     */
    static bool isAuthorized(const char* publicKey);
    
    /**
     * Add a node to the whitelist
     * @param publicKey Base64 encoded public key
     * @param name Optional friendly name (uses key if null)
     * @return true if added successfully
     */
    static bool addNode(const char* publicKey, const char* name = nullptr);
    
    /**
     * Remove a node from the whitelist
     * @param publicKey Base64 encoded public key
     * @return true if removed successfully
     */
    static bool removeNode(const char* publicKey);
    
    /**
     * Get node name by public key
     * @param publicKey Base64 encoded public key
     * @return Node name or null if not found
     */
    static const char* getNodeName(const char* publicKey);
    
    /**
     * Set node name
     * @param publicKey Base64 encoded public key
     * @param name New name (max 20 chars)
     * @return true if updated successfully
     */
    static bool setNodeName(const char* publicKey, const char* name);
    
    /**
     * Update node battery level
     * @param publicKey Base64 encoded public key
     * @param batteryLevel Battery level 0-100%
     */
    static void updateBatteryLevel(const char* publicKey, uint16_t batteryLevel);
    
    /**
     * Update last seen timestamp
     * @param publicKey Base64 encoded public key
     */
    static void updateLastSeen(const char* publicKey);
    
    /**
     * Get number of nodes in whitelist
     */
    static uint8_t getNodeCount();
    
    /**
     * Get node at index
     * @param index Node index (0 to count-1)
     * @return Pointer to node or null if invalid index
     */
    static WhitelistNode* getNodeAt(uint8_t index);
    
    /**
     * Save whitelist to preferences
     */
    static void save();
    
    /**
     * Load whitelist from preferences
     */
    static void load();
    
    /**
     * Clear all nodes
     */
    static void clear();

private:
    static WhitelistNode nodes[MAX_WHITELIST_NODES];
    static uint8_t nodeCount;
    
    /**
     * Find node index by public key
     * @return Index or -1 if not found
     */
    static int findNodeIndex(const char* publicKey);
};

#endif // WHITELIST_H
