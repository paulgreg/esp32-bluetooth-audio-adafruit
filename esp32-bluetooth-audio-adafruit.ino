#include "parameters.h"

#include <SPI.h>
#include <Adafruit_VS1053.h>
#include "player.h"

#include "BluetoothA2DPSink.h"
#include <cbuf.h>
#include "bluetoothsink.h"

BluetoothA2DPSink a2dp_sink;

void setup() {
  Serial.begin(115200);
  Serial.println(BLUETOOTH_NAME);

  if (!player.begin()) {
     Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
     while (1);
  } 
  
  player.begin();
  player.setVolume(10,10); // Set volume for left, right channels. lower numbers == louder volume!

  circBuffer.flush();

  a2dp_sink.set_stream_reader(read_data_stream, false);
  a2dp_sink.set_avrc_metadata_callback(avrc_metadata_callback);
  a2dp_sink.start(BLUETOOTH_NAME);
  delay(100);
  circBuffer.write((char *)bt_wav_header, 44);
  delay(100);
}

void loop() {
  handle_stream();
}
