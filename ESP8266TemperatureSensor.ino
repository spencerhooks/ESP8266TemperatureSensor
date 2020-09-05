// Sketch that sets up an Adafruit Feather Huzzah to be used as a battery powered
// temperature sensor. Data is sent to Adafruit.io for monitoring. Using a LiPo battery
// and a DHT22 temperature/humidty sensory.

#define DEBUG                       // Used to show/hide the debug messages on the serial monitor

// Config file that includes Adafruit.io account info and wifi config
#include "config.h"
//#include <EEPROM.h>

void setup() {
  // Initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

#ifdef DEBUG
  // Setup the serial connection
  Serial.begin(115200);
  Serial.setTimeout(2000);

  // Wait for serial to initialize.
  while (!Serial) { }
#endif

  // Turn the LED off by making the voltage LOW
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  // Call function to read the battery level and send to Adafruit.io
  battery_level();

  // Wait 60 seconds so that we are only uploading at that interveral
  delay(60 * 1000);

  // Deep sleep mode for 30 seconds, the ESP8266 wakes up by itself when GPIO 16 (D0 in NodeMCU board) is connected to the RESET pin
  //  Serial.println("I'm awake, but I'm going into deep sleep mode for 30 seconds");
  //  ESP.deepSleep(30e6);

  // Deep sleep mode until RESET pin is connected to a LOW signal (for example pushbutton or magnetic reed switch)
  //Serial.println("I'm awake, but I'm going into deep sleep mode until RESET pin is connected to a LOW signal");
  //ESP.deepSleep(0);

}

// Function to read the battery level from the analog input pin on the Feather,
// convert it to percentage, and send to Adafruit.io.
void battery_level() {
  // Read the analog input pin on the Feather to the variable voltage
  int voltage = analogRead(A0);
  int percent;

  // Convert voltage read from analog pin above to battery charge percentage.
  // The battery is a 4.3/3.7v battery, while the input pin on the Adafruit Feather
  // has a maximum input of 1v. Therefore, the voltage is being measured across the
  // 22k ohm leg of a 100k/22k ohm voltage divider. The range below (580 to 790)
  // is the range of voltage at the analog input that we are mapping to the full
  // charge range of the battery (0-100%).
  percent = map(voltage, 580, 790, 0, 100);

#ifdef DEBUG
  Serial.print("Battery voltage: "); Serial.println(voltage);
  Serial.print("Battery level: "); Serial.print(percent); Serial.println("%");
#endif

  // connect to Adafruit IO
  connect_AIO();

  // grab the battery feeds
  AdafruitIO_Feed *batterypercentage = io.feed("batterypercentage");
  AdafruitIO_Feed *batteryvoltage = io.feed("batteryvoltage");

  // send battery levels to AIO
  batterypercentage->save(percent);
  batteryvoltage->save(voltage);
  io.run();
}

void connect_AIO() {

#ifdef DEBUG
  Serial.println("Connecting to Adafruit IO...");
#endif

  io.connect();

  // wait for a connection
  while (io.status() < AIO_CONNECTED) {
#ifdef DEBUG
    Serial.print(".");
#endif
    delay(500);
  }

#ifdef DEBUG
  // we are connected
  Serial.println();
  Serial.println(io.statusText());
#endif

}
