#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <Arduino.h>
#include "config.h"

/**
 * Web Server Module
 * Provides web-based configuration and monitoring interface
 * 
 * Note: This is a placeholder interface. Full implementation requires
 * integration with ESP32 WiFi or similar networking capability.
 * For RAK4631 (nRF52), you may need:
 * - External WiFi module (e.g., ESP32 via serial)
 * - BLE-based configuration app
 * - Or use Meshtastic's built-in web interface
 */

class WebServer {
public:
    /**
     * Initialize web server
     */
    static void init();
    
    /**
     * Handle web requests
     * Called periodically from main loop
     */
    static void handle();
    
    /**
     * Check if web server is running
     */
    static bool isRunning();
    
    /**
     * Stop web server
     */
    static void stop();
    
    /**
     * Start web server
     */
    static void start();
    
    // Web API endpoints (to be implemented):
    // GET  /api/status - Get current system status
    // POST /api/relay - Set relay state
    // GET  /api/nodes - Get whitelist nodes
    // POST /api/nodes - Add node to whitelist
    // DELETE /api/nodes/:key - Remove node
    // GET  /api/log - Get message log
    // POST /api/log/clear - Clear message log
    // GET  /api/time - Get current time
    // POST /api/time - Set time
    // GET  /api/config - Get configuration
    // POST /api/config - Update configuration

private:
    static bool serverRunning;
    
    /**
     * Handle status request
     */
    static void handleStatus();
    
    /**
     * Handle relay control request
     */
    static void handleRelay();
    
    /**
     * Handle whitelist requests
     */
    static void handleNodes();
    
    /**
     * Handle log requests
     */
    static void handleLog();
    
    /**
     * Handle time requests
     */
    static void handleTime();
    
    /**
     * Handle configuration requests
     */
    static void handleConfig();
    
    /**
     * Generate JSON response
     */
    static String generateJSON();
};

#endif // WEBSERVER_H
