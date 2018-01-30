import micropython
import uctypes
from machine import I2C

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

#class to handle interface with mpu device
class mpu:
    def __init__(self, freq=400000):
        self.i2c = I2C(freq=freq)
        self.slave = i2c.scan()
        self.accel_buf  = bytearray(6)
        self.gyro_buf   = bytearray(6)
        self.sensor_buf = bytearray(14)
        self.init_mpu()


    def turn_off(self):
        self.i2c.deinit()

    def write_reg(self, reg_addr, data):
        if uctypes.size_of(data) > 1:
            raise ValueError('Only write 1 Byte of data to a register')
        else:
            self.i2c.writeto_mem(self.slave,reg_addr,data)
 
    def read_reg(self, reg_addr, size):
        self.i2c.readfrom_mem(self.slave,reg_addr,size)   

    def read_sensor_reg(self):
        self.i2c.readfrom_mem_into(self.slave,ACCEL_X_H_REG,self.sensor_buf)

    def init_mpu(self):
        self.write_reg(SMPLRT_DIV   , b'00000000') #8kHz sampling rate
        self.write_reg(CONF         , b'00000000') #no external sync, largest bandwidth
        self.write_reg(GYRO_CONF    , b'00011000') #2000deg/s gyro range
        self.write_reg(ACCEL_CONF   , b'00011000') #16g accel range
        self.write_reg(INT_EN       , b'00000001') #16g accel range
        self.write_reg(INT_PIN_CFG  , b'10100000') #16g accel range
       
 
