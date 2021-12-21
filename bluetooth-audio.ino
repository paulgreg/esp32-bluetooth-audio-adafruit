#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>
#include "BluetoothA2DPSink.h"
#include <cbuf.h>


// SPI3 https://techoverflow.net/2021/07/26/what-is-the-spi-pinout-of-the-esp32-esp-wroom-32/
#define CLK 18       // SPI Clock, shared with SD card
#define MISO 19      // Input data, from VS1053/SD card
#define MOSI 23      // Output data, to VS1053/SD card

#define BREAKOUT_RESET  9     // VS1053 reset pin (output)
#define BREAKOUT_CS     5     // VS1053 chip select pin (output)
#define BREAKOUT_DCS    21    // VS1053 Data/command select pin (output)
// These are the pins used for the music maker shield
#define SHIELD_RESET  -1      // VS1053 reset pin (unused!)
#define SHIELD_CS     7       // VS1053 chip select pin (output)
#define SHIELD_DCS    6       // VS1053 Data/command select pin (output)

// These are common pins between breakout and shield
#define CARDCS 17     // Card chip select pin
// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
#define DREQ 26       // VS1053 Data request, ideally an Interrupt pin

Adafruit_VS1053_FilePlayer player = Adafruit_VS1053_FilePlayer(BREAKOUT_RESET, BREAKOUT_CS, BREAKOUT_DCS, DREQ, CARDCS);

cbuf circBuffer(1024 * 24); //64);
char readBuffer[1000] __attribute__((aligned(4)));

uint8_t mp3buff[32]; // vs1053 likes 32 bytes at a time


BluetoothA2DPSink a2dp_sink;

unsigned char bt_wav_header[44] = {
    0x52, 0x49, 0x46, 0x46, // RIFF
    0xFF, 0xFF, 0xFF, 0xFF, // size
    0x57, 0x41, 0x56, 0x45, // WAVE
    0x66, 0x6d, 0x74, 0x20, // fmt
    0x10, 0x00, 0x00, 0x00, // subchunk1size
    0x01, 0x00,             // audio format - pcm
    0x02, 0x00,             // numof channels
    0x80, 0xbb, 0x00, 0x00, //, //samplerate 44k1: 0x44, 0xac, 0x00, 0x00       48k: 48000: 0x80, 0xbb, 0x00, 0x00,
    0x10, 0xb1, 0x02, 0x00, //byterate
    0x04, 0x00,             // blockalign
    0x10, 0x00,             // bits per sample - 16
    0x64, 0x61, 0x74, 0x61, // subchunk3id -"data"
    0xFF, 0xFF, 0xFF, 0xFF  // subchunk3size (endless)
};


bool f_bluetoothsink_metadata_received = false;

char bluetooth_media_title[255];

int cnt = 0;

void bluetoothsink_avrc_metadata_callback(uint8_t data1, const uint8_t *data2) {

    Serial.printf("AVRC metadata rsp: attribute id 0x%x, %s\n", data1, data2);

    if (data1 == 0x1) {
        // Title
        strncpy(bluetooth_media_title, (char *)data2, sizeof(bluetooth_media_title) - 1);
    }
    else if (data1 == 0x2) {
        strncat(bluetooth_media_title, " - ", sizeof(bluetooth_media_title) - 1);
        strncat(bluetooth_media_title, (char *)data2, sizeof(bluetooth_media_title) - 1);
        f_bluetoothsink_metadata_received = true;
    }
}

void bluetoothsink_handle_stream() {
    if (circBuffer.available())
    {
        // Does the VS1053 want any more data (yet)?
        if (player.readyForData()) {
            int bytesRead = circBuffer.read((char *)mp3buff, 32);

            // If we didn't read the full 32 bytes, that's a worry
            if (bytesRead != 32) {
                Serial.printf("Only read %d bytes from  circular buffer\n", bytesRead);
            }

            // Actually send the data to the VS1053
            player.playData(mp3buff, bytesRead);
        }
    }
}

void bluetoothsink_read_data_stream(const uint8_t *data, uint32_t length) {
    if (circBuffer.room() > length) {
        // If we get -1 here it means nothing could be read from the stream
        if (length > 0) {
            // Add them to the circular buffer
            circBuffer.write((char *)data, length); // length seems to be 4096 every time
            //Serial.printf("\nRead %lu bytes", length);
        }
    }
}

void bluetoothsink_setup() {
    a2dp_sink.set_stream_reader(bluetoothsink_read_data_stream, false);
    a2dp_sink.set_avrc_metadata_callback(bluetoothsink_avrc_metadata_callback);
}

void bluetoothsink_start() {
    a2dp_sink.start("MyAirSpeaker");

    circBuffer.flush();

    delay(100);
    circBuffer.write((char *)bt_wav_header, 44);
    delay(100);
}

void bluetoothsink_end() {
    a2dp_sink.end(false);
}

void bluetoothsink_next() {
    a2dp_sink.next();
}

void bluetoothsink_previous() {
    a2dp_sink.previous();
}


void setup() {
  Serial.begin(115200);
  Serial.println("Bluetooth Music Player");

  if (!player.begin()) {
     Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
     while (1);
  }
  Serial.println(F("VS1053 found"));
  
  player.setVolume(10,10); // Set volume for left, right channels. lower numbers == louder volume!

  // Timer interrupts are not suggested, better to use DREQ interrupt!
  player.useInterrupt(VS1053_FILEPLAYER_TIMER0_INT); // timer int

  // If DREQ is on an interrupt pin (on uno, #2 or #3) we can do background audio playing
  //player.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int 

  bluetoothsink_setup();
  bluetoothsink_start();
}

void loop() {
  bluetoothsink_handle_stream();
}
