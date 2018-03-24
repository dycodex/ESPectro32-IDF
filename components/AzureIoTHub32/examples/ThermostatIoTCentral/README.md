### Microsoft IoT Central
This Thermostat example shows how we can use the AzureIoTHub32 libary for publishing device's telemetry to Microsoft IoT Central. 
Microsoft IoT Central is essentially a web-based on application leveraging Azure IoT Hub and other Azure's service, so you'll see some terminologies in Azure IoT Hub when using it.

### Using it
Take a look the example, and you'll see `IOT_CENTRAL_DEVICE_CONN_STRING`. Change it with the connection string you copy from Microsoft IoT Central device page. 
This example doesn't require real sensors to publish temperature and  pressure as it will randomize that values. If everything is set correctly, you'll begin to see the data coming to you device Measurements as this screenshot: 
![Screenshot](https://github.com/dycodex/ESPectro32-IDF/raw/master/components/AzureIoTHub32/examples/ThermostatIoTCentral/screenshot1.png)

### Limitation
Currently the library only handles telemetry publishing. It has not yet handled `Setting` update from Microsoft IoT Central.
