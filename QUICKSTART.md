# Quick Start Guide

Get your Meshtastic-Solar Emergency Signal System running in minutes!

## Prerequisites

- RAK4631 WisBlock Core module
- RAK19007 WisBlock Base Board (or compatible)
- RAK1921 OLED Display module
- Button (for IO6)
- Relay with electro-optic coupler
- USB-C cable
- PlatformIO installed (VS Code extension or CLI)

## Hardware Assembly

1. **Install RAK4631**: Insert the RAK4631 into the CPU slot on RAK19007 base board
2. **Install RAK1921 OLED**: Connect RAK1921 to Slot A (IO slot) on RAK19007
3. **Connect Button**: Wire button between IO6 (WB_IO6) and GND (button pulls IO6 LOW when pressed)
4. **Connect Relay**: Wire relay control through electro-optic coupler to IO4 (WB_IO4)

## Software Setup

### Step 1: Clone Repository

```bash
git clone https://github.com/RobAbz/Meshtastic-Solar.git
cd Meshtastic-Solar
```

### Step 2: Open in VS Code with PlatformIO

```bash
code .
```

Or open VS Code and use File → Open Folder

### Step 3: Build the Project

In VS Code:
- Click the PlatformIO icon in the left sidebar
- Click "Build" under rak4631 environment

Or using CLI:
```bash
pio run -e rak4631
```

### Step 4: Upload to RAK4631

**Method A: Using PlatformIO Upload (requires J-Link)**
```bash
pio run -e rak4631 --target upload
```

**Method B: Using Bootloader (No J-Link needed)**
1. Double-press the reset button on RAK4631
2. The device appears as a USB drive named "RAK4631"
3. Copy the `.uf2` file from `.pio/build/rak4631/` to the RAK4631 drive
4. The device will automatically reset and run the new firmware

### Step 5: Open Serial Monitor

```bash
pio device monitor -e rak4631
```

Or in VS Code: Click "Serial Monitor" in PlatformIO toolbar

## First Test

### Test 1: Display

After upload, you should see on the OLED:
```
Meshtastic Solar
RELAY:OFF
System Ready
```

### Test 2: Button

Press the button connected to IO6:
- Relay should activate (IO4 goes HIGH)
- Display shows "EMERGENCY ACTIVATED"
- Serial monitor shows button press messages

### Test 3: Serial Commands

While not a full Meshtastic implementation yet, you can see debug output:
```
Meshtastic-Solar Emergency System starting...
Relay pin configured (IO4)
Button pin configured (IO6)
I2C initialized
Display initialized
Meshtastic Handler: Initializing...
Meshtastic Handler: Ready (stub mode)
Setup complete - System ready
```

## Configuration

### Add Nodes to Whitelist

Edit `include/config.h`:

```cpp
const char* WHITELIST[] = {
    "0mmK16HVkgbaV+2Pb+RLa4gStG1RCkmx-WWKRh12dlhY=",  // Existing
    "YOUR_NODE_PUBLIC_KEY_HERE==",                     // Add yours
};
```

Then rebuild and upload.

## Next Steps

### For Full Meshtastic Integration

See `INTEGRATION.md` for detailed instructions on integrating with the Meshtastic firmware.

The current code provides a working foundation with:
- ✅ Button input working
- ✅ Relay control working
- ✅ OLED display working
- ✅ Whitelist system implemented
- ⏳ Meshtastic communication (stub - needs integration)

### Testing Without Meshtastic

You can fully test the hardware functionality:
1. Button triggers relay
2. Display shows messages
3. Serial output confirms all operations

For actual mesh networking, follow the integration guide.

## Troubleshooting

### Display shows nothing
- Check RAK1921 is in Slot A IO connector
- Try I2C scanner (see examples/TESTING.md)
- Verify I2C address is 0x3C

### Button doesn't work
- Check button connection (IO6 to GND)
- Verify pull-up is enabled in code (it is by default)
- Test with serial monitor output

### Relay doesn't activate
- Check relay wiring to IO4
- Use multimeter to test IO4 voltage
- Verify electro-optic coupler connections

### Upload fails
- Try bootloader method (double-press reset)
- Check USB cable (must support data)
- Try different USB port
- Update RAK4631 bootloader if very old

### Build errors
- Ensure PlatformIO is up to date
- Clean build: `pio run -t clean`
- Delete `.pio` folder and rebuild

## Getting Help

1. Check `examples/TESTING.md` for detailed test procedures
2. Review `INTEGRATION.md` for Meshtastic integration
3. Open an issue on GitHub with:
   - Hardware setup description
   - Serial monitor output
   - Steps to reproduce problem

## Success Checklist

- [ ] Hardware assembled correctly
- [ ] Firmware compiles without errors
- [ ] Firmware uploaded successfully
- [ ] OLED display shows startup message
- [ ] Button press activates relay
- [ ] Serial monitor shows debug messages
- [ ] Ready for Meshtastic integration

Congratulations! Your hardware is working and ready for full Meshtastic integration.
