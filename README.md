# Meshtastic-Solar Emergency Shutdown System

A comprehensive emergency shutdown system for solar panel installations using Meshtastic mesh networking on RAK4631 hardware.

## Overview

This system monitors a button and Meshtastic messages from whitelisted nodes to control an emergency shutdown relay. When activated, it can remotely shut down solar panel systems or other critical equipment.

### Key Features

- **Emergency Button**: Physical button on AIN1 triggers shutdown
- **Mesh Network Control**: Remote shutdown via Meshtastic from whitelisted nodes
- **Relay Control**: Controls RAK13007 relay board on IO Slot
- **OLED Display**: Visual status with auto-off power saving
- **Web Configuration**: Browser-based setup and monitoring
- **Whitelist Security**: Only authorized nodes can control the system
- **Message Acknowledgment**: Reliable delivery with retry logic
- **Low Power**: Deep sleep mode when idle

## Hardware Requirements

- **RAK4631** WisBlock Core module (nRF52840)
- **RAK19007** WisBlock Base Board
- **RAK13007** WisBlock Relay Module (IO Slot)
- **Button** connected to AIN1 (normally low, >2.9V when pressed)
- **OLED Display** (included with RAK4631)

## Quick Start Guide (For Beginners)

### Step 1: Install PlatformIO

1. **Install Visual Studio Code**
   - Download from: https://code.visualstudio.com/
   - Install and open VS Code

2. **Install PlatformIO Extension**
   - In VS Code, click the Extensions icon (left sidebar)
   - Search for "PlatformIO IDE"
   - Click "Install"
   - Wait for installation to complete
   - Restart VS Code if prompted

### Step 2: Get the Code

1. **Clone this repository**
   ```bash
   git clone https://github.com/RobAbz/Meshtastic-Solar.git
   cd Meshtastic-Solar
   ```

   Or download as ZIP:
   - Click the green "Code" button on GitHub
   - Select "Download ZIP"
   - Extract the ZIP file

2. **Open in VS Code**
   - In VS Code: File → Open Folder
   - Navigate to the Meshtastic-Solar folder
   - Click "Select Folder"

### Step 3: Compile the Firmware

1. **Wait for PlatformIO to Initialize**
   - When you first open the project, PlatformIO will:
   - Download the platform tools (this may take 5-10 minutes)
   - Download required libraries
   - You'll see progress in the bottom status bar

2. **Build the Project**
   - Click the PlatformIO icon in the left sidebar (alien head icon)
   - Under "Project Tasks" → "rak4631" → "General"
   - Click "Build"
   - Wait for compilation to complete
   - Success message will appear when done

### Step 4: Upload to Your RAK4631

**Method A: Using USB (Recommended for Beginners)**

1. **Enter Bootloader Mode**
   - Connect your RAK4631 to your computer via USB
   - Double-press the reset button on the RAK4631
   - The device will appear as a USB drive named "RAK4631"

2. **Upload Firmware**
   - In the PlatformIO sidebar, click "Upload"
   - Or: Drag the `.uf2` file from `.pio/build/rak4631/` to the RAK4631 drive

**Method B: Using J-Link (Advanced)**

1. Connect J-Link programmer to RAK4631
2. In PlatformIO sidebar, click "Upload"

### Step 5: Monitor Serial Output

1. **Open Serial Monitor**
   - In PlatformIO sidebar → "Monitor"
   - Or click the plug icon in the bottom toolbar
   - Baud rate: 115200

2. **View Debug Messages**
   - You'll see initialization messages
   - Button press events
   - Relay state changes
   - Meshtastic message traffic

## Configuration

### Initial Whitelist

The system comes with one pre-configured whitelisted node:
```
Public Key: 0mmK16HVgbaV+2Pb+RLa4ggStG1RCkmx-WwkRh12d1hY=
```

### Web Configuration

After the system boots:
1. Connect to the device's WiFi network (if configured)
2. Open a browser to the device's IP address
3. Configure:
   - Time and date
   - Additional whitelist nodes
   - Node names
   - Power settings
   - View message logs

## How It Works

### Emergency Activation

1. **Button Press**: Press button on AIN1 for >1 second
   - Relay engages (turns ON)
   - "Emergency Shutdown Initiated" displays with timestamp
   - Message sent to all whitelisted nodes

2. **Meshtastic Message**: Whitelisted node sends "Emergency_Shutdown ON"
   - System validates sender is whitelisted
   - Relay engages
   - Acknowledgment sent back
   - Status displayed on OLED

### Emergency Deactivation

1. **Reset**: Press reset button on RAK4631
   - Relay disengages (turns OFF)
   - "Emergency Shutdown Cancelled" displays with timestamp

2. **Meshtastic Message**: Whitelisted node sends "Emergency_Shutdown OFF"
   - Relay disengages
   - Acknowledgment sent back
   - Display turns off after 30 seconds

3. **Web Interface**: Set Emergency Shutdown = OFF
   - Relay disengages
   - Status updated

### Power Management

- **Active**: Medium CPU speed, OLED on when relay active
- **Idle**: Deep sleep, wakes on button or message
- **Display**: Auto-off 30 seconds after relay goes OFF

## Troubleshooting

### Build Errors

**Problem**: "Platform nordicnrf52 not found"
- **Solution**: Wait for PlatformIO to finish downloading platforms
- Check the PlatformIO Home → Platforms tab

**Problem**: "Library not found"
- **Solution**: PlatformIO will auto-download libraries
- Or manually: PlatformIO → Libraries → Search and Install

### Upload Issues

**Problem**: Device not recognized
- **Solution**: 
  - Try a different USB cable (must support data)
  - Install RAK4631 drivers from RAKwireless website
  - Try different USB port

**Problem**: Upload fails
- **Solution**: 
  - Enter bootloader mode (double-press reset)
  - Check that RAK4631 appears as USB drive
  - Try Method A (drag .uf2 file)

### Runtime Issues

**Problem**: Button not responding
- **Solution**: 
  - Check button connection to AIN1
  - Verify voltage >2.9V when pressed
  - Check serial monitor for button events

**Problem**: Relay not activating
- **Solution**: 
  - Verify RAK13007 is in IO Slot
  - Check relay module LED
  - Verify relay can handle your load

## Technical Details

### Pin Configuration

- **AIN1**: Emergency button input (analog, 0-3.3V)
- **IO Slot**: RAK13007 relay module
- **I2C**: OLED display (built-in)

### Message Protocol

- `Emergency_Shutdown ON`: Activate relay
- `Emergency_Shutdown OFF`: Deactivate relay
- `ACK`: Acknowledgment message

### Power Consumption

- **Active (relay ON)**: ~50mA
- **Idle (relay OFF, display off)**: ~5mA
- **Deep Sleep**: <1mA

## Support

### Getting Help

1. Check the serial monitor output for error messages
2. Verify hardware connections
3. Check that button voltage is correct
4. Ensure Meshtastic nodes can communicate

### Common Questions

**Q: How do I add more nodes to the whitelist?**
A: Use the web configuration interface to add public keys

**Q: Can I change the button pin?**
A: Yes, modify the configuration in `include/config.h`

**Q: How long does the battery last?**
A: Depends on usage, but deep sleep mode significantly extends battery life

**Q: Can I use a different relay?**
A: The code is designed for RAK13007, but can be adapted

## License

© SilverTime 2025

This project is designed for emergency shutdown applications. Test thoroughly before deploying in critical systems.

## Version History

- **v1.0**: Initial release with core functionality
  - Button and Meshtastic control
  - Whitelist security
  - OLED display
  - Web configuration
  - Power management
