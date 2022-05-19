#include "WiFi.h"
#include <stdio.h>
#include <PubSubClient.h>
#include "DHTesp.h"

WiFiClient espClient;
PubSubClient client(espClient);
DHTesp dhtSensor;

const int DHT_PIN = 15;

const char *ssid = "Wokwi-GUEST";
const char *password = "";
char server[] = "broker.netpie.io";

// sender
char ClientID[] = "083827d1-bcdc-465f-a71b-6d3b7555b3b8";
char token[] = "hCcnF3KrNYF1wXmCrEvtjLEBFof4c8gU";
char secret[] = "MB$ch)8pA8L79_r7b5ask$j$JG(3B-QS";

void setup_wifi()
{
    delay(10);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    randomSeed(micros());
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void reconnect()
{
    while (!client.connected())
    {
        Serial.print("Attempting MQTT connection...");
        // String clientId = "ItKMITL-62070035-sender";
        // clientId += String(random(0xffff), HEX);
        if (client.connect(ClientID, token, secret))
        {
            Serial.println("Connected");
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println("try again in 5 seconds");
            delay(5000);
        }
    }
}

void setup()
{
    Serial.begin(115200);
    setup_wifi();
    client.setServer(server, 1883);
    dhtSensor.setup(DHT_PIN, DHTesp::DHT22);
}

void loop()
{

    if (!client.connected())
    {
        reconnect();
    }
    client.loop();

    TempAndHumidity data = dhtSensor.getTempAndHumidity();

    // Temperature Sensor
    String val_str_temp = String(data.temperature, 2);
    char val_char_temp[10];
    val_str_temp.toCharArray(val_char_temp, 10);
    Serial.println("Temp: " + String(data.temperature, 2) + "°C");
    client.publish("@msg/telegraf/temp", val_char_temp);

    // Humidity Sensor
    String val_str_hum = String(data.humidity, 1);
    char val_char_hum[10];
    val_str_hum.toCharArray(val_char_hum, 10);
    Serial.println("Humidity: " + String(data.humidity, 1) + "%");
    client.publish("@msg/telegraf/humidity", val_char_hum);

    Serial.println("---");
    delay(3000);
}
