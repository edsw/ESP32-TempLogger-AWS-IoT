#define ARDUINO 100
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

#include "cert_params.h"
#include "wifi_params.h"
#include "mqqt_params.h"

#define ONE_WIRE_BUS 15

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress insideThermometer;
WiFiClientSecure wifiClient;
PubSubClient client;

long messageSentAt = 0;
char pubMessage[128];

void connectWifi() {
    int counter = 0;

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Attempting to connect to WiFi...");
        WiFi.begin(ssid, ssidPassword);
        counter++;

        if (counter >= 60) {
            Serial.println("Unable to connect to WiFi...");
            Serial.println("Restarting Arduino");
            ESP.restart();
        }
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.print("Attempting to connect to ");
    Serial.println(ssid);

    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);

    connectWifi();

    Serial.println("WiFi connected...");
    Serial.print("IP address set: ");
    Serial.println(WiFi.localIP());

    wifiClient.setCACert(rootCert);
    wifiClient.setCertificate(pubCert);
    wifiClient.setPrivateKey(privKey);

    client.setClient(wifiClient);
    client.setServer(mqqtServer, mqqtPort);

    sensors.begin();
}

void loop() {
    connectWifi();

    int counter = 0;

    while (!client.connected()) {
        delay(500);
        Serial.println("Attempting to connect to MQQT server...");
        
        if (!client.connect(mqqtThing)) {
            Serial.print("Failed to connect to MQQT server. rc = ");
            Serial.println(client.state());
        }
        
        counter++;

        if (counter >= 4) { // Default timeout in PubSubCLient is 15sec
            Serial.println("Unable to connect to MQQT server...");
            Serial.println("Restarting Arduino");
            ESP.restart();
        }
    }

    client.loop();

    long now = millis();
    if (now - messageSentAt > 60000) {
        messageSentAt = now;
        
        sensors.requestTemperatures();
        float tempF = sensors.getTempFByIndex(0);

        sprintf(pubMessage, "{\"tempF\":\"%.2f\"}", tempF);

        Serial.print("Publishing ");
        Serial.print(pubMessage);
        Serial.print(" to ");
        Serial.println(pubTopic);
        
        client.publish(pubTopic, pubMessage);
    }
}
