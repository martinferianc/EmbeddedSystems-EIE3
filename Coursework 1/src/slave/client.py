from machine import Pin, I2C
from umqtt.simple import MQTTClient
from ntptime import settime
import machine, ubinascii, time
import esp, network, ujson
import micropython

#register defines
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
PWR_MGMT_1 = micropython.const(107)

# MQTT Defaults:
BROKER = "192.168.0.10"  # TEMP ADDRESS
CLIENT_ID ='headAid' + str(ubinascii.hexlify(machine.unique_id()))
TOPIC = "sensor_reading"

#Network setup variables:
SSID = 'EEERover'
NETWORK_PW = 'exhibition'

class Client:

    ##################### Setup Functions ############################

        # Initialise the class with the device address and the player number.
    def __init__(self, playerNum, deviceAddress):
        self.i2c = I2C(scl = Pin(5), sda=Pin(4), freq=400000)
        self.slave = deviceAddress
        self.sensor_buf = bytearray(14)
	self.__playerNum = playerNum
		# Set up the wifi and the accelerometer and the datastructures to store the data

        #network init
        self.ap_if  = self.accessInit()
        self.sta_if = self.stationInit()

        # Set up the RTC
        # TODO is it required to have the RTC as a publically available value?
        self.setTime()
        self.mainPack = {'PLAYER': playerNum, 'DEVICE ADDRESS': deviceAddress, 'TIMESTAMP': 0, 'DATA': []}
        self.accelValues = {'ACX': 0, 'ACY': 0, 'ACZ': 0, 'TMP': 0, 'GYX': 0, 'GYY': 0, 'GYZ': 0}
        self.measurementList = [self.accelValues for x in range(100)]
        self.mainPack['DATA'] = self.measurementList
        self.client = MQTTClient(CLIENT_ID,BROKER)
        self.client.connect()

    def accessInit(self):
        #access point setup
        ap = network.WLAN(network.AP_IF)
        ap.active(True)

        print (ap.ifconfig())

        return ap

    def stationInit(self):

        #station setup
        wlan = network.WLAN(network.STA_IF)
        wlan.active(True)
        wlan.scan()
        #TODO: maybe loop until connected? and also have some sort of timeout?
        if not wlan.isconnected():
            wlan.connect(SSID, NETWORK_PW)

        print (wlan.ifconfig())

        return wlan

    def stationRefresh(self):
        if not self.sta_if.isconnected():
            self.sta_if.scan()
            self.sta_if.connect(SSID, NETWORK_PW)
        return

    def initMpu(self):
        self.write_reg(PWR_MGMT_1, 0x00)
        self.write_reg(SMPLRT_DIV, 0xFF)  # 8kHz sampling rate
        self.write_reg(CONF, 0x01)  # no external sync, largest bandwidth
        self.write_reg(GYRO_CONF, 0x18)  # 2000deg/s gyro range
        self.write_reg(ACCEL_CONF, 0x18)  # 16g accel range
        self.write_reg(INT_EN, 0x01)
        self.write_reg(INT_PIN_CFG, 0x30)
        #TODO: write checks to make sure config correct


###################### MPU Functions #############################

    def read_sensor_reg(self):
        self.i2c.readfrom_mem_into(self.slave, ACCEL_X_H_REG, self.sensor_buf)
        micropython.schedule(self.updateAccelValues)

    def write_reg(self, reg_addr, data):
        #if uctypes.size_of(data) > 1:
        #    raise ValueError('Only write 1 Byte of data to a register')
        #else:
        self.i2c.writeto_mem(self.slave, reg_addr, bytes(data))

##################### Client Functions ############################

    def updateAccelValues(self):
        #update accelerator values
        self.accelValues['ACX'] =  self.sensor_buf[0] << 8 | self.sensor_buf[1]
        self.accelValues['ACY'] =  self.sensor_buf[2] << 8 | self.sensor_buf[3]
        self.accelValues['ACZ'] =  self.sensor_buf[4] << 8 | self.sensor_buf[5]

        #update temperature value
        self.accelValues['TMP'] =  self.sensor_buf[6] << 8 | self.sensor_buf[7]

        #update gyroscope values
        self.accelValues['GYX'] =  self.sensor_buf[8] << 8 | self.sensor_buf[9]
        self.accelValues['GYY'] =  self.sensor_buf[10] << 8 | self.sensor_buf[11]

        # Store the last 100 measurements ready for transmission if a shock occurs
        # FIFO structure
        if(len(self.measurementList) > 99):
            self.measurementList.pop(0)
        self.measurementList.append(self.accelValues)
        # TODO Routine here to publish data when impact is over threashold
        #TODO: only center data around threshold point (ie keep collecting data for next 50 samples)

##################### Client MQTT Functions ############################

    def publishDataToBroker(self):

        #disable interrupts whilst sending data
        #irq_state = pyb.disable_irq()

        # Check if there is an active connection
        if not self.ap.active:
            # Reconnect if there isn't
            self.ap_if = self.accessInit() #TODO: need to check what we are reconnecting to
            self.client = MQTTClient(CLIENT_ID,BROKER)
            self.client.connect()

        # Serialize the data packet
        self.mainPack['DATA'] = self.measurementList
        self.mainPack['TIMESTAMP'] = self.getTimeStamp()
        # Publish the data to the MQTT broker
        self.client.publish(TOPIC, bytes(ujson.dumps(self.mainPack)), 'utf-8')

        #enable interrupts again (keep collecting data)
        #pyb.enable_irq(irq_state)

    def getTimeStamp(self):
        pass

    def setTime(self):
        # Get the time from an NTP server at startup and set the RTC
        rtc = machine.RTC()
        rtc.init(settime())
