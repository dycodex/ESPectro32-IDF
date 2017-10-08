COMPONENT_ADD_INCLUDEDIRS := Adafruit_Sensor Adafruit_BME280 HDC1080/src NeoGPS/src RTClib CCS811/src SX1509/src Alora/src 
#COMPONENT_PRIV_INCLUDEDIRS := cores/esp32/libb64
COMPONENT_SRCDIRS := Adafruit_Sensor Adafruit_BME280 HDC1080/src NeoGPS/src RTClib CCS811/src SX1509/src Alora/src 
CXXFLAGS += -fno-rtti -Wno-sign-compare -Wno-reorder -fpermissive -Wno-maybe-uninitialized -Wno-return-type