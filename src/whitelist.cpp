/**
 * Whitelist Module Implementation
 * Handles authorized node list and node names
 */

#include "whitelist.h"
#include <Preferences.h>

// Static member initialization
WhitelistNode Whitelist::nodes[MAX_WHITELIST_NODES];
uint8_t Whitelist::nodeCount = 0;

// Preferences object for persistent storage
static Preferences prefs;

void Whitelist::init() {
    // Clear node array
    for (int i = 0; i < MAX_WHITELIST_NODES; i++) {
        nodes[i].active = false;
        nodes[i].publicKey[0] = '\0';
        nodes[i].name[0] = '\0';
        nodes[i].batteryLevel = 0;
        nodes[i].lastSeen = 0;
    }
    nodeCount = 0;
    
    // Load from preferences
    load();
    
    // If whitelist is empty, add default node
    if (nodeCount == 0) {
        DEBUG_PRINTLN("Whitelist empty, adding default node");
        addNode(DEFAULT_WHITELIST_KEY, "Default Node");
    }
    
    DEBUG_PRINTF("Whitelist initialized with %d node(s)\n", nodeCount);
}

bool Whitelist::isAuthorized(const char* publicKey) {
    if (!publicKey || publicKey[0] == '\0') {
        return false;
    }
    
    for (int i = 0; i < MAX_WHITELIST_NODES; i++) {
        if (nodes[i].active && strcmp(nodes[i].publicKey, publicKey) == 0) {
            return true;
        }
    }
    
    return false;
}

bool Whitelist::addNode(const char* publicKey, const char* name) {
    if (!publicKey || publicKey[0] == '\0') {
        DEBUG_PRINTLN("Cannot add node: invalid public key");
        return false;
    }
    
    // Check if already exists
    if (isAuthorized(publicKey)) {
        DEBUG_PRINTLN("Node already in whitelist");
        return false;
    }
    
    // Find empty slot
    for (int i = 0; i < MAX_WHITELIST_NODES; i++) {
        if (!nodes[i].active) {
            // Add node
            strncpy(nodes[i].publicKey, publicKey, sizeof(nodes[i].publicKey) - 1);
            nodes[i].publicKey[sizeof(nodes[i].publicKey) - 1] = '\0';
            
            if (name && name[0] != '\0') {
                strncpy(nodes[i].name, name, NODE_NAME_MAX_LENGTH);
                nodes[i].name[NODE_NAME_MAX_LENGTH] = '\0';
            } else {
                // Use truncated key as name
                strncpy(nodes[i].name, publicKey, NODE_NAME_MAX_LENGTH);
                nodes[i].name[NODE_NAME_MAX_LENGTH] = '\0';
            }
            
            nodes[i].active = true;
            nodes[i].batteryLevel = 0;
            nodes[i].lastSeen = millis();
            nodeCount++;
            
            DEBUG_PRINTF("Node added: %s (%s)\n", nodes[i].name, publicKey);
            save();
            return true;
        }
    }
    
    DEBUG_PRINTLN("Whitelist full, cannot add node");
    return false;
}

bool Whitelist::removeNode(const char* publicKey) {
    int index = findNodeIndex(publicKey);
    if (index >= 0) {
        nodes[index].active = false;
        nodeCount--;
        DEBUG_PRINTF("Node removed: %s\n", publicKey);
        save();
        return true;
    }
    return false;
}

const char* Whitelist::getNodeName(const char* publicKey) {
    int index = findNodeIndex(publicKey);
    if (index >= 0) {
        return nodes[index].name;
    }
    return "Unknown";
}

bool Whitelist::setNodeName(const char* publicKey, const char* name) {
    int index = findNodeIndex(publicKey);
    if (index >= 0 && name && name[0] != '\0') {
        strncpy(nodes[index].name, name, NODE_NAME_MAX_LENGTH);
        nodes[index].name[NODE_NAME_MAX_LENGTH] = '\0';
        DEBUG_PRINTF("Node renamed: %s -> %s\n", publicKey, name);
        save();
        return true;
    }
    return false;
}

void Whitelist::updateBatteryLevel(const char* publicKey, uint16_t batteryLevel) {
    int index = findNodeIndex(publicKey);
    if (index >= 0) {
        nodes[index].batteryLevel = batteryLevel;
    }
}

void Whitelist::updateLastSeen(const char* publicKey) {
    int index = findNodeIndex(publicKey);
    if (index >= 0) {
        nodes[index].lastSeen = millis();
    }
}

uint8_t Whitelist::getNodeCount() {
    return nodeCount;
}

WhitelistNode* Whitelist::getNodeAt(uint8_t index) {
    uint8_t activeIndex = 0;
    for (int i = 0; i < MAX_WHITELIST_NODES; i++) {
        if (nodes[i].active) {
            if (activeIndex == index) {
                return &nodes[i];
            }
            activeIndex++;
        }
    }
    return nullptr;
}

void Whitelist::save() {
    prefs.begin(PREFS_NAMESPACE, false);
    
    // Save node count
    prefs.putUChar("node_count", nodeCount);
    
    // Save each node
    uint8_t savedCount = 0;
    for (int i = 0; i < MAX_WHITELIST_NODES && savedCount < nodeCount; i++) {
        if (nodes[i].active) {
            char keyBase[16];
            snprintf(keyBase, sizeof(keyBase), "node%d_", savedCount);
            
            String keyStr = String(keyBase) + "key";
            prefs.putString(keyStr.c_str(), nodes[i].publicKey);
            
            String nameStr = String(keyBase) + "name";
            prefs.putString(nameStr.c_str(), nodes[i].name);
            
            savedCount++;
        }
    }
    
    prefs.end();
    DEBUG_PRINTF("Whitelist saved: %d nodes\n", savedCount);
}

void Whitelist::load() {
    prefs.begin(PREFS_NAMESPACE, true);  // read-only
    
    uint8_t savedCount = prefs.getUChar("node_count", 0);
    
    if (savedCount > 0) {
        DEBUG_PRINTF("Loading %d nodes from preferences\n", savedCount);
        
        nodeCount = 0;
        for (uint8_t i = 0; i < savedCount && i < MAX_WHITELIST_NODES; i++) {
            char keyBase[16];
            snprintf(keyBase, sizeof(keyBase), "node%d_", i);
            
            String keyStr = String(keyBase) + "key";
            String publicKey = prefs.getString(keyStr.c_str(), "");
            
            if (publicKey.length() > 0) {
                String nameStr = String(keyBase) + "name";
                String name = prefs.getString(nameStr.c_str(), publicKey.c_str());
                
                strncpy(nodes[nodeCount].publicKey, publicKey.c_str(), 
                       sizeof(nodes[nodeCount].publicKey) - 1);
                strncpy(nodes[nodeCount].name, name.c_str(), NODE_NAME_MAX_LENGTH);
                nodes[nodeCount].active = true;
                nodes[nodeCount].batteryLevel = 0;
                nodes[nodeCount].lastSeen = 0;
                nodeCount++;
                
                DEBUG_PRINTF("Loaded node: %s\n", name.c_str());
            }
        }
    }
    
    prefs.end();
}

void Whitelist::clear() {
    for (int i = 0; i < MAX_WHITELIST_NODES; i++) {
        nodes[i].active = false;
    }
    nodeCount = 0;
    save();
    DEBUG_PRINTLN("Whitelist cleared");
}

int Whitelist::findNodeIndex(const char* publicKey) {
    if (!publicKey || publicKey[0] == '\0') {
        return -1;
    }
    
    for (int i = 0; i < MAX_WHITELIST_NODES; i++) {
        if (nodes[i].active && strcmp(nodes[i].publicKey, publicKey) == 0) {
            return i;
        }
    }
    
    return -1;
}
