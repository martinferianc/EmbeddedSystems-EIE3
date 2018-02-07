#must be connected to mqtt broker network
#SSID: EEERover    PASSWORD: exhibition
mosquitto_sub -h 192.168.0.10 -v -t esys/headaid/#
