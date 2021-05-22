
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include <ArduinoMqttClient.h>
#if defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_AVR_UNO_WIFI_REV2)
#include <WiFiNINA.h>
#elif defined(ARDUINO_SAMD_MKR1000)
#include <WiFi101.h>
#elif defined(ARDUINO_ESP8266_ESP12)
#include <ESP8266WiFi.h>
#endif


char ssid[] = "MIWIFI_2G_HTsX";        // your network SSID (name)
char pass[] = "CuDUdEse";    // your network password (use for WPA, or use as key for WEP)

/* Initialise the LiquidCrystal library. The default address is 0x27 and this is a 16x2 line display */
LiquidCrystal_I2C lcd(0x27, 20, 4);
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "test.mosquitto.org";
int        port     = 1883;
const char topic[]  = "blogTFMAntoni";

bool haveMessage = false;
int ticks = 0;
void setup()
{
  /* Initialise the LCD */
  lcd.init();
  lcd.init();

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // attempt to connect to Wifi network:
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }

  Serial.println("You're connected to the network");
  Serial.println();

  // You can provide a unique client ID, if not set the library uses Arduino-millis()
  // Each client must have a unique client ID
  // mqttClient.setId("clientId");

  // You can provide a username and password for authentication
  // mqttClient.setUsernamePassword("username", "password");

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();

  Serial.print("Subscribing to topic: ");
  Serial.println(topic);
  Serial.println();


  mqttClient.subscribe(topic);

  Serial.print("Waiting for messages on topic: ");
  Serial.println(topic);
  Serial.println();


  lcd.backlight();
  waitingMessage();



}

/* Main program loop */
void loop()
{
  ticks = ticks + 1;
  Serial.print("Ticks: ");
  Serial.println(ticks);
  lcd.backlight();

  int messageSize = mqttClient.parseMessage();
  if (messageSize) {
    ticks = 0;
    haveMessage = true;
    Serial.print("Received a message with topic '");
    Serial.print(mqttClient.messageTopic());
    Serial.print("', length ");
    Serial.print(messageSize);
    Serial.println(" bytes:");
    int fila = 0;
    int columna = 0;
    lcd.clear();
    while (mqttClient.available()) {
      if (columna == 20) {
        columna = 0;
        fila = fila + 1;
      }
      if (fila == 4) {
        delay(500);
        lcd.clear();
        fila = 0;
      }
      lcd.setCursor(columna, fila);
      char caracter = (char)mqttClient.read();
      lcd.print(caracter);
      Serial.print(caracter);
      columna = columna + 1;
      delay(100);
    }
    Serial.println();
  }

  if (haveMessage && ticks == 30000) {
    haveMessage = false;
    ticks = 0;
    waitingMessage();
    Serial.println("Borrariamos mensaje");
  }
}

void waitingMessage() {
  lcd.clear();
  lcd.setCursor(0, 0);
  String waitingMessage = "Waiting for message.";
  for (int i = 0; i < waitingMessage.length(); i++) {
    lcd.setCursor(i, 0);
    lcd.print(waitingMessage[i]);
  }
}
