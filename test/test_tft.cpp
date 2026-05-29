/**
 * TEST: TFT Display (tanpa touchscreen) + SD Card SPI sharing
 *
 * Cek: TFT display saja, tapi SD Card di-init dulu supaya sharing SPI bus aman
 * - Init SD Card pada SPI bus yang sama
 * - Disable SD Card sebelum init TFT
 * - Tampilkan warna hitam, merah, hijau, biru, putih
 * - Tampilkan teks dan grafik sederhana
 *
 * Pin SPI shared (Goouuu ESP32-S3 Expansion Board):
 * - MISO = 46
 * - MOSI = 45
 * - SCLK = 3
 *
 * Pin device:
 * - SD_CS    = 5
 * - TFT_CS   = 14
 * - TFT_DC   = 47
 * - TFT_RST  = 21
 *
 * Driver: ILI9341 (320x240)
 */

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <TFT_eSPI.h>
#include "../src/config.h"

TFT_eSPI tft;

void drawCornerMarkers()
{
    int w = tft.width();
    int h = tft.height();
    tft.fillTriangle(0, 0, 30, 0, 0, 30, TFT_RED);
    tft.fillTriangle(w - 1, 0, w - 31, 0, w - 1, 30, TFT_GREEN);
    tft.fillTriangle(0, h - 1, 30, h - 1, 0, h - 31, TFT_BLUE);
    tft.fillTriangle(w - 1, h - 1, w - 31, h - 1, w - 1, h - 31, TFT_YELLOW);
}

void testOrientation(uint8_t rotation, const char *label, const char *usbPosition, uint16_t bgColor, uint16_t textColor)
{
    Serial.printf("\nOrientation test: rotation %u (%s)\n", rotation, label);

    digitalWrite(SD_CS, HIGH);
    tft.setRotation(rotation);
    int w = tft.width();
    int h = tft.height();

    tft.fillScreen(bgColor);

    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(textColor, bgColor);
    tft.drawString(label, w / 2, h / 2 - 30, 4);

    char dimBuf[16];
    snprintf(dimBuf, sizeof(dimBuf), "%d x %d", w, h);
    tft.drawString(dimBuf, w / 2, h / 2 + 5, 2);
    tft.drawString(usbPosition, w / 2, h / 2 + 30, 2);

    tft.setTextDatum(TL_DATUM);
    tft.drawString("TL", 35, 8, 2);
    tft.setTextDatum(TR_DATUM);
    tft.drawString("TR", w - 36, 8, 2);
    tft.setTextDatum(BL_DATUM);
    tft.drawString("BL", 35, h - 8, 2);
    tft.setTextDatum(BR_DATUM);
    tft.drawString("BR", w - 36, h - 8, 2);

    drawCornerMarkers();
    delay(3000);
}

void testLandscapeGraphics()
{
    Serial.println("\nLandscape graphics test");

    digitalWrite(SD_CS, HIGH);
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);

    tft.fillRect(0, 0, 320, 30, TFT_DARKGREY);
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
    tft.drawString("LANDSCAPE DISPLAY TEST", 160, 15, 2);

    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.drawString("Progress bar 300px wide", 160, 45, 2);
    tft.drawRect(10, 62, 300, 22, TFT_WHITE);
    for (int width = 0; width <= 296; width += 8)
    {
        tft.fillRect(12, 64, width, 18, TFT_GREEN);
        delay(20);
    }

    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.drawString("Landscape spectrum area", 160, 105, 2);
    const int barWidth = 16;
    const int barGap = 4;
    const int baseY = 205;
    for (int i = 0; i < 15; i++)
    {
        int height = 25 + ((i * 37) % 85);
        uint16_t color = tft.color565(40 + (i * 13) % 215, 255 - (i * 11) % 180, 80 + (i * 17) % 175);
        int x = 12 + i * (barWidth + barGap);
        tft.fillRect(x, baseY - height, barWidth, height, color);
        tft.drawRect(x, 115, barWidth, 90, TFT_DARKGREY);
    }

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("OK: display landscape penuh", 160, 225, 2);
    delay(3000);
}

void testAllOrientations()
{
    testOrientation(0, "PORTRAIT 0", "USB: BOTTOM", TFT_DARKGREEN, TFT_WHITE);
    testOrientation(1, "LANDSCAPE 1", "USB: LEFT", TFT_NAVY, TFT_YELLOW);
    testOrientation(2, "PORTRAIT 2 (REV)", "USB: TOP", TFT_PURPLE, TFT_WHITE);
    testOrientation(3, "LANDSCAPE 3 (REV)", "USB: RIGHT", TFT_MAROON, TFT_CYAN);
    testLandscapeGraphics();

    tft.setRotation(1);
}

void setup()
{
    Serial.begin(9600);
    delay(1000);
    Serial.println("\n=== TEST TFT DISPLAY + SD SPI SHARING ===");
    Serial.println("Shared SPI (Goouuu ESP32-S3 Expansion Board):");
    Serial.printf("  MISO = %d\n", TFT_MISO);
    Serial.printf("  MOSI = %d\n", TFT_MOSI);
    Serial.printf("  SCLK = %d\n", TFT_SCLK);
    Serial.println("Device CS:");
    Serial.printf("  SD_CS  = %d\n", SD_CS);
    Serial.printf("  TFT_CS = %d\n", TFT_CS);
    Serial.println("TFT control:");
    Serial.printf("  DC   = %d\n", TFT_DC);
    Serial.printf("  RST  = %d\n", TFT_RST);

    // Pastikan semua device SPI nonaktif dulu
    pinMode(SD_CS, OUTPUT);
    pinMode(TFT_CS, OUTPUT);
    digitalWrite(SD_CS, HIGH);  // Disable SD Card
    digitalWrite(TFT_CS, HIGH); // Disable TFT
    delay(100);

    // Skip SD Card init - fokus test TFT dulu
    Serial.println("\nSkip SD Card init - fokus TFT dulu");

    // Init TFT
    Serial.println("\nInit TFT...");
    tft.init();
    delay(200);

    Serial.println("TFT init OK!");
    tft.setRotation(1); // Landscape 320x240

    // Test basic display
    Serial.println("\nTest basic colors...");
    tft.fillScreen(TFT_RED);
    delay(1000);
    tft.fillScreen(TFT_GREEN);
    delay(1000);
    tft.fillScreen(TFT_BLUE);
    delay(1000);

    Serial.println("\nTFT basic test OK! Lanjut full test...\n");

    // Now try SD Card
    Serial.println("Init SD Card...");
    if (SD.begin(SD_CS))
    {
        Serial.println("SD Card detected");

        uint8_t cardType = SD.cardType();
        Serial.print("SD Card Type: ");
        if (cardType == CARD_MMC)
        {
            Serial.println("MMC");
        }
        else if (cardType == CARD_SD)
        {
            Serial.println("SDSC");
        }
        else if (cardType == CARD_SDHC)
        {
            Serial.println("SDHC");
        }
        else
        {
            Serial.println("UNKNOWN");
        }

        uint64_t cardSize = SD.cardSize() / (1024 * 1024);
        Serial.printf("SD Size: %lluMB\n", cardSize);
    }
    else
    {
        Serial.println("SD Card failed / not inserted");
        Serial.println("Lanjut test TFT saja");
    }

    // Test 1: RED (TFT sudah di-init di atas)
    Serial.println("\nTest 1: RED");
    tft.fillScreen(TFT_RED);
    tft.setTextColor(TFT_WHITE, TFT_RED);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("RED", 160, 120, 4);
    delay(2000);

    // Test 2: GREEN
    Serial.println("Test 2: GREEN");
    tft.fillScreen(TFT_GREEN);
    tft.setTextColor(TFT_BLACK, TFT_GREEN);
    tft.drawString("GREEN", 160, 120, 4);
    delay(2000);

    // Test 3: BLUE
    Serial.println("Test 3: BLUE");
    tft.fillScreen(TFT_BLUE);
    tft.setTextColor(TFT_WHITE, TFT_BLUE);
    tft.drawString("BLUE", 160, 120, 4);
    delay(2000);

    // Test 4: WHITE
    Serial.println("Test 4: WHITE");
    tft.fillScreen(TFT_WHITE);
    tft.setTextColor(TFT_BLACK, TFT_WHITE);
    tft.drawString("WHITE", 160, 120, 4);
    delay(2000);

    // Test 5: BLACK with graphics
    Serial.println("Test 5: BLACK + Graphics");
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("TFT TEST OK", 160, 60, 4);

    // Draw rectangles
    tft.drawRect(20, 100, 60, 40, TFT_RED);
    tft.fillRect(25, 105, 50, 30, TFT_RED);

    tft.drawRect(100, 100, 60, 40, TFT_GREEN);
    tft.fillRect(105, 105, 50, 30, TFT_GREEN);

    tft.drawRect(180, 100, 60, 40, TFT_BLUE);
    tft.fillRect(185, 105, 50, 30, TFT_BLUE);

    // Draw circles
    tft.drawCircle(50, 180, 20, TFT_YELLOW);
    tft.fillCircle(160, 180, 20, TFT_MAGENTA);
    tft.drawCircle(270, 180, 20, TFT_CYAN);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Display: 320x240", 160, 220, 2);

    Serial.println("\nTest semua orientasi (portrait + landscape + reverse)...");
    delay(1000);
    testAllOrientations();

    Serial.println("\nTest selesai. Display menampilkan semua orientasi dan grafik.");
}

void loop()
{
    // Blink text
    static unsigned long lastBlink = 0;
    static bool textVisible = true;

    if (millis() - lastBlink > 500)
    {
        lastBlink = millis();
        textVisible = !textVisible;

        // Pastikan SD tidak aktif saat update TFT
        digitalWrite(SD_CS, HIGH);

        if (textVisible)
        {
            tft.setTextColor(TFT_GREEN, TFT_BLACK);
            tft.setTextDatum(TL_DATUM);
            tft.drawString("OK", 5, 5, 2);
        }
        else
        {
            tft.fillRect(5, 5, 30, 20, TFT_BLACK);
        }
    }
}