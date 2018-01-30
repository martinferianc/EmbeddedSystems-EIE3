from machine import Pin I2C
from umqtt.simple import MQTTClient
import machine, ubinascii, time
import esp, network, json


#register defines
ACCEL_X_H_REG = micropython.const(59)
ACCEL_X_L_REG = micropython.const(60)
ACCEL_Y_H_REG = micropython.const(61)
ACCEL_Y_L_REG = micropython.const(62)
ACCEL_Z_H_REG = micropython.const(63)
ACCEL_Z_L_REG = micropython.const(64)

GYRO_X_H_REG = micropython.const(67)
GYRO_X_L_REG = micropython.const(68)
GYRO_Y_H_REG = micropython.const(69)
GYRO_Y_L_REG = micropython.const(70)
GYRO_Z_H_REG = micropython.const(71)
GYRO_Z_L_REG = micropython.const(72)

SMPLRT_DIV  = micropython.const(25)
CONF        = micropython.const(26)
GYRO_CONF   = micropython.const(27)
ACCEL_CONF  = micropython.const(28)
INT_EN      = micropython.const(56)
INT_PIN_CFG = micropython.const(55)

# MQTT Defaults:
BROKER = "192.168.0.10" # TEMP ADDRESS
CLIENT_ID = ubinascii.hexlify(machine.unique_id())
TOPIC = b"sensor_reading"

class client:

##################### Setup Functions ############################

	# Initialise the class with the device address and the player number.
    def __init__(self, playerNum, deviceAddress):
        self.i2c = I2C(freq=400000)
        self.slave = i2c.scan()
        self.sensor_buf = bytearray(14)
        self.p0
		self.__playerNum = playerNum
		# Setup the wifi and the accelerometer and the datastructures to store the data
        self.ap = self.setupWifi()
        self.initMpu()
		self.accelValues = {'PLAYER': playerNum, 'DEVICE ADDRESS': deviceAddress, 'ACX': 0, 'ACY': 0, 'ACZ': 0, 'TMP': 0, 'GYX': 0, 'GYY': 0, 'GYZ': 0}
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

    def initMpu(self):
        self.write_reg(SMPLRT_DIV   , b'00000000') #8kHz sampling rate
        self.write_reg(CONF         , b'00000000') #no external sync, largest bandwidth
        self.write_reg(GYRO_CONF    , b'00011000') #2000deg/s gyro range
        self.write_reg(ACCEL_CONF   , b'00011000') #16g accel range
        self.write_reg(INT_EN       , b'00000001') #16g accel range
        self.write_reg(INT_PIN_CFG  , b'10100000') #16g accel range

###################### MPU Functions #############################

    def read_sensor_reg(self):
        self.i2c.readfrom_mem_into(self.slave,ACCEL_X_H_REG,self.sensor_buf)
        micropython.schedule(self.updateAccelValues)

    def write_reg(self, reg_addr, data):
        if uctypes.size_of(data) > 1:
            raise ValueError('Only write 1 Byte of data to a register')
        else:
            self.i2c.writeto_mem(self.slave,reg_addr,data)

##################### Client Functions ############################

	def getJsonValues(self):
		# Return the array of measurements as a json object
		return ujson.dumps(self.measurementList)

    def updateAccelValues(self):
		# Take all the values
        i = 0
        for key in accelValues:
            accelValues[key] = self.sensor_buf[i]<<8|self.sensor_buf[i+1]
            i = i+2
		# Store the last 100 measurements ready for transmission if a shock occurs
		if(len(self.measurementList) > 99):
			self.measurementList.pop(0)
		self.measurementList.append(accelValues)

##################### Client MQTT Functions ############################

    def publishDataToBroker(self):
        client = MQTTClient
