# Meshtastic Firmware Integration Guide

This document explains how to integrate the Meshtastic-Solar emergency shutdown system with the full Meshtastic firmware.

## Current Status

The current implementation is a **standalone PlatformIO project** with:
- ✅ Full hardware control (button, relay, display)
- ✅ Whitelist management with node tracking
- ✅ Message protocol defined
- ✅ Acknowledgment and retry logic
- ⏳ **Meshtastic mesh networking (placeholder)**

## Integration Options

### Option 1: Standalone with External Meshtastic Node (Current)

**Status**: ✅ Implemented

**Description**: Run this code on RAK4631 and communicate with separate Meshtastic device via serial

**Pros**:
- Completely isolated system
- Easy to develop and test
- No Meshtastic firmware modifications needed

**Cons**:
- Requires two devices
- Serial communication overhead
- Can't leverage Meshtastic's power management

**Implementation**: Add serial communication to/from external Meshtastic device in `messaging.cpp`

### Option 2: Meshtastic Firmware Module (Recommended)

**Status**: ⏳ Future work

**Description**: Integrate as a custom module within Meshtastic firmware

**Pros**:
- Single device solution
- Full Meshtastic features
- Optimal power management
- Official Meshtastic tooling

**Cons**:
- Requires Meshtastic firmware knowledge
- More complex build process
- Must maintain compatibility with Meshtastic updates

**Implementation Steps**:

1. **Clone Meshtastic Firmware**:
   ```bash
   git clone https://github.com/meshtastic/firmware.git
   cd firmware
   git submodule update --init --recursive
   ```

2. **Create Custom Module**:
   ```bash
   mkdir -p src/modules/EmergencyShutdownModule
   ```

3. **Adapt Code**:
   - Move core logic to Meshtastic module structure
   - Extend `SinglePortModule` or `ProtobufModule`
   - Use Meshtastic's radio, display, and power APIs

4. **Example Module Structure**:
   ```cpp
   class EmergencyShutdownModule : public SinglePortModule {
   public:
       EmergencyShutdownModule() 
           : SinglePortModule("emergency", PortNum_PRIVATE_APP) {}
       
   protected:
       virtual bool handleReceived(const meshtastic_MeshPacket &mp) override {
           // Process incoming messages
           // Check whitelist
           // Control relay
           // Update display
           return true;
       }
       
       void sendEmergencySignal(bool activate) {
           // Create and send packet via Meshtastic
       }
   };
   ```

5. **Register Module**:
   - Add to `src/main.cpp`: `emergencyModule = new EmergencyShutdownModule();`
   - Add to module list

6. **Build for RAK4631**:
   ```bash
   pio run -e rak4631
   ```

### Option 3: Direct LoRa Integration

**Status**: ⏳ Possible but not recommended

**Description**: Implement LoRa radio communication directly without Meshtastic

**Pros**:
- Complete control over radio
- Minimal code dependencies

**Cons**:
- Loses Meshtastic mesh networking
- No standard protocol
- Must implement radio layer
- Incompatible with Meshtastic network

**Not Recommended**: Use Option 1 or 2 instead

## Key Integration Points

### 1. Message Transmission

**Current (Placeholder)**:
```cpp
bool Messaging::transmitMessage(const char* publicKey, const char* message) {
    DEBUG_PRINTLN("PLACEHOLDER: Would send via Meshtastic");
    return true;
}
```

**With Meshtastic**:
```cpp
bool Messaging::transmitMessage(const char* publicKey, const char* message) {
    meshtastic_MeshPacket *p = router->allocForSending();
    p->to = getNodeNumByPublicKey(publicKey);
    p->decoded.portnum = PortNum_TEXT_MESSAGE_APP;
    
    memcpy(p->decoded.payload.bytes, message, strlen(message));
    p->decoded.payload.size = strlen(message);
    
    service.sendToMesh(p);
    return true;
}
```

### 2. Message Reception

**Integration Point**: Register callback with Meshtastic router

```cpp
void setupMessageCallback() {
    // Register with Meshtastic message router
    router->addReceiveCallback([](const meshtastic_MeshPacket &mp) {
        if (mp.decoded.portnum == PortNum_TEXT_MESSAGE_APP) {
            char message[128];
            memcpy(message, mp.decoded.payload.bytes, mp.decoded.payload.size);
            message[mp.decoded.payload.size] = '\0';
            
            // Get sender's public key
            const char* senderKey = getPublicKeyByNodeNum(mp.from);
            
            // Call our handler
            onMeshtasticMessage(senderKey, message, 
                              getBatteryLevel(mp.from));
        }
    });
}
```

### 3. Power Management

**Current**: Placeholder deep sleep

**With Meshtastic**: Use Meshtastic's power management

```cpp
void Hardware::enterDeepSleep() {
    // Use Meshtastic's power manager
    powerFSM.trigger(EVENT_PRESS);  // or appropriate event
}
```

### 4. Display Integration

**Current**: Direct U8g2 control

**With Meshtastic**: Coordinate with Meshtastic's screen management

```cpp
// Use Meshtastic's screen framework
screen->setFrames();
screen->addFrame(emergencyFrame);
```

## File Mapping

### Standalone → Meshtastic Module

| Standalone File | Meshtastic Location |
|----------------|---------------------|
| `src/main.cpp` | `src/modules/EmergencyShutdownModule/EmergencyShutdownModule.cpp` |
| `include/*.h` | `src/modules/EmergencyShutdownModule/*.h` |
| `src/hardware.cpp` | Use Meshtastic HAL functions |
| `src/messaging.cpp` | Use Meshtastic router/service |
| `src/display.cpp` | Use Meshtastic screen framework |
| `src/whitelist.cpp` | Use Meshtastic NodeDB + custom list |

## Testing Strategy

### Phase 1: Standalone Testing (Current)
- ✅ Test hardware without mesh networking
- ✅ Verify button, relay, display
- ✅ Test whitelist logic
- ✅ Validate message protocol

### Phase 2: Serial Bridge Testing
- Add serial communication to external Meshtastic
- Test message exchange
- Verify end-to-end functionality

### Phase 3: Full Integration Testing
- Build as Meshtastic module
- Test on single node
- Test multi-node mesh
- Stress test retry logic

## Migration Path

### Step 1: Prepare Code
1. Review all `TODO` and `PLACEHOLDER` comments
2. Document all Meshtastic dependencies
3. List required Meshtastic APIs

### Step 2: Create Module Structure
1. Set up Meshtastic build environment
2. Create module directory
3. Copy and adapt source files

### Step 3: Replace Placeholders
1. Implement actual message transmission
2. Add message reception callback
3. Integrate with Meshtastic power management
4. Coordinate display usage

### Step 4: Test Integration
1. Build and flash
2. Test basic functionality
3. Test mesh networking
4. Verify power consumption

### Step 5: Optimize
1. Reduce code size if needed
2. Optimize power usage
3. Fine-tune radio parameters

## Helpful Resources

- **Meshtastic Firmware**: https://github.com/meshtastic/firmware
- **Module API Documentation**: https://meshtastic.org/docs/development/device/module-api/
- **Building Firmware**: https://meshtastic.org/docs/development/firmware/build/
- **RAK4631 Variant**: `firmware/variants/rak4631/`
- **Community Forum**: https://meshtastic.discourse.group/

## Current vs. Full Integration

### What Works Now (Standalone)
- ✅ Button triggers emergency
- ✅ Relay control
- ✅ OLED display
- ✅ Whitelist management
- ✅ Message protocol
- ✅ Retry logic
- ✅ Message logging
- ✅ Preferences storage

### What Needs Meshtastic
- ⏳ Actual LoRa transmission
- ⏳ Mesh routing
- ⏳ Node discovery
- ⏳ Public key exchange
- ⏳ Coordinated power management
- ⏳ GPS integration (optional)
- ⏳ Web interface via WiFi/BLE

## Recommendations

### For Quick Testing
Use **Option 1** (Standalone) with serial bridge to external Meshtastic node

### For Production Deployment
Use **Option 2** (Meshtastic Module) for full integration

### For Learning
Start with Option 1, understand the system, then migrate to Option 2

## Next Steps

1. **Test Current Implementation**:
   - Upload to RAK4631
   - Verify all hardware functions
   - Test without mesh networking

2. **Add Serial Bridge** (Optional):
   - Connect to external Meshtastic via serial
   - Implement serial protocol
   - Test message exchange

3. **Plan Full Integration**:
   - Study Meshtastic module API
   - Plan code migration
   - Set up Meshtastic build environment

4. **Implement Module**:
   - Create module structure
   - Migrate code
   - Test and debug

## Support

For Meshtastic integration questions:
- Meshtastic Discord: https://discord.gg/meshtastic
- Forum: https://meshtastic.discourse.group/
- GitHub Issues: https://github.com/meshtastic/firmware/issues

For this project:
- GitHub Issues: https://github.com/RobAbz/Meshtastic-Solar/issues

---

© SilverTime 2025
