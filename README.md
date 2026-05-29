# ESP32-S3 N16R8 Custom Board Setup

Guide untuk setup ESP32-S3 DevKitC-1 N16R8V board custom dengan PlatformIO, fokus pada konfigurasi board, test PSRAM, LED, dan penyelesaian masalah serial.

## Hardware

- **Board**: ESP32-S3 DevKitC-1 N16R8V
- **Flash**: 16MB (QD, QIO mode)
- **PSRAM**: 8MB (OPI mode)
- **CPU**: Dual-core Xtensa LX7 @ 240MHz
- **RAM**: 320KB SRAM
- **Connectivity**: WiFi 802.11 b/g/n, Bluetooth 5.0 LE

## Custom Board Definition

File `boards/esp32-s3-devkitc-1-n16r8v.json` mendefinisikan konfigurasi khusus:

```json
{
  "build": {
    "arduino": {
      "ldscript": "esp32s3_out.ld",
      "partitions": "default_16MB.csv",
      "memory_type": "qio_opi"
    },
    "extra_flags": [
      "-DARDUINO_ESP32S3_DEV",
      "-DBOARD_HAS_PSRAM",
      "-DARDUINO_USB_MODE=1",
      "-DARDUINO_USB_CDC_ON_BOOT=0"
    ],
    "f_cpu": "240000000L",
    "f_flash": "80000000L",
    "flash_mode": "qio",
    "psram_type": "opi"
  }
}
```

**Poin penting:**
- `memory_type: "qio_opi"` - Flash QIO, PSRAM OPI
- `ARDUINO_USB_MODE=1` - Menggunakan hardware USB
- `ARDUINO_USB_CDC_ON_BOOT=0` - **USB CDC tidak aktif saat boot** (lihat bagian Troubleshooting)
- `BOARD_HAS_PSRAM` - Mengaktifkan PSRAM

## PlatformIO Configuration

```ini
[env]
platform = espressif32
board = esp32-s3-devkitc-1-n16r8v
framework = arduino
monitor_speed = 9600
```

## Test Environments

### 1. Test PSRAM

Memverifikasi bahwa 8MB PSRAM dapat dialokasikan dan diakses.

```bash
# Build dan upload
pio run -e test_psram -t upload --upload-port COM15

# Monitor serial
pio device monitor -e test_psram --port COM15
```

**Output yang diharapkan:**
```
=== TEST PSRAM ===
PSRAM Size: 8388608 bytes (8 MB)
Allocation test: OK
Read/write test: OK
```

### 2. Test LED Built-in

Menguji LED RGB built-in pada GPIO 48.

```bash
# Build dan upload
pio run -e test_led_builtin -t upload --upload-port COM15

# Monitor serial
pio device monitor -e test_led_builtin --port COM15
```

**Output yang diharapkan:**
```
=== TEST LED BUILTIN ===
LED ON
LED OFF
LED ON
...
```

### 3. Environment Lainnya

| Environment | Deskripsi |
|-------------|-----------|
| `esp32s3` | Environment default (kosong) |
| `test_tft` | Test TFT display (dalam pengembangan) |

## Built-in LED

| Fungsi | GPIO | Keterangan |
|--------|------|------------|
| LED | 48 | RGB LED built-in |

## Serial Communication Issue

### Masalah: Serial Tidak Tampil atau Garbled

**Gejala:**
- Serial monitor tidak menampilkan output program
- Hanya menampilkan karakter garbled seperti `␒()2)!%N` atau symbol aneh
- Output muncul tapi tidak terbaca

**Penyebab:**
ESP32-S3 menggunakan USB CDC untuk komunikasi serial. Konfigurasi berikut:

```json
"-DARDUINO_USB_CDC_ON_BOOT=0"
```

Artinya: **USB CDC tidak aktif saat boot**. Ini menyebabkan:
1. Boot messages tidak terkirim via USB serial
2. Hanya program output setelah inisialisasi USB CDC yang akan muncul
3. Karakter garbled di awal adalah boot ROM messages

**Solusi:**

**Opsi 1: Aktifkan USB CDC on boot**
Ubah di `boards/esp32-s3-devkitc-1-n16r8v.json`:
```json
"-DARDUINO_USB_CDC_ON_BOOT=1"
```

**Opsi 2: Tunggu inisialisasi USB CDC**
Program akan mulai output setelah USB CDC diinisialisasi oleh framework Arduino.

**Opsi 3: Gunakan Serial.begin() dengan benar**
```cpp
void setup() {
  Serial.begin(115200);  // atau 9600 sesuai monitor_speed
  delay(1000);           // Tunggu USB CDC ready
  Serial.println("Program started");
}
```

**Opsi 4: Gunakan hardware UART**
Gunakan pin UART fisik (GPIO 43/44) untuk serial debugging.

### Upload Failed: Port Busy

**Masalah:** `Could not open COM15, the port is busy or doesn't exist`

**Solusi:**
1. Tutup serial monitor (Ctrl+C di terminal monitor)
2. Jalankan upload command
3. Buka monitor kembali setelah upload selesai

## Development

### Struktur Project

```
vortek/
├── boards/
│   └── esp32-s3-devkitc-1-n16r8v.json  # Custom board definition
├── src/
│   ├── main.cpp                         # Main application
│   └── config.h                         # Pin configuration
├── test/
│   ├── test_psram.cpp                   # PSRAM test
│   ├── test_tft.cpp                     # TFT test (dalam pengembangan)
│   └── test_led_builtin.cpp             # LED blink test
├── lib/
│   └── TFT_eSPI/                        # TFT library (untuk pengembangan)
└── platformio.ini                       # PlatformIO configuration
```

### Menambah Environment Baru

Tambah di `platformio.ini`:
```ini
[env:nama_env]
build_src_filter =
    +<../test/nama_test.cpp>
build_flags =
    -DFLAG_BARU=1
```

### Custom Build Flags

```ini
[env:custom]
build_flags =
    -DMY_FLAG=1
    -DDEBUG_LEVEL=5
    -DCORE_DEBUG_LEVEL=5
```

## Troubleshooting

### 1. Serial Output Kosong

**Cek:**
1. `ARDUINO_USB_CDC_ON_BOOT` setting
2. `Serial.begin()` sudah dipanggil di `setup()`
3. Delay 1-2 detik setelah Serial.begin() untuk USB CDC ready
4. Monitor speed sesuai dengan `monitor_speed` di platformio.ini

### 2. PSRAM Tidak Terdeteksi

**Cek:**
1. Board确实是 N16R8V variant (ada PSRAM)
2. `boards/esp32-s3-devkitc-1-n16r8v.json` punya `"psram_type": "opi"`
3. Build flag `-DBOARD_HAS_PSRAM` ada
4. Test dengan program minimal PSRAM allocation

### 3. Upload Gagal

**Solusi:**
1. Pastikan port COM benar
2. Tutup semua program yang menggunakan port (termasuk serial monitor)
3. Coba reset board manual sebelum upload
4. Periksa driver USB (CP210x atau CH340)

### 4. Program Crash

**Cek:**
1. Stack size cukup (untuk WiFi/PSRAM operations)
2. Memory allocation tidak berlebihan
3. Watchdog timer tidak timeout
4. Gunakan `esp_task_wdt_feed()` jika perlu

## Resources

- [ESP32-S3 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf)
- [ESP32-S3 USB CDC](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/api-guides/usb-serial-jtag.html)
- [PlatformIO ESP32](https://docs.platformio.org/en/latest/platforms/espressif32.html)
- [Arduino ESP32 Core](https://github.com/espressif/arduino-esp32)

## License

Project ini disediakan untuk tujuan edukasi dan pengembangan.

---

**Catatan:** 
- LCD/TFT display sedang dalam pengembangan, belum stable
- Serial output mungkin perlu penyesuaian tergantung konfigurasi USB CDC
- Pastikan board确实是 ESP32-S3 N16R8V sebelum menggunakan konfigurasi PSRAM