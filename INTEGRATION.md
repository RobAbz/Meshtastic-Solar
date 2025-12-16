# Meshtastic Integration Guide

## Overview

This project provides a foundation for Meshtastic-based emergency signaling. The current implementation includes placeholder functions for Meshtastic communication that need to be replaced with actual Meshtastic firmware integration.

## Integration Approaches

There are three main approaches to integrate this system with Meshtastic:

### Option 1: Custom Meshtastic Firmware Module (Recommended)

Build this as a custom module within the official Meshtastic firmware repository.

**Steps:**

1. Clone the Meshtastic firmware repository:
   ```bash
   git clone https://github.com/meshtastic/firmware.git
   cd firmware
   git submodule update --init --recursive
   ```

2. Create a custom module in `src/modules/`:
   ```bash
   mkdir -p src/modules/EmergencyModule
   ```

3. Integrate this code as a module that extends `SinglePortModule` or `ProtobufModule`

4. Example module skeleton:
   ```cpp
   class EmergencyModule : public SinglePortModule<meshtastic_EmergencySignal> {
   public:
       EmergencyModule() : SinglePortModule("emergency", meshtastic_PortNum_PRIVATE_APP) {}
       
   protected:
       virtual bool handleReceived(const meshtastic_MeshPacket &mp) override {
           // Handle incoming emergency messages
           // Check whitelist
           // Control relay
           // Update display
           return true;
       }
       
       void sendEmergencySignal(bool activate) {
           // Create and send packet
       }
   };
   ```

5. Register the module in `src/main.cpp`:
   ```cpp
   emergencyModule = new EmergencyModule();
   ```

6. Build for RAK4631:
   ```bash
   pio run -e rak4631
   ```

### Option 2: Meshtastic Serial Module

Use this Arduino project with an external Meshtastic device via serial connection.

**Requirements:**
- RAK4631 running this code
- Separate Meshtastic device (e.g., another RAK4631 with Meshtastic firmware)
- Serial connection between them

**Implementation:**
- Use Meshtastic Serial Protocol
- Replace `meshtastic_handler.cpp` functions with serial communication
- Use the Meshtastic-Arduino library or direct serial protocol

### Option 3: Direct Radio Integration

Implement LoRa radio communication directly without using the full Meshtastic stack.

**Note:** This approach loses Meshtastic mesh networking features but gives you full control.

## Real Meshtastic API Integration

When integrating with actual Meshtastic firmware, replace the placeholder functions in `meshtastic_handler.cpp`:

### Sending Messages

```cpp
void MeshtasticHandler::sendMessage(const char* message) {
    meshtastic_MeshPacket *p = allocDataPacket();
    p->decoded.portnum = meshtastic_PortNum_TEXT_MESSAGE_APP;
    
    // Set payload
    p->decoded.payload.size = strlen(message);
    memcpy(p->decoded.payload.bytes, message, p->decoded.payload.size);
    
    // Send to mesh
    service.sendToMesh(p);
}
```

### Receiving Messages

```cpp
// In your module's handleReceived() method:
bool EmergencyModule::handleReceived(const meshtastic_MeshPacket &mp) {
    auto &p = mp.decoded;
    
    // Extract message
    String message((char*)p.payload.bytes, p.payload.size);
    
    // Get sender's public key for whitelist check
    NodeNum sender = mp.from;
    const meshtastic_NodeInfo *senderNode = nodeDB.getNode(sender);
    
    if (senderNode && isNodeWhitelisted(senderNode->user.public_key)) {
        // Process message
        if (message == "Emergency_signal ON") {
            activateRelay();
        }
    }
    
    return true;
}
```

### Whitelist Implementation

```cpp
bool isNodeWhitelisted(const char* publicKey) {
    // Compare against your whitelist
    for (int i = 0; i < WHITELIST_SIZE; i++) {
        if (strcmp(publicKey, WHITELIST[i]) == 0) {
            return true;
        }
    }
    return false;
}
```

## Node Public Key Management

### Getting a Node's Public Key

1. Using Meshtastic CLI:
   ```bash
   meshtastic --info
   ```
   Look for the "Public Key" field.

2. Using Python:
   ```python
   import meshtastic
   interface = meshtastic.SerialInterface()
   info = interface.getMyNodeInfo()
   print(info['user']['publicKey'])
   ```

3. Via Meshtastic Web UI or Mobile App:
   - Settings → Radio Configuration → User
   - Public key is displayed in base64 format

### Adding Nodes to Whitelist

Update `include/config.h`:

```cpp
const char* WHITELIST[] = {
    "0mmK16HVkgbaV+2Pb+RLa4gStG1RCkmx-WWKRh12dlhY=",  // Node 1
    "AbCdEfGhIjKlMnOpQrStUvWxYz1234567890+/==",        // Node 2
    "YourNodePublicKeyBase64EncodedHere==",             // Node 3
};
```

## Testing Without Real Meshtastic

For development and testing without actual Meshtastic hardware/firmware:

1. The current stub implementation logs all actions to serial
2. You can manually inject test messages in the code:
   ```cpp
   // In loop(), for testing:
   if (Serial.available()) {
       String testMsg = Serial.readStringUntil('\n');
       processIncomingMessage(testMsg.c_str(), WHITELIST[0]);
   }
   ```

3. Use the serial monitor to trigger emergency signals and test display

## Hardware Setup

### RAK19007 Base Board Connections

- **Slot A (CPU)**: RAK4631 Core Module
- **Slot A (IO)**: RAK1921 OLED Display
- **IO4**: Relay control (via electro-optic coupler)
- **IO6**: Emergency button (with pull-up resistor)

### Relay Circuit

```
RAK4631 IO4 → Electro-optic Coupler → Relay Coil → 12V/24V Supply
                   ↓
                 Relay Contacts → Load
```

The electro-optic coupler provides electrical isolation between the RAK4631 and the relay.

## Security Considerations

1. **Whitelist Validation**: Always validate sender's public key before processing commands
2. **Message Authentication**: Meshtastic provides built-in encryption and authentication
3. **Relay Control**: Consider adding a timeout to automatically turn off the relay
4. **Physical Security**: The button is a direct trigger - secure it appropriately

## Additional Resources

- [Meshtastic Firmware Repository](https://github.com/meshtastic/firmware)
- [Meshtastic Module API Documentation](https://meshtastic.org/docs/development/device/module-api/)
- [RAK4631 Documentation](https://docs.rakwireless.com/product-categories/wisblock/rak4631/)
- [Meshtastic Protocol Documentation](https://meshtastic.org/docs/developers/protocol/)

## Support

For issues specific to this emergency system, open an issue on the GitHub repository.
For Meshtastic-specific questions, refer to the [Meshtastic community forums](https://meshtastic.org/docs/community/).
