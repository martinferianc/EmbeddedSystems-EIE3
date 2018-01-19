import machine, esp, network

class huzzah:
    def __init__(self):
        self.ap = setupWifi()
        self.accel = setupAccel()
        self.accelValues = {'ACX': 0, 'ACY': 0, 'ACZ': 0, 'TMP': 0, 'GYX': 0, 'GYY': 0, 'GYZ': 0}

    def setupWifi(self):
        wlan = network.WLAN(network.STA_IF)
        wlan.active(True)
        wlan.scan()
        if(!wlan.isconnected()):
            wlan.connect('essid', 'password')

        ap = network.WLAN(network.AP_IF)
        ap.active(True)
        return ap


    def setupAccel(self):
        accelHandle = I2C(scl=Pin(5), sda=Pin(4), freq = 100000)
        buf = bytearray(2)
        accelHandle.writeto(0x68, buf)

    def updateAccelValues(self, accelHandle, accelValues):
        accelHandle.writeto(0x68, 0x3B)
        buf = bytearray(14);
        buf = accelHandle.readfrom(0x68, 14)
        i = 0
        for key in accelValues:
            accelValues[key] = buf[i]<<8|buf[i+1]
            i = i+2
        
