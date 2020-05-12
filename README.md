# Arduino ESP32 Temperature Logger to AWS IoT

## Introduction

This project deploys on an Arduino ESP32 microcontroller, like the [HiLetgo ESP-WROOM-32](https://www.amazon.com/dp/B01MY8U394/ref=cm_sw_em_r_mt_dp_U_kmShDbWT4C6YH) that I use. The controller will log data from an external [DS18B20-based temperature sensor](https://www.amazon.com/dp/B01MY8U394/ref=cm_sw_em_r_mt_dp_U_kmShDbWT4C6YH) to an AWS IoT MQQT endpoint over WiFi. See my other project, [ThingStack](https://github.com/edsw/ThingStack), for AWS CDK code used to deploy the AWS IoT components used.

## Setup

The project includes the Arduino code itself in `app.ino`, along with three header files (`.h`) used to configure the controller to work in your specific environment. These three files are:

1. `cert_params-template.h` – This file contains the three pieces of certificate material required to connect to AWS IoT:
  * Amazon's trusted certificate authority root certificate
  * The public key of your IoT Thing
  * The private key of your IoT Thing

  The first of these is public information, so I've included it in the template file and it is accurate as of May 2020. The others should be obtained from AWS IoT's certificate authority for your specific device and protected as a secret. See my example in [ThingStack](https://github.com/edsw/ThingStack) for storing these in AWS Secrets Manager as part of provisioning the IoT Thing. Don't include your device's private certificate information in a GitHub commit!

  Note that these values must be contrusted in a very specific string format in order to be supported by the Arduino's libraries. See the `rootCert` example in the template file, as well as my instructions in [ThingStack](https://github.com/edsw/ThingStack) for converting your other key material to this string format.

2. `mqqt_params-template.h` – This file contains the configuration parameters for your AWS IoT Thing:
  * `mqqtThing` – The name of your microcontroller as configured in AWS IoT
  * `mqqtServer` – The region-specific MQQT endpoint deployed in your AWS account by the AWS IoT service
  * `mqqtPort` – The TCP port used by the MQQT endpoint, which should be `8883`
  * `pubTopic` – The MQQT Topic published to by your controller

  I've included examples from my own use as default values. Make sure to update these to match your own environment.

3. `wifi_params-template.h` – This file contains the WiFi SSID and its password to which the Arduino will connect. Again, don't commit this information to GitHub!

Please rename each of these without the `-template` suffix and customize them to suit your specific environment.

Please also note the definition below which refers to my temperature sensor being connected to GPIO 15. Your value may be different.

```
#define ONE_WIRE_BUS 15
```

## Libraries Used

* [OneWire](https://www.pjrc.com/teensy/td_libs_OneWire.html) v2.3.4
* [DallasTemperature](https://github.com/milesburton/Arduino-Temperature-Control-Library) v3.8.0
* ESP32 v1.0.4
  * [WiFi](https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFi)
  * [WiFiClientSecure](https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFiClientSecure)
* [PubSubClient](https://github.com/knolleary/pubsubclient) v2.7

## See Also

* [ThingStack](https://github.com/edsw/ThingStack) – The AWS CDK Python project that I created to provision AWS IoT things and their corresponding device certificates.