# ESP32-S3 N16R8 PlatformIO Setup Guide

Complete guide for setting up ESP32-S3 DevKitC-1 N16R8V board with PlatformIO, including PSRAM configuration and peripheral testing.

## Hardware Specifications

- **Board**: ESP32-S3 DevKitC-1 N16R8V
- **Flash**: 16MB (QD, QIO mode)
- **PSRAM**: 8MB (OPI mode)
- **CPU**: Dual-core Xtensa LX7 @ 240MHz
- **RAM**: 320KB SRAM
- **Connectivity**: WiFi 802.11 b/g/n, Bluetooth 5.0 LE

### Peripherals
- **LCD**: ILI9341 2.8" TFT SPI (240x320 pixels)
- **SD Card**: SPI interface (shared with TFT)

## Prerequisites

- [PlatformIO Core](https://platformio.org/install/cli) or [VS Code + PlatformIO Extension](https://platformio.org/install/ide?install=vscode)
- USB-C cable for programming and power
- CP210x or CH340 USB-to-Serial driver (if needed)

## Quick Start

### 1. Clone Repository

```bash
git clone <repository-url>
cd vortek
```

### 2. Project Structure

```
vortek/
├── boards/
│   └── esp32-s3-devkitc-1-n16r8v.json  # Custom board definition
├── src/
│   ├── main.cpp                         # Main application
│   └── config.h                         # Pin configuration
├── test/
│   ├── test_psram.cpp                   # PSRAM test
│   ├── test_tft.cpp                     # TFT LCD test
│   └── test_led_builtin.cpp             # LED blink test
├── lib/
│   └── TFT_eSPI/                        # TFT display library
└── platformio.ini                       # PlatformIO configuration
```

### 3. Board Configuration

The custom board definition (`boards/esp32-s3-devkitc-1-n16r8v.json`) configures:
- 16MB Flash with QIO mode
- 8MB PSRAM with OPI mode
- USB CDC for serial communication
- 240MHz CPU frequency

Key settings in `platformio.ini`:
```ini
[env]
platform = espressif32
board = esp32-s3-devkitc-1-n16r8v
framework = arduino
monitor_speed = 9600
```

### 4. Build and Upload

```bash
# Build default environment
pio run

# Upload to board (replace COM15 with your port)
pio run -t upload --upload-port COM15

# Open serial monitor
pio device monitor --port COM15
```

## Test Environments

The project includes three test environments to verify hardware functionality:

| Environment | Description | Command |
|-------------|-------------|---------|
| `test_psram` | Tests 8MB PSRAM allocation and access | `pio run -e test_psram -t upload` |
| `test_tft` | Tests ILI9341 TFT display and SD card | `pio run -e test_tft -t upload` |
| `test_led_builtin` | Blinks built-in LED | `pio run -e test_led_builtin -t upload` |

### Running Tests

```bash
# Test PSRAM
pio run -e test_psram -t upload --upload-port COM15
pio device monitor -e test_psram --port COM15

# Test TFT Display
pio run -e test_tft -t upload --upload-port COM15
pio device monitor -e test_tft --port COM15

# Test LED
pio run -e test_led_builtin -t upload --upload-port COM15
pio device monitor -e test_led_builtin --port COM15
```

## Pin Configuration

### SPI Bus (Shared: TFT + SD Card)
| Function | GPIO | Description |
|----------|------|-------------|
| MISO | 46 | Master In Slave Out |
| MOSI | 45 | Master Out Slave In |
| SCLK | 3 | SPI Clock |

### TFT Display (ILI9341)
| Function | GPIO | Description |
|----------|------|-------------|
| CS | 14 | Chip Select |
| DC | 47 | Data/Command |
| RST | 21 | Reset |
| BL | - | Backlight (always on) |

### SD Card
| Function | GPIO | Description |
|----------|------|-------------|
| CS | 5 | Chip Select |

### Built-in LED
| Function | GPIO | Description |
|----------|------|-------------|
| LED | 48 | Built-in RGB LED |

## Troubleshooting

### Serial Output Shows Garbled Text

**Problem**: Serial monitor displays garbled characters like `␒()2)!%N`

**Solution**: ESP32-S3 uses USB CDC for serial communication by default. The garbled output is normal boot messages. Your actual program output will be readable after initialization.

**Board Configuration**:
```json
"extra_flags": [
    "-DARDUINO_USB_MODE=1",
    "-DARDUINO_USB_CDC_ON_BOOT=0"
]
```

### Upload Failed: Port Busy

**Problem**: `Could not open COM15, the port is busy or doesn't exist`

**Solution**: Close the serial monitor before uploading:
1. Press `Ctrl+C` in the monitor terminal
2. Run upload command
3. Restart monitor after upload completes

### PSRAM Not Detected

**Problem**: PSRAM test fails or shows 0 bytes

**Solution**: Verify board configuration:
1. Check `boards/esp32-s3-devkitc-1-n16r8v.json` has `"psram_type": "opi"`
2. Ensure build flag `-DBOARD_HAS_PSRAM` is set
3. Confirm your board actually has PSRAM (N16R8V variant)

### TFT Display Not Working

**Problem**: Display shows nothing or crashes with `Guru Meditation Error`

**Solution**: 
1. Verify pin connections match `src/config.h`
2. Check SPI bus initialization
3. Confirm LCD is ILI9341 compatible
4. Test with minimal example first

**Current Status**: TFT test crashes at `tft.init()` - pin mapping verification needed.

## Development

### Adding New Features

1. Edit `src/main.cpp` for main application
2. Use `src/config.h` for pin definitions
3. Add dependencies in `platformio.ini` under `lib_deps`

### Custom Build Flags

Add build flags in `platformio.ini`:
```ini
[env:custom]
build_flags =
    -DMY_CUSTOM_FLAG=1
    -DDEBUG_LEVEL=5
```

## Resources

- [ESP32-S3 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf)
- [ESP32-S3 DevKitC-1 User Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/hw-reference/esp32s3/user-guide-devkitc-1.html)
- [PlatformIO ESP32 Platform](https://docs.platformio.org/en/latest/platforms/espressif32.html)
- [TFT_eSPI Library](https://github.com/Bodmer/TFT_eSPI)

## License

This project is provided as-is for educational and development purposes.

## Contributing

Contributions are welcome! Please:
1. Fork the repository
2. Create a feature branch
3. Test your changes thoroughly
4. Submit a pull request with clear description

---

**Note**: This is a development project. Some features (TFT display) are still under debugging. See Troubleshooting section for known issues.