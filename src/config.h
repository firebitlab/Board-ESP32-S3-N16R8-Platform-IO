#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ==========================================
// ESP32 PIN DEFINITIONS
// ==========================================

// I2S Audio (PCM5102 DAC)
#define I2S_BCLK 26
#define I2S_LRCLK 25
#define I2S_DIN 22

// Alias untuk backward compatibility
#define I2S_LRC I2S_LRCLK
#define I2S_DOUT I2S_DIN

// Rotary Encoder
#define ENC_CLK 32
#define ENC_DT 33
#define ENC_SW 27

// SD Card (SPI)
#define SD_CS 5

// TFT LCD & Touch (SPI) - Goouuu ESP32-S3 Expansion Board
#define TFT_CS 14
#define TFT_DC 47
#ifndef TFT_RST
#define TFT_RST 21
#endif
#define TFT_MISO 46
#define TFT_MOSI 45
#define TFT_SCLK 3
#define TOUCH_CS 1

// SPI alias untuk test compatibility
#define SPI_MISO TFT_MISO
#define SPI_MOSI TFT_MOSI
#define SPI_SCLK TFT_SCLK

// ==========================================
// TIMING CONFIG
// ==========================================

#define SD_CACHE_LIFETIME 60000 // ms
#define TOAST_DURATION 2000
#define VOLUME_OVERLAY_DURATION 1500
#define BUTTON_DEBOUNCE 40 // ms, state-stable debounce time on both edges (press + release)
#define LONG_PRESS_DURATION 1000
#define DOUBLE_CLICK_WINDOW 250 // ms, max gap between 2 clicks for double-tap

// ==========================================
// AUDIO CONFIG
// ==========================================

#define MAX_VOLUME 10
#define DEFAULT_VOLUME 7
#define MAX_EQ_VALUE 6   // dB
#define MIN_EQ_VALUE -40 // dB

// Audio stability - I2S DMA buffer
#define I2S_BUFFER_SIZE 512
#define I2S_DMA_COUNT 16
#define SD_BLOCK_SIZE 512

// Volume normalization (SD vs BT)
#define BT_VOL_MULTIPLIER 12

// ==========================================
// SD CARD CONFIG
// ==========================================

#define MAX_FILES 30
#define FILES_PER_PAGE 5

// ==========================================
// DISPLAY CONFIG
// ==========================================

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

// Visualizer config
#define VISUALIZER_FPS 20
#define VISUALIZER_INTERVAL_MS (1000 / VISUALIZER_FPS)
#define NUM_BARS 16
#define SAMPLES 256
#define SAMPLING_FREQUENCY 44100
#define NUM_PRESETS 5

// Visualizer modes
#define VIZ_MODE_SPECTRUM 0
#define VIZ_MODE_WAVEFORM 1
#define VIZ_MODE_OSCILLOSCOPE 2
#define VIZ_MODE_FILLED 3
#define NUM_VIZ_MODES 4

// Button areas (touch)
#define EQ_BUTTON_X 5
#define EQ_BUTTON_Y 5
#define EQ_BUTTON_W 30
#define EQ_BUTTON_H 20

// Colors
#define COLOR_BACKGROUND TFT_BLACK
#define COLOR_TEXT TFT_WHITE
#define COLOR_ACCENT TFT_CYAN
#define COLOR_WARNING TFT_YELLOW
#define COLOR_ERROR TFT_RED
#define COLOR_SUCCESS TFT_GREEN

// ==========================================
// BLUETOOTH CONFIG
// ==========================================

#define BT_DEVICE_NAME "AudioTherapy"
#define BT_RECONNECT_INTERVAL 5000 // ms
#define BT_MAX_RECONNECT_RETRIES 10

// ==========================================
// ENCODER CONFIG
// ==========================================

#define ENCODER_STEPS_PER_DETENT 2 // depends on encoder type

// ==========================================
// PREFERENCES CONFIG
// ==========================================

#define PREF_NAMESPACE "audiotherapy"
#define PREF_VOLUME "volume"
#define PREF_VIZ_PRESET "vizPreset"
#define PREF_VIZ_MODE "vizMode"
#define PREF_BASS "bass"
#define PREF_TREBLE "treble"
#define PREF_EQ_PRESET "eqPreset"

// ==========================================
// STATE DEFINITIONS
// ==========================================

enum SystemState
{
    STATE_BOOT = 0,
    STATE_NO_SD_INFO,
    STATE_HOME,
    STATE_SD_LIST,
    STATE_SD_PLAYER,
    STATE_BT_WAITING,
    STATE_BT_PLAYER,
    STATE_VIZ_MODE,
    STATE_VIZ_PRESET,
    STATE_EQ_PRESET,
    STATE_EQ_CUSTOM,
    STATE_SETTINGS,
    STATE_QUICK_MENU, // Quick player chooser during playback
    STATE_QUICK_EQ,   // Quick EQ overlay during playback (live preview)
    STATE_QUICK_VIZ   // Quick visualizer mode overlay during playback
};

enum ButtonEvent
{
    BTN_NONE = 0,
    BTN_SHORT,
    BTN_LONG,
    BTN_DOUBLE
};

enum EncoderEvent
{
    ENC_NONE = 0,
    ENC_CW,
    ENC_CCW
};

// Legacy compatibility
enum SystemMode
{
    MODE_HOME = -1,
    MODE_BLUETOOTH = 0,
    MODE_SD_CARD = 1
};

enum SDState
{
    SD_NOT_INITIALIZED = 0,
    SD_OK = 1,
    SD_ERROR = 2
};

// ==========================================
// GLOBAL VARIABLES (extern declarations)
// ==========================================

// System state
extern SystemState currentState;
extern bool sdCardAvailable;
extern int currentMode;
extern int sdState;

// Volume & EQ
extern int savedVolume;
extern int gainLowCount;  // Bass
extern int gainMidCount;  // Mid
extern int gainHighCount; // Treble
extern int savedBass;
extern int savedTreble;

// Playback state
extern bool isPlaying;
extern int equalizerSetting;
extern bool equalizerMode;

// Bluetooth state
extern bool btConnected;
extern unsigned long btDisconnectTime;
extern int btReconnectRetries;

// SD card
extern String fileList[MAX_FILES];
extern int fileCount;
extern int currentFileIndex;
extern int currentPage;

// Progress tracking
extern int lastCurrentTime;
extern int lastTotalTime;
extern unsigned long lastVisualizerUpdate;
#endif // CONFIG_H
