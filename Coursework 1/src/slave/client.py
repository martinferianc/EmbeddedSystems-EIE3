from machine import Pin I2C
import esp, network, json

class client:
	# Initialise the class with the device address and the player number.
    def __init__(self, devAddr, playerNum):
		self.__devAddr = devAddr
        self.p0
		self.__playerNum = playerNum
		# Setup the wifi and the accelerometer and the datastructures to store the data
        self.ap = setupWifi()
        self.accel = setupAccel()
		self.accelValues = {'ACX': 0, 'ACY': 0, 'ACZ': 0, 'TMP': 0, 'GYX': 0, 'GYY': 0, 'GYZ': 0}
		self.measurementList = [self.accelValues for x in range(100)]

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
		# Setup the I2C interface for the accelerometer
        accelHandle = I2C(scl=Pin(5), sda=Pin(4), freq = 100000)
        buf = bytearray(2)
        accelHandle.writeto(__devAddr, buf)

    def setupInterupt(self, pin):
        self.p0 = Pin(0, Pin.IN)
        self.p0.irq(trigger=Pin.IRQ_RISING, handler=mpu.read_sensor_reg())

    def updateAccelValues(self,accelValues):
		# Take all the values
        i = 0
        for key in accelValues:
            accelValues[key] = buf[i]<<8|buf[i+1]
            i = i+2
		# Store the last 100 measurements ready for transmission if a shock occurs
		if(len(self.measurementList) > 99):
			self.measurementList.pop(0)
		self.measurementList.append(accelValues)

	def getJsonValues(self):
		# Return the array of measurements as a json object
		return json.dumps(self.measurementList)
