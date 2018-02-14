from machine import Pin, I2C
from umqtt.simple import MQTTClient
import machine
import ubinascii
import time
import esp
import network
import ujson
import micropython

# Accelerometer register definitions
ACCEL_X_H_REG = micropython.const(59)
ACCEL_X_L_REG = micropython.const(60)
ACCEL_Y_H_REG = micropython.const(61)
ACCEL_Y_L_REG = micropython.const(62)
ACCEL_Z_H_REG = micropython.const(63)
ACCEL_Z_L_REG = micropython.const(64)

# Gyroscope register definitions
GYRO_X_H_REG = micropython.const(67)
GYRO_X_L_REG = micropython.const(68)
GYRO_Y_H_REG = micropython.const(69)
GYRO_Y_L_REG = micropython.const(70)
GYRO_Z_H_REG = micropython.const(71)
GYRO_Z_L_REG = micropython.const(72)

# Configuration register definitions
SMPLRT_DIV  = micropython.const(25)
CONF        = micropython.const(26)
GYRO_CONF   = micropython.const(27)
ACCEL_CONF  = micropython.const(28)
INT_EN      = micropython.const(56)
INT_PIN_CFG = micropython.const(55)
PWR_MGMT    = micropython.const(107)

# MQTT Details:
BROKER    = "192.168.0.10" # Broker Address 
CLIENT_ID = "HeadAid" + str(ubinascii.hexlify(machine.unique_id()))
TOPIC     = "esys/HeadAid/sensor" # Sensor topic

#Network setup variables:
SSID        = 'EEERover'
NETWORK_PW  = 'exhibition'

DEBUG = False 

class Client:

    ##################### Setup Functions ############################

    def __init__(self, playerNum, deviceAddress, measurementSize):
        
        print('---SETUP START---')
        
        # I2C setup
        self.i2c = I2C(scl = Pin(5), sda=Pin(4), freq=400000)

        # Sending data flag
        self.BOARD_ON = False 

        #device information
        self.slave = deviceAddress
        self.__playerNum = playerNum

        #hardware threshold initialisation
        self.accelThreshold = 2500
        self.gyroThreshold  = 100

        #network initialisation
        self.ap_if  = self.accessInit()
        self.sta_if = self.stationInit(10)

        #MPU register initialisation
        self.initMpu()

        #data to be sent
        self.mainPack = [0,0,0,0,0]
        #list arrangement: ACX-ACY-ACZ-GYX-GYY-GYZ-playerNum-deviceAddr

        #MQTT setup
        self.mqttClient = MQTTClient(CLIENT_ID,BROKER)
        self.mqttClient.set_callback(self.sub_cb)
        self.mqttClient.connect()
        self.mqttClient.subscribe(b"esys/HeadAid/on_field_status1")
        
        print('---SETUP DONE---')
        
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
        wlan.active(True)
        wlan.scan()
        wlan.connect(SSID, NETWORK_PW)
        while not wlan.isconnected():
            pass

        if DEBUG:
            print (wlan.ifconfig())

        return wlan

    ### Initialise the MPU sensor
    def initMpu(self):
        self.write_reg(PWR_MGMT, 0x80) # reset
        time.sleep(1)
        self.write_reg(PWR_MGMT, 0x00) # power on
        self.write_reg(SMPLRT_DIV, 0x7F) # 8kHz sampling rate
        self.write_reg(CONF, 0x01) # no external sync,largest bandwidth
        self.write_reg(GYRO_CONF, 0x18) # 2000deg/s gyro range
        self.write_reg(ACCEL_CONF, 0x18) # 16g accel range
        self.write_reg(INT_EN, 0x01) # 16g accel range
        self.write_reg(INT_PIN_CFG, 0x90) # 16g accel

###################### MPU Functions #############################

    ### Read sensor registers
    def read_sensor_reg(self,_):
        #disable interrupts whilst doing memory operations
        irq_state = machine.disable_irq()

        #buffer for the sensor data
        sensor_buf = bytearray(14)

        #read sensor data into buffer
        self.i2c.readfrom_mem_into(self.slave,ACCEL_X_H_REG,sensor_buf)

        #update the sensor values in class dictionary
        self.updateValues(sensor_buf)

        #re-enable interrupts
        machine.enable_irq(irq_state)

    ### write to register
    def write_reg(self, reg_addr, data):
        dataByte = bytearray(1)
        dataByte[0] = data
        self.i2c.writeto_mem(self.slave, reg_addr, dataByte)

##################### Client Functions ############################

    ### Turn 16bit integer to signed integer
    def intSigned(self,val):
        if val > 32768:
          return val - 65536
        return val

    ### lightweight magnitude function
    def magnitude(self,x,y,z):
        return abs(self.intSigned(x))+abs(self.intSigned(y))+abs(self.intSigned(z))

    ### update values to be sent
    def updateValues(self, sensor_buf):
        if DEBUG:
            print('updating values')

        #compress into 3 32bit integers
        self.mainPack[0] = int(sensor_buf[2]<<24  | sensor_buf[3]<<16  | sensor_buf[0]<<8  | sensor_buf[1])
        self.mainPack[1] = int(sensor_buf[8]<<24  | sensor_buf[9]<<16  | sensor_buf[4]<<8  | sensor_buf[5])
        self.mainPack[2] = int(sensor_buf[12]<<24 | sensor_buf[13]<<16 | sensor_buf[10]<<8 | sensor_buf[11])

        #only sent above threshold
        if self.magnitude(sensor_buf[0]<<8|sensor_buf[1],sensor_buf[2]<<8|sensor_buf[3],sensor_buf[4]<<8|sensor_buf[5]) > self.accelThreshold or self.magnitude(sensor_buf[8]<<8|sensor_buf[9],sensor_buf[10]<<8|sensor_buf[11],sensor_buf[12]<<8|sensor_buf[13]) > self.gyroThreshold:
            #publish to broker
            micropython.schedule(self.publishDataToBroker,0)

##################### Client MQTT Functions ############################

    ### encryption function
    def encrypt(self, val):
        temp = int(0)

        # shifting bytes around
        temp = temp | ((val >> 0)  & 0x000000FF)
        temp = temp | ((val >> 16) & 0x0000FF00)
        temp = temp | ((val >> 0)  & 0x00FF0000)
        temp = temp | ((val << 16) & 0xFF000000)
        
        return temp

    ### subscription callback
    def sub_cb(self,topic,msg):
        m = int(msg.decode('utf-8'))
        print('got command')
        #turn on sending data
        if m == 1:
            self.BOARD_ON = True
        #turn off sending data
        if m == 0:
            self.BOARD_ON = False

    ### publish function
    def publishDataToBroker(self,_):
        if DEBUG:
            print('sent data')
        
        self.mainPack[3] = self.__playerNum
        self.mainPack[4] = self.slave
        # Publish the data to the MQTT broker
        if self.BOARD_ON:
            self.mainPack = [self.encrypt(int(d)) for d in self.mainPack]
            if DEBUG:
                print('sent data') 
            self.mqttClient.publish(TOPIC, bytes(ujson.dumps(self.mainPack),'utf-8'))
