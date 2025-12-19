# Complete PlatformIO Compilation Guide for Beginners

This guide will walk you through every step needed to compile and upload the Meshtastic-Solar firmware to your RAK4631 board.

## Prerequisites

### What You Need

1. **Computer**: Windows, Mac, or Linux
2. **RAK4631 Board**: WisBlock Core module on RAK19007 base
3. **USB Cable**: USB-C cable that supports data (not just charging)
4. **Internet Connection**: For downloading tools and libraries

### Optional but Recommended

- **RAK13007 Relay Module**: For testing relay functionality
- **Button**: Connected to AIN1 for emergency trigger
- **OLED Display**: Should be built into your RAK4631/RAK19007 setup

## Step 1: Install Visual Studio Code

### Windows

1. Go to https://code.visualstudio.com/
2. Click "Download for Windows"
3. Run the installer (VSCodeUserSetup-x64-1.x.x.exe)
4. Follow the installation wizard:
   - Accept the license agreement
   - Choose installation location (default is fine)
   - **Important**: Check "Add to PATH" option
   - Click Install

### Mac

1. Go to https://code.visualstudio.com/
2. Click "Download for Mac"
3. Open the downloaded .zip file
4. Drag Visual Studio Code to Applications folder
5. Open VS Code from Applications

### Linux (Ubuntu/Debian)

```bash
sudo apt update
sudo apt install software-properties-common apt-transport-https wget
wget -q https://packages.microsoft.com/keys/microsoft.asc -O- | sudo apt-key add -
sudo add-apt-repository "deb [arch=amd64] https://packages.microsoft.com/repos/vscode stable main"
sudo apt update
sudo apt install code
```

## Step 2: Install PlatformIO Extension

1. **Open VS Code**

2. **Open Extensions Panel**:
   - Click the Extensions icon in the left sidebar (looks like 4 squares)
   - Or press `Ctrl+Shift+X` (Windows/Linux) or `Cmd+Shift+X` (Mac)

3. **Search for PlatformIO**:
   - Type "PlatformIO IDE" in the search box
   - Look for the official extension by "PlatformIO"
   - Should have millions of installs

4. **Install the Extension**:
   - Click the "Install" button
   - Wait for installation (may take 2-5 minutes)
   - You'll see a "Reload Required" button when done

5. **Reload VS Code**:
   - Click "Reload" or close and reopen VS Code
   - Wait for PlatformIO to finish installing additional components
   - You'll see the PlatformIO icon (alien head) in the left sidebar when ready

## Step 3: Get the Meshtastic-Solar Code

### Option A: Clone with Git (Recommended)

**Install Git first** (if you don't have it):
- Windows: https://git-scm.com/download/win
- Mac: `brew install git` or use Xcode Command Line Tools
- Linux: `sudo apt install git`

**Clone the repository**:

```bash
cd ~/Documents  # or wherever you want to store the code
git clone https://github.com/RobAbz/Meshtastic-Solar.git
cd Meshtastic-Solar
```

### Option B: Download ZIP

1. Go to https://github.com/RobAbz/Meshtastic-Solar
2. Click the green "Code" button
3. Select "Download ZIP"
4. Extract the ZIP file to a location you'll remember
   - Windows: Right-click → Extract All
   - Mac: Double-click the ZIP file
   - Linux: `unzip Meshtastic-Solar-main.zip`

## Step 4: Open the Project in VS Code

1. **Open VS Code**

2. **Open Folder**:
   - Click "File" → "Open Folder..."
   - Navigate to the Meshtastic-Solar folder
   - Click "Select Folder"

3. **Wait for PlatformIO to Initialize**:
   - You'll see "PlatformIO: Loading..." in the bottom status bar
   - This may take 5-15 minutes the first time
   - PlatformIO is downloading:
     - Nordic nRF52 platform tools
     - Arduino framework
     - Required libraries (U8g2, ArduinoJson, etc.)

4. **Check Project Structure**:
   - You should see folders: `src/`, `include/`, `lib/`
   - And files: `platformio.ini`, `README.md`

## Step 5: Understand the PlatformIO Interface

### PlatformIO Sidebar

After clicking the PlatformIO icon (alien head), you'll see:

- **PIO Home**: Main dashboard
- **Project Tasks**: Your project's available commands
  - **rak4631**: Your board environment
    - **General**: Build, Upload, Clean, etc.
    - **Platform**: Platform-specific tasks
    - **Advanced**: Advanced build options

### Bottom Toolbar

The bottom toolbar has quick-access buttons:
- **Home** (house icon): PlatformIO Home
- **Build** (checkmark): Compile the project
- **Upload** (arrow): Upload to board
- **Clean**: Clean build files
- **Serial Monitor** (plug): View serial output

## Step 6: Build the Project

### First Build

1. **Click the Build Button**:
   - Use the checkmark icon in the bottom toolbar
   - Or: PlatformIO sidebar → Project Tasks → rak4631 → General → Build
   - Or: Press `Ctrl+Alt+B` (Windows/Linux) or `Cmd+Alt+B` (Mac)

2. **Wait for Compilation**:
   - First build will take longer (5-10 minutes)
   - You'll see compilation output in the terminal
   - Lots of text scrolling is normal

3. **Check for Success**:
   - Look for "SUCCESS" at the end
   - Build artifacts are in `.pio/build/rak4631/`
   - If you see errors, see Troubleshooting section below

### What Gets Built

- `firmware.elf`: Executable and Linkable Format (main binary)
- `firmware.bin`: Raw binary file
- `firmware.hex`: Intel HEX format
- `firmware.uf2`: USB Flashing Format (we'll use this!)

## Step 7: Connect Your RAK4631

### Physical Connection

1. **Connect USB Cable**:
   - Plug USB-C end into RAK4631 (on the RAK19007 base board)
   - Plug USB-A end into your computer

2. **Check Connection**:
   - **Windows**: Device Manager → Ports (COM & LPT) → Look for "USB Serial Device (COM X)"
   - **Mac**: Terminal → `ls /dev/cu.*` → Look for `/dev/cu.usbmodemXXXX`
   - **Linux**: Terminal → `ls /dev/ttyACM*` → Look for `/dev/ttyACM0`

3. **Driver Issues?**:
   - Windows: May need to install RAK4631 drivers from RAKwireless website
   - Mac/Linux: Usually works out of the box

## Step 8: Upload the Firmware

### Method A: Using UF2 Bootloader (Easiest for Beginners)

1. **Enter Bootloader Mode**:
   - Locate the RESET button on your RAK4631
   - **Double-press** the RESET button quickly (like a double-click)
   - The board will reset and appear as a USB drive

2. **Verify Bootloader Mode**:
   - A new drive named "RAK4631" should appear
   - **Windows**: Check "This PC" for new drive
   - **Mac**: Check Finder sidebar
   - **Linux**: Check `/media/` or `/mnt/`

3. **Copy Firmware File**:
   - Open the RAK4631 drive
   - Navigate to your project: `.pio/build/rak4631/`
   - Find `firmware.uf2`
   - **Drag and drop** `firmware.uf2` to the RAK4631 drive
   - Or copy/paste the file

4. **Wait for Upload**:
   - The drive will disconnect automatically
   - Board will reset and run your new firmware
   - This takes about 5 seconds

### Method B: Using PlatformIO Upload (Requires J-Link)

**Note**: This method requires a J-Link programmer (not included with RAK4631)

1. **Connect J-Link**:
   - Connect J-Link programmer to your computer
   - Connect J-Link to RAK4631's SWD pins

2. **Upload**:
   - Click the Upload button (arrow icon) in bottom toolbar
   - Or: PlatformIO sidebar → Project Tasks → rak4631 → General → Upload
   - Or: Press `Ctrl+Alt+U`

3. **Wait for Upload**:
   - PlatformIO will compile (if needed) and upload
   - You'll see "SUCCESS" when done

## Step 9: Monitor Serial Output

### Open Serial Monitor

1. **Click Serial Monitor Icon**:
   - Use the plug icon in bottom toolbar
   - Or: PlatformIO sidebar → Project Tasks → rak4631 → General → Monitor

2. **Select COM Port** (if prompted):
   - Choose the port your RAK4631 is connected to
   - Usually shows as "USB Serial Device" or similar

3. **Check Baud Rate**:
   - Should automatically be 115200 (configured in platformio.ini)
   - If you see garbage, check baud rate setting

### What You Should See

```
===========================================
Meshtastic-Solar Emergency Shutdown System
© SilverTime 2025
Compiled: 2025-12-19 10:30:00
===========================================
Initializing hardware...
Hardware initialized
Button pin (AIN1) initialized
Relay pin (IO2) initialized
CPU speed set to medium (64MHz)
Initializing display...
Display initialized
Boot splash displayed
Initializing time keeper...
TimeKeeper initialized
Initializing whitelist...
Whitelist initialized with 1 node(s)
Initializing messaging...
Messaging system initialized
Setup complete - System ready
```

## Step 10: Test Your System

### Test 1: Boot Sequence

1. Press RESET button on RAK4631
2. Watch serial monitor
3. Should see initialization sequence
4. OLED should show:
   - Meshtastic logo
   - "© SilverTime 2025"
   - Compile date/time
   - Then "System Ready"

### Test 2: Button Test (if you have button)

1. Connect button between AIN1 and GND
2. Press and hold button for >1 second
3. Serial monitor should show:
   ```
   Button pressed (high voltage detected)
   Button press validated (>1 second)
   BUTTON PRESSED!
   === Emergency Activation ===
   ```
4. Relay should activate (if RAK13007 is connected)

### Test 3: Serial Commands

You can test the system through serial monitor:
- Watch for incoming messages
- Check status updates every 10 seconds
- Verify display timeout after 30 seconds

## Troubleshooting

### Build Errors

**Error: "Platform nordicnrf52 not found"**
- **Solution**: Wait for PlatformIO to finish downloading
- Check PlatformIO Home → Platforms → Installed
- Manually install: PlatformIO Home → Platforms → Search "Nordic" → Install

**Error: "Library not found: U8g2"**
- **Solution**: PlatformIO should auto-download
- Manual install: PlatformIO Home → Libraries → Search "U8g2" → Install "U8g2 by oliver"

**Error: "No such file or directory: Wire.h"**
- **Solution**: Framework not fully installed
- Clean build: Bottom toolbar → Clean
- Rebuild: Bottom toolbar → Build

### Upload Errors

**Error: "Device not found"**
- **Solution**:
  - Check USB cable (must support data transfer)
  - Try different USB port
  - Check Device Manager (Windows) or lsusb (Linux)
  - Install RAK4631 USB drivers

**Error: "Access denied" or "Permission denied"**
- **Linux Solution**: `sudo usermod -a -G dialout $USER`
  - Log out and back in
- **Windows Solution**: Run VS Code as Administrator (right-click)

**Error: "Device busy"**
- **Solution**:
  - Close Serial Monitor
  - Close any other program using the COM port
  - Unplug and replug USB cable

### UF2 Bootloader Issues

**RAK4631 drive doesn't appear**
- Double-press RESET button faster
- Try single-press, wait 1 second, single-press again
- Check USB cable
- Try different computer USB port

**Firmware doesn't run after copying UF2**
- Check that you copied the correct file (`firmware.uf2`)
- Check file size (should be >200KB)
- Try rebuilding: Clean → Build → Copy again

### Runtime Issues

**Display shows nothing**
- Check I2C connections
- Verify OLED is working (test with I2C scanner)
- Check serial monitor for display initialization messages

**Button not responding**
- Verify button is connected to AIN1
- Check button provides >2.9V when pressed
- Use multimeter to test button
- Check serial monitor for button readings

**Relay not activating**
- Verify RAK13007 is inserted in IO Slot
- Check relay LED (should light when activated)
- Measure voltage on WB_IO2 (should be 3.3V when ON)

## Next Steps

### Customize Your System

1. **Edit Configuration**:
   - Open `include/config.h`
   - Modify pin assignments, timeouts, etc.
   - Rebuild after changes

2. **Add More Nodes**:
   - System starts with one whitelisted node
   - Add more through web interface (when implemented)
   - Or modify `DEFAULT_WHITELIST_KEY` in config.h

3. **Integrate Meshtastic**:
   - Current implementation has placeholder Meshtastic code
   - See `INTEGRATION.md` for full Meshtastic firmware integration

### Learn More

- **PlatformIO Documentation**: https://docs.platformio.org/
- **RAK4631 Documentation**: https://docs.rakwireless.com/
- **Arduino Reference**: https://www.arduino.cc/reference/
- **U8g2 Graphics Library**: https://github.com/olikraus/u8g2

## Getting Help

### If You're Stuck

1. **Check Serial Monitor**: Most issues show error messages
2. **Read Error Messages**: They usually tell you what's wrong
3. **Search Online**: Copy error message and search
4. **Ask for Help**: Include:
   - Full error message
   - Serial monitor output
   - Steps you've tried
   - Operating system
   - PlatformIO version

### Common Commands Reference

```bash
# Clean build files
pio run -t clean

# Build project
pio run

# Build and upload
pio run -t upload

# Open serial monitor
pio device monitor

# Update platforms and libraries
pio pkg update

# Show environment configuration
pio run -t envdump
```

## Success Checklist

- [ ] VS Code installed
- [ ] PlatformIO extension installed and initialized
- [ ] Project opened in VS Code
- [ ] First build completed successfully
- [ ] RAK4631 connected and recognized
- [ ] Firmware uploaded (UF2 or PlatformIO)
- [ ] Serial monitor showing output
- [ ] Boot splash displayed on OLED
- [ ] System shows "Setup complete - System ready"

**Congratulations!** Your Meshtastic-Solar emergency shutdown system is now running!

---

© SilverTime 2025 | For support, see GitHub Issues
