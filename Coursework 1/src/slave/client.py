from machine import Pin, I2C
from umqtt.simple import MQTTClient
import machine
import ubinascii
import time
import esp
import network
import ujson
import micropython


# register defines
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

SMPLRT_DIV = micropython.const(25)
CONF = micropython.const(26)
GYRO_CONF = micropython.const(27)
ACCEL_CONF = micropython.const(28)
INT_EN = micropython.const(56)
INT_PIN_CFG = micropython.const(55)

# MQTT Defaults:
BROKER = "192.168.0.10"  # TEMP ADDRESS
CLIENT_ID = ubinascii.hexlify(machine.unique_id())
TOPIC = "sensor_reading"


class Client:

    ##################### Setup Functions ############################

        # Initialise the class with the device address and the player number.
    def __init__(self, playerNum, deviceAddress):
        self.i2c = I2C(scl = Pin(5), sda=Pin(4), freq=400000)
        self.slave = deviceAddress
        self.__playerNum = playerNum
        # Setup the wifi and the accelerometer and the datastructures to store the data
        #self.ap = self.setupWifi()
        self.initMpu()
        self.sampleNumber = 0
        accelValues = {'SAMPLE': self.sampleNumber, 'PLAYER': playerNum, 'ACX': 0, 'ACY': 0, 'ACZ': 0, 'TMP': 0, 'GYX': 0, 'GYY': 0, 'GYZ': 0}
        self.measurementList = [accelValues for x in range(50)]

    def setupWifi(self):
        wlan = network.WLAN(network.STA_IF)
        wlan.active(True)
        wlan.scan()
        if not (wlan.isconnected()):
            wlan.connect('essid', 'password')

        ap = network.WLAN(network.AP_IF)
        ap.active(True)
        return ap

    def initMpu(self):
        self.write_reg(SMPLRT_DIV, 0xFF)  # 8kHz sampling rate
        self.write_reg(CONF, 0x01)  # no external sync, largest bandwidth
        self.write_reg(GYRO_CONF, 0x18)  # 2000deg/s gyro range
        self.write_reg(ACCEL_CONF, 0x18)  # 16g accel range
        self.write_reg(INT_EN, 0x01)  # 16g accel range
        self.write_reg(INT_PIN_CFG, 0xF0)  # 16g accel 

###################### MPU Functions #############################

    def read_sensor_reg(self):
        sensor_buf = bytearray(14)
        self.i2c.readfrom_mem_into(self.slave, ACCEL_X_H_REG, sensor_buf)
        print("Data Read")
        self.updateAccelValues(sensor_buf)


    def write_reg(self, reg_addr, data):
        #if uctypes.size_of(data) > 1:
        #    raise ValueError('Only write 1 Byte of data to a register')
        #else:
        dataByte = bytearray(1)
        dataByte[0] = data
        self.i2c.writeto_mem(self.slave, reg_addr, dataByte)

##################### Client Functions ############################

    def getJsonValues(self):
        # Return the array of measurements as a json object
        return ujson.dumps(self.measurementList)

    def updateAccelValues(self, sensor_buf):
        # Update all of the values
        accelValues = {}
        accelValues['SAMPLE'] = self.sampleNumber
        accelValues['PLAYER'] = self.__playerNum
        accelValues['ACX'] = sensor_buf[0] << 8 | sensor_buf[1]
        accelValues['ACY'] = sensor_buf[2] << 8 | sensor_buf[3]
        accelValues['ACZ'] = sensor_buf[4] << 8 | sensor_buf[5]

        #update temperature value
        accelValues['TMP'] = sensor_buf[6] << 8 | sensor_buf[7]

        #update gyroscope values
        accelValues['GYX'] = sensor_buf[8] << 8 | sensor_buf[9]
        accelValues['GYY'] = sensor_buf[10] << 8 | sensor_buf[11]
        accelValues['GYZ'] = sensor_buf[12] << 8 | sensor_buf[13]

        # Store the last 100 measurements ready for transmission if a shock occurs
        if(len(self.measurementList) > 49):
            self.measurementList.pop(0)
        self.measurementList.append(accelValues)
        self.sampleNumber = self.sampleNumber +1

##################### Client MQTT Functions ############################

#    def publishDataToBroker(self):
#        client = MQTTClient(CLIENT_ID, BROKER)
#        client.connect()
#        client.publish(TOPIC, bytes(self.getJsonValues(), 'utf-8')
#
#   def getTimeFromBroker(self)
