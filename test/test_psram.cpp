/**
 * - Info chip: flash, heap, PSRAM
 * - Alokasi buffer besar via ps_malloc()
 * - Write pattern + verify
 */

#include <Arduino.h>

void setup()
{
    Serial.begin(9600);
    delay(1000);
    Serial.println("\n=== TEST PSRAM===");

    Serial.println("\n--- Chip Info ---");
    Serial.printf("ESP32 SDK: %s\n", ESP.getSdkVersion());
    Serial.printf("Chip model: %s Rev %d\n",
                  ESP.getChipModel(), ESP.getChipRevision());
    Serial.printf("CPU freq: %d MHz\n", ESP.getCpuFreqMHz());

    Serial.println("\n--- Flash ---");
    Serial.printf("Flash size: %u bytes (%u MB)\n",
                  ESP.getFlashChipSize(), ESP.getFlashChipSize() / (1024 * 1024));

    Serial.println("\n--- PSRAM ---");
    if (psramFound())
    {
        Serial.println("PSRAM: DETECTED");
        Serial.printf("PSRAM size: %u bytes (%u MB)\n",
                      ESP.getPsramSize(), ESP.getPsramSize() / (1024 * 1024));
        Serial.printf("PSRAM free: %u bytes\n", ESP.getFreePsram());
    }
    else
    {
        Serial.println("PSRAM: NOT FOUND");
    }

    Serial.println("\n--- Heap ---");
    Serial.printf("Heap total: %u bytes\n", ESP.getHeapSize());
    Serial.printf("Heap free: %u bytes\n", ESP.getFreeHeap());
    Serial.printf("Heap min free: %u bytes\n", ESP.getMinFreeHeap());

    Serial.println("\n--- PSRAM Alloc Test ---");
    if (psramFound())
    {
        size_t allocSizes[] = {1024, 102400, 512000, 1048576, 2097152};
        const char *labels[] = {"1KB", "100KB", "500KB", "1MB", "2MB"};

        for (int i = 0; i < 5; i++)
        {
            Serial.printf("Alokasi %s di PSRAM... ", labels[i]);
            uint8_t *buf = (uint8_t *)ps_malloc(allocSizes[i]);
            if (buf == NULL)
            {
                Serial.printf("GAGAL (free PSRAM: %u)\n", ESP.getFreePsram());
                continue;
            }

            // Write pattern
            for (size_t j = 0; j < allocSizes[i]; j++)
                buf[j] = (uint8_t)(j & 0xFF);

            // Verify
            bool ok = true;
            for (size_t j = 0; j < allocSizes[i]; j++)
            {
                if (buf[j] != (uint8_t)(j & 0xFF))
                {
                    ok = false;
                    break;
                }
            }

            if (ok)
            {
                Serial.printf("OK (free PSRAM: %u)\n", ESP.getFreePsram());
            }
            else
            {
                Serial.printf("VERIFY FAILED (free PSRAM: %u)\n", ESP.getFreePsram());
            }

            free(buf);
        }
    }
    else
    {
        // Fallback: alokasi heap biasa
        Serial.println("PSRAM tidak ada, test heap internal:");
        size_t allocSizes[] = {1024, 102400, 512000};
        const char *labels[] = {"1KB", "100KB", "500KB"};

        for (int i = 0; i < 3; i++)
        {
            Serial.printf("Alokasi %s... ", labels[i]);
            uint8_t *buf = (uint8_t *)malloc(allocSizes[i]);
            if (buf == NULL)
            {
                Serial.printf("GAGAL (free heap: %u)\n", ESP.getFreeHeap());
                continue;
            }

            for (size_t j = 0; j < allocSizes[i]; j++)
                buf[j] = (uint8_t)(j & 0xFF);

            bool ok = true;
            for (size_t j = 0; j < allocSizes[i]; j++)
            {
                if (buf[j] != (uint8_t)(j & 0xFF))
                {
                    ok = false;
                    break;
                }
            }

            if (ok)
                Serial.printf("OK (free heap: %u)\n", ESP.getFreeHeap());
            else
                Serial.printf("VERIFY FAILED\n");

            free(buf);
        }
    }

    Serial.println("\n=== TEST PSRAM SELESAI ===");
}

void loop()
{
    delay(1000);
}