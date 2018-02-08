from machine import Pin, I2C
from umqtt.simple import MQTTClient
import machine
import ubinascii
import time
import esp
import network
import ujson
import micropython
import uheapq

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

SMPLRT_DIV  = micropython.const(25)
CONF        = micropython.const(26)
GYRO_CONF   = micropython.const(27)
ACCEL_CONF  = micropython.const(28)
INT_EN      = micropython.const(56)
INT_PIN_CFG = micropython.const(55)
PWR_MGMT    = micropython.const(107)

# MQTT Defaults:
#BROKER = "172.20.10.7"  # TEMP ADDRESS
BROKER = "192.168.0.50"  # TEMP ADDRESS
CLIENT_ID = "HeadAid" #+ ubinascii.hexlify(machine.unique_id())
TOPIC = "esys/HeadAid/sensor"


#Network setup variables:
#SSID = 'Alexander\'s iPhone'
SSID = 'EEERover'
#NETWORK_PW = 'alexLuisi1996'
NETWORK_PW = 'exhibition'

class Client:

    ##################### Setup Functions ############################

        # Initialise the class with the device address and the player number.
    def __init__(self, playerNum, deviceAddress, measurementSize):
        self.i2c = I2C(scl = Pin(5), sda=Pin(4), freq=400000)
        self.slave = deviceAddress
        self.__playerNum = playerNum
        self.measurementSize = measurementSize

        #hardware threshold init
        self.thresholdFlag = False
        self.thresholdValue = 500
        self.thresholdCounter = int(measurementSize/2)

        #network init
        self.ap_if  = self.accessInit()
        self.sta_if = self.stationInit()

        # Setup the wifi and the accelerometer and the datastructures to store the data
        #self.ap = self.setupWifi()
        self.initMpu()

        self.mainPack = {'PLAYER': playerNum, 'DEVICE ADDRESS': deviceAddress, 'TIMESTAMP': 0, 'DATA': []}

        #ACX-ACY-ACZ-TMP-GYX-GYY-GYZ
        tmp = [0,0,0,0,0,0,0]
        self.measurementList = [tmp for x in range(measurementSize)]

        uheapq.heapify(self.measurementList)

        self.mqttClient = MQTTClient(CLIENT_ID,BROKER)
        self.mqttClient.connect()

        print('here,init')


    def accessInit(self):
        #access point setup
        ap = network.WLAN(network.AP_IF)
        ap.active(True)

        print (ap.ifconfig())

        return ap

    def stationInit(self):

        #station setup
        wlan = network.WLAN(network.STA_IF)
        wlan.active(False)
        wlan.active(True)
        wlan.scan()
        wlan.connect(SSID, NETWORK_PW)
        #TODO: maybe loop until connected? and also have some sort of timeout?
        while not wlan.isconnected():
            pass

        print (wlan.ifconfig())

        return wlan

    def stationRefresh(self):
        if not self.sta_if.isconnected():
            self.sta_if.scan()
            self.sta_if.connect(SSID, NETWORK_PW)
        return

    def initMpu(self):
        self.write_reg(PWR_MGMT, 0x80)  # reset
        time.sleep(1)
        self.write_reg(PWR_MGMT, 0x00)  # power on
        self.write_reg(SMPLRT_DIV, 0x7F)  # 8kHz sampling rate
        #TODO adjust sample rate
        self.write_reg(CONF, 0x01)  # no external sync, largest bandwidth
        self.write_reg(GYRO_CONF, 0x18)  # 2000deg/s gyro range
        self.write_reg(ACCEL_CONF, 0x18)  # 16g accel range
        self.write_reg(INT_EN, 0x01)  # 16g accel range
        self.write_reg(INT_PIN_CFG, 0x90)  # 16g accel
        print('here, init MPU')

###################### MPU Functions #############################

    def read_sensor_reg(self,_):

        #disable interrupts whilst doing memory operations
        irq_state = machine.disable_irq()

        #buffer for the sensor data
        sensor_buf = bytearray(14)

        #read sensor data into buffer
        self.i2c.readfrom_mem_into(self.slave, ACCEL_X_H_REG, sensor_buf)

        #update the sensor values in class dictionary
        self.updateAccelValues(sensor_buf)

        #re-enable interrupts
        machine.enable_irq(irq_state)

    def write_reg(self, reg_addr, data):
        dataByte = bytearray(1)
        dataByte[0] = data
        self.i2c.writeto_mem(self.slave, reg_addr, dataByte)

##################### Client Functions ############################

    def getJsonValues(self):
        # Return the array of measurements as a json object
        #return ujson.dumps(self.measurementList)
        for item in self.measurementList:
            print(item)
        print('\n')

    def getAccelMagnitude(self, values):
        return values[0]+values[1]+values[2]

    def resetThresholdCounter(self):
        self.thresholdCounter = int(self.measurementSize/2)

    def updateAccelValues(self, sensor_buf):

        print('here, update')

        # Update all of the values
        accelValues = [0,0,0,0,0,0,0]
        
        accelValues[0] = sensor_buf[0] << 8 | sensor_buf[1]
        accelValues[1] = sensor_buf[2] << 8 | sensor_buf[3]
        accelValues[2] = sensor_buf[4] << 8 | sensor_buf[5]

        #update temperature value
        accelValues[3] = sensor_buf[6] << 8 | sensor_buf[7]

        #update gyroscope values
        accelValues[4] = sensor_buf[8] << 8 | sensor_buf[9]
        accelValues[5] = sensor_buf[10] << 8 | sensor_buf[11]
        accelValues[6] = sensor_buf[12] << 8 | sensor_buf[13]

        # Store the last 100 measurements ready for transmission if a shock occurs
        if(len(self.measurementList) > 5):
            self.measurementList.pop(0)
        uheapq.heappush(self.measurementList,accelValues)

        #flag set: decrease counter
        if self.thresholdFlag:
          self.thresholdCounter -= 1

        #check for threshold
        if (self.getAccelMagnitude(accelValues)>self.thresholdValue) and not self.thresholdFlag:
            #reset threshold
            print('here, threshold')
            self.resetThresholdCounter()
            self.thresholdFlag = True

        if self.thresholdFlag and (self.thresholdCounter==0):
            #publish results
            micropython.schedule(self.publishDataToBroker,0)
            self.thresholdFlag = False


##################### Client MQTT Functions ############################

    def publishDataToBroker(self,_):

        print('here, publish')

        #disable interrupts whilst sending data
        #irq_state = pyb.disable_irq()
        '''
        # Check if there is an active connection
        if not self.ap_if.active:
            # Reconnect if there isn't
            self.ap_if = self.accessInit() #TODO: need to check what we are reconnecting to
            self.mqttClient = MQTTClient(CLIENT_ID,BROKER)
            self.mqttClient.connect()
        '''
        # Serialize the data packet
        self.mainPack['DATA'] = self.measurementList
        self.mainPack['TIMESTAMP'] = self.getTimeStamp()

        # Publish the data to the MQTT broker
        #self.mqttClient.publish(TOPIC, bytes(ujson.dumps(self.mainPack),'utf-8'))
        self.mqttClient.publish(TOPIC, bytes('Hello World!','utf-8'))

        #enable interrupts again (keep collecting data)
        #pyb.enable_irq(irq_state)

    def getTimeStamp(self):
        pass
'''
    def setTime(self):
        # Get the time from an NTP server at startup and set the RTC
        rtc = machine.RTC()
        rtc.init(settime())
'''
