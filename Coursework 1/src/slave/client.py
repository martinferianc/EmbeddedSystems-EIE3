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
import socket

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
CLIENT_ID = "HeadAid" + str(ubinascii.hexlify(machine.unique_id()))
TOPIC = "esys/HeadAid/sensor"

#Network setup variables:
#SSID = 'Alexander\'s iPhone'
SSID = 'EEERover'
#NETWORK_PW = 'alexLuisi1996'
NETWORK_PW = 'exhibition'

DEBUG = False

class Client:

    ##################### Setup Functions ############################

        # Initialise the class with the device address and the player number.
    def __init__(self, playerNum, deviceAddress, measurementSize):
        #I2C setup
        self.i2c = I2C(scl = Pin(5), sda=Pin(4), freq=400000)

        #device information
        self.slave = deviceAddress
        self.__playerNum = playerNum

        #hardware threshold initialisation
        self.thresholdFlag = False
        self.thresholdValue = 500
        self.thresholdCounter = int(measurementSize/2)

        #network initialisation
        self.ap_if  = self.accessInit()
        self.sta_if = self.stationInit(10)

        #MPU register initialisation
        self.initMpu()

        #data to be sent 
        self.mainPack = {'PLAYER': playerNum, 'DEVICE ADDRESS': deviceAddress, 'DATA': []}
        #list arrangement: ACX-ACY-ACZ-TMP-GYX-GYY-GYZ

        #MQTT setup
        self.mqttClient = MQTTClient(CLIENT_ID,BROKER)
        self.mqttClient.connect()

    def accessInit(self):
        #access point setup
        ap = network.WLAN(network.AP_IF)
        ap.active(True)

        if DEBUG:
            print (ap.ifconfig())

        return ap

    def stationInit(self,timeout):
        #station setup
        wlan = network.WLAN(network.STA_IF)
        wlan.active(False)
        wlan.active(True)
        wlan.scan()
        wlan.connect(SSID, NETWORK_PW)
        #TODO: maybe loop until connected? and also have some sort of timeout?
        while (not wlan.isconnected()) and ( not timeout == 0):
            time.sleep(1)
            timeout -= 1

        if timeout == 0:
            print('STA ERROR')

        if DEBUG:
            print (wlan.ifconfig())

        return wlan

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
        self.i2c.writeto_mem(self.slave, reg_addr, bytes(dataByte))

##################### Client Functions ############################

    def updateAccelValues(self, sensor_buf):
        # Update all of the values
        values = [0,0,0,0,0,0,0]
        
        #update acceleration values
        values[0] = sensor_buf[0] << 8 | sensor_buf[1]
        values[1] = sensor_buf[2] << 8 | sensor_buf[3]
        values[2] = sensor_buf[4] << 8 | sensor_buf[5]

        #update temperature value
        values[3] = sensor_buf[6] << 8 | sensor_buf[7]

        #update gyroscope values
        values[4] = sensor_buf[8] << 8 | sensor_buf[9]
        values[5] = sensor_buf[10] << 8 | sensor_buf[11]
        values[6] = sensor_buf[12] << 8 | sensor_buf[13]

        #update main packet
        self.mainPack['DATA'] = values

        #publish data
        micropython.schedule(self.publishDataToBroker,0)


##################### Client MQTT Functions ############################

    def publishDataToBroker(self,_):    
        # Check if there is an active connection
        if not self.ap_if.active:
            # Reconnect if there isn't
            self.ap_if = self.accessInit() 
        
        if not self.sta_if.active:
            self.sta_if = self.stationInit(10)

        # Publish the data to the MQTT broker
        self.mqttClient.publish(TOPIC, bytes(ujson.dumps(self.mainPack),'utf-8'))
    
