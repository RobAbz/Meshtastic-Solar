# Testing the Emergency System

This directory contains test scenarios and examples for the Meshtastic-Solar Emergency System.

## Serial Monitor Testing

Since the current implementation includes stub Meshtastic functions, you can test the system using the serial monitor.

### Test 1: Button Press Simulation

1. Upload the firmware to your RAK4631
2. Open serial monitor at 115200 baud
3. Press the button connected to IO6
4. Expected output:
   ```
   Emergency button pressed!
   Relay set to: ON
   Sending message: Emergency_signal ON
   Meshtastic Handler: Sending message: Emergency_signal ON
   Display message added: EMERGENCY ACTIVATED
   Sending acknowledgment
   Sending message: Emergency_signal ACK
   ```

### Test 2: Display Testing

1. Upload firmware and observe the OLED display
2. Check that the display shows:
   - Header: "Meshtastic Solar"
   - Status: "RELAY:OFF" or "RELAY:ON"
   - Messages as they are added

### Test 3: Relay Testing

1. Connect a multimeter or LED to IO4
2. Press the button
3. Verify that IO4 goes HIGH (3.3V)
4. Messages should appear on display

### Test 4: Whitelist Verification

The system is configured to only accept messages from whitelisted nodes. The default whitelist includes:
- `0mmK16HVkgbaV+2Pb+RLa4gStG1RCkmx-WWKRh12dlhY=`

To test with real Meshtastic integration, you would:
1. Get the public key of a test node
2. Add it to the whitelist in `include/config.h`
3. Send an "Emergency_signal ON" message from that node
4. Verify the relay activates

## Message Protocol Tests

### Emergency ON
**Message:** `Emergency_signal ON`
**Expected Result:**
- Relay activates (IO4 HIGH)
- Display shows "RX: EMERGENCY ON"
- ACK message sent back

### Emergency OFF
**Message:** `Emergency_signal OFF`
**Expected Result:**
- Relay deactivates (IO4 LOW)
- Display shows "RX: EMERGENCY OFF"
- ACK message sent back

### Acknowledgment
**Message:** `Emergency_signal ACK`
**Expected Result:**
- Display shows "RX: ACK"
- No relay state change

## Hardware Tests

### Test the OLED Display

Create a minimal test sketch:

```cpp
#include <Wire.h>
#include <U8g2lib.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

void setup() {
    Wire.begin();
    u8g2.begin();
    u8g2.setFont(u8g2_font_t0_11_mf);
    u8g2.clearBuffer();
    u8g2.drawStr(0, 10, "Display Test OK");
    u8g2.sendBuffer();
}

void loop() {}
```

### Test the Relay

Create a relay blink test:

```cpp
#define RELAY_PIN WB_IO4

void setup() {
    pinMode(RELAY_PIN, OUTPUT);
}

void loop() {
    digitalWrite(RELAY_PIN, HIGH);
    delay(1000);
    digitalWrite(RELAY_PIN, LOW);
    delay(1000);
}
```

### Test the Button

Create a button test sketch:

```cpp
#define BUTTON_PIN WB_IO6
#define LED_PIN LED_BUILTIN

void setup() {
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT);
    Serial.begin(115200);
}

void loop() {
    if (digitalRead(BUTTON_PIN) == LOW) {
        digitalWrite(LED_PIN, HIGH);
        Serial.println("Button pressed!");
    } else {
        digitalWrite(LED_PIN, LOW);
    }
    delay(50);
}
```

## Integration Testing with Real Meshtastic

Once you've integrated with the actual Meshtastic firmware:

### Test 1: Node Discovery
1. Flash two RAK4631 boards with the integrated firmware
2. Verify they can see each other in the mesh
3. Check that public keys are correctly identified

### Test 2: Emergency Signal Chain
1. Set up three nodes (A, B, C) all with each other in whitelist
2. Press button on Node A
3. Verify Nodes B and C activate their relays
4. Check all nodes show messages on their displays

### Test 3: Whitelist Enforcement
1. Set up Node A with whitelist
2. Send emergency signal from Node B (not in whitelist)
3. Verify Node A ignores the message
4. Add Node B to whitelist
5. Resend message and verify it's processed

### Test 4: Range Testing
1. Set up two nodes with emergency system
2. Test at various distances
3. Verify messages are received and relays activate
4. Document maximum reliable range

## Troubleshooting Tests

### I2C Scanner

If the display doesn't work, run an I2C scanner:

```cpp
#include <Wire.h>

void setup() {
    Wire.begin();
    Serial.begin(115200);
    Serial.println("I2C Scanner");
}

void loop() {
    byte error, address;
    int nDevices = 0;
    
    Serial.println("Scanning...");
    
    for(address = 1; address < 127; address++) {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();
        
        if (error == 0) {
            Serial.print("I2C device found at address 0x");
            if (address < 16) Serial.print("0");
            Serial.println(address, HEX);
            nDevices++;
        }
    }
    
    if (nDevices == 0)
        Serial.println("No I2C devices found\n");
    else
        Serial.println("Scan complete\n");
        
    delay(5000);
}
```

Expected output: Device at address 0x3C (RAK1921 OLED)

## Performance Metrics

Track these metrics during testing:
- Button response time (should be < 100ms)
- Relay activation time (should be < 50ms)
- Message send latency (depends on mesh conditions)
- Display update rate (10 FPS with current settings)
- Message buffer capacity (7 messages)

## Safety Tests

Before deploying in a real emergency system:

1. **Relay Endurance**: Test relay switching for 1000+ cycles
2. **Power Loss Recovery**: Test system behavior after power interruption
3. **False Trigger Prevention**: Verify button debouncing works correctly
4. **Simultaneous Messages**: Test with multiple nodes sending simultaneously
5. **Long Runtime**: Run for 24+ hours to check for memory leaks or hangs

## Documentation

Record all test results in a test log:
- Date and time
- Hardware configuration
- Software version
- Test performed
- Results (pass/fail)
- Notes and observations
