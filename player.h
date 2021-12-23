// SPI3 https://techoverflow.net/2021/07/26/what-is-the-spi-pinout-of-the-esp32-esp-wroom-32/
#define CLK 18       // SPI Clock, shared with SD card
#define MISO 19      // Input data, from VS1053/SD card
#define MOSI 23      // Output data, to VS1053/SD card

#define BREAKOUT_RESET  9     // VS1053 reset pin (output)
#define BREAKOUT_CS     5     // VS1053 chip select pin (output)
#define BREAKOUT_DCS    21    // VS1053 Data/command select pin (output)

#define CARDCS 17     // Card chip select pin
// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
#define DREQ 26       // VS1053 Data request, ideally an Interrupt pin

Adafruit_VS1053_FilePlayer player = Adafruit_VS1053_FilePlayer(BREAKOUT_RESET, BREAKOUT_CS, BREAKOUT_DCS, DREQ, CARDCS);

unsigned char bt_wav_header[44] = {
    0x52, 0x49, 0x46, 0x46, // RIFF
    0xFF, 0xFF, 0xFF, 0xFF, // size
    0x57, 0x41, 0x56, 0x45, // WAVE
    0x66, 0x6d, 0x74, 0x20, // fmt
    0x10, 0x00, 0x00, 0x00, // subchunk1size
    0x01, 0x00,             // audio format - pcm
    0x02, 0x00,             // numof channels
    0x44, 0xac, 0x00, 0x00, //, samplerate 44k1: 0x44, 0xac, 0x00, 0x00       48k: 48000: 0x80, 0xbb, 0x00, 0x00,
    0x10, 0xb1, 0x02, 0x00, //byterate
    0x04, 0x00,             // blockalign
    0x10, 0x00,             // bits per sample - 16
    0x64, 0x61, 0x74, 0x61, // subchunk3id -"data"
    0xFF, 0xFF, 0xFF, 0xFF  // subchunk3size (endless)
};
