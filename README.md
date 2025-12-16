# Meshtastic-Solar Emergency Signal System

Meshtastic-based emergency signal system for solar panel systems and general emergency applications.

## Overview

This system uses Meshtastic mesh networking to send emergency signals between nodes. When an emergency is triggered (via button press or received message), the system activates a relay and notifies other whitelisted nodes.

## Features

- **Button Input**: Physical button trigger for emergency signals
- **Relay Control**: Controls relay via IO4 (D04) through electro-optic coupler
- **Mesh Communication**: Sends emergency signals to whitelisted nodes via Meshtastic
- **OLED Display**: Shows incoming messages on RAK1921 OLED display
- **Whitelist Security**: Only processes messages from authorized nodes
- **Acknowledgments**: Sends ACK messages after relay activation
- **Emergency ON/OFF**: Remote control of relay via Meshtastic messages

## Hardware Requirements

- **Board**: RAK4631 (nRF52840-based WisBlock Core)
- **Base Board**: RAK19007 WisBlock Base Board (or compatible)
- **Display**: RAK1921 OLED Display Module (connected to Slot A on RAK19007)
- **Button**: Connected to IO6 (with internal pull-up)
- **Relay**: Connected to IO4 via electro-optic coupler

## Pin Configuration

| Function | Pin | Description |
|----------|-----|-------------|
| Relay Control | IO4 (WB_IO4) | High = Relay ON |
| Button Input | IO6 (WB_IO6) | Active LOW (internal pull-up) |
| OLED I2C SDA | I2C_SDA | Standard I2C connection |
| OLED I2C SCL | I2C_SCL | Standard I2C connection |

## Software Requirements

- PlatformIO IDE or PlatformIO Core
- Visual Studio Code (recommended)

## Installation & Building

### 1. Clone the Repository

```bash
git clone https://github.com/RobAbz/Meshtastic-Solar.git
cd Meshtastic-Solar
```

### 2. Open in PlatformIO

- Open Visual Studio Code
- Install PlatformIO extension if not already installed
- Open the Meshtastic-Solar folder
- PlatformIO will automatically detect the project

### 3. Build the Project

```bash
# Using PlatformIO CLI
pio run

# Or in VS Code: Click the Build button in PlatformIO toolbar
```

### 4. Upload to Device

```bash
# Using PlatformIO CLI
pio run --target upload

# Or in VS Code: Click the Upload button in PlatformIO toolbar
```

**Note**: For RAK4631, you may need to enter bootloader mode by double-pressing the reset button. The device will appear as a USB drive, and you can drag/drop the firmware file.

## Configuration

### Whitelist Configuration

Edit the `include/config.h` file to add authorized node public keys:

```cpp
const char* WHITELIST[] = {
    "0mmK16HVkgbaV+2Pb+RLa4gStG1RCkmx-WWKRh12dlhY=",
    // Add more public keys here
};
```

### Display Settings

The display uses the `u8g2_font_t0_11_mf` font (8 pixels high) as specified. You can adjust display timing in `config.h`:

```cpp
#define MESSAGE_DISPLAY_TIME_MS 30000  // Message display duration
#define DISPLAY_UPDATE_MS 100          // Display refresh rate
```

## Usage

### Triggering Emergency Signal

**Local Trigger**: Press the button connected to IO6
- Activates relay (IO4 goes HIGH)
- Sends "Emergency_signal ON" to whitelisted nodes
- Displays "EMERGENCY ACTIVATED" on OLED
- Sends acknowledgment message

**Remote Trigger**: Send "Emergency_signal ON" message via Meshtastic
- Only processed if sender is whitelisted
- Activates relay
- Displays received message on OLED
- Sends acknowledgment back

### Deactivating Emergency Signal

Send "Emergency_signal OFF" message via Meshtastic
- Deactivates relay (IO4 goes LOW)
- Updates display status

## Message Protocol

| Message | Action |
|---------|--------|
| `Emergency_signal ON` | Activates relay and notifies nodes |
| `Emergency_signal OFF` | Deactivates relay |
| `Emergency_signal ACK` | Acknowledgment message |

## Display Information

The RAK1921 OLED displays:
- Header with system name
- Current relay status (ON/OFF)
- Incoming messages from whitelisted nodes (last 7 messages)
- Messages auto-expire after 30 seconds

## Architecture

The project follows the open architecture principle:
- All source code is included in this repository
- Based on standard Arduino framework for RAK4631
- Uses official Meshtastic library for mesh communication
- Uses U8g2 library for OLED display
- Complete PlatformIO configuration included

## Dependencies

Dependencies are automatically managed by PlatformIO (defined in `platformio.ini`):

- `meshtastic/Meshtastic` - Mesh networking library
- `olikraus/U8g2` - OLED display library
- `adafruit/Adafruit Unified Sensor` - Sensor abstraction
- `adafruit/Adafruit BusIO` - I2C/SPI support

## Development

### Project Structure

```
Meshtastic-Solar/
├── platformio.ini      # PlatformIO configuration
├── include/
│   └── config.h        # System configuration and constants
├── src/
│   └── main.cpp        # Main application code
├── lib/                # Local libraries (if needed)
└── README.md           # This file
```

### Adding Features

The codebase is designed to be extensible:
- Add new message types in `processIncomingMessage()`
- Modify display layout in `updateDisplay()`
- Add more whitelisted nodes in `config.h`
- Customize timing and behavior in `config.h`

## Troubleshooting

### Display Not Working
- Check I2C connections to Slot A on RAK19007
- Verify I2C address is 0x3C (standard for RAK1921)
- Check serial output for initialization messages

### Relay Not Activating
- Verify relay is connected to IO4
- Check electro-optic coupler connections
- Use serial monitor to verify button press detection

### Build Errors
- Ensure PlatformIO is up to date
- Try cleaning the build: `pio run --target clean`
- Check that all dependencies are installed

### Upload Issues
- Enter bootloader mode (double-press reset)
- Try different USB cable or port
- Use J-Link programmer if available

## Serial Monitor

Connect serial monitor at 115200 baud to see debug output:
- System initialization messages
- Button press detection
- Message send/receive events
- Relay state changes

## License

This project is open source. Please refer to the repository for license details.

## Contributing

Contributions are welcome! Please submit pull requests or open issues for bugs and feature requests.

## Support

For questions and support, please open an issue on the GitHub repository.
